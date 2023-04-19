#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <signal.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"

/*
    cd - change directory

    cmd: full command passed to shell string
    cwd: current working directory string
*/
void cd(char *cmd, char *cwd) {
    char *new_path = cmd + 3;

    if (new_path[0] == '\0' || new_path[0] == '\n') { // Case: go to home dir
        struct passwd *user;
        if ((user = getpwuid(getuid())) == NULL) {
            fprintf(stderr, "\nError: Cannot get passwd entry. %s.\n", strerror(errno));
        } else {
            chdir(user->pw_dir);
        }
    } else { // Case: go to new_path (abs and relative)
        new_path[strlen(new_path) - 1] = '\0';
        for (int i = 0; i < strlen(new_path); i++) {
            if (new_path[i] == ' ') {
                fprintf(stderr, "\nError: Too many arguments to cd.\n");
            }
        }
        if (chdir(new_path) != 0) {
            fprintf(stderr, "\nError: Cannot change directory to '%s'. %s.\n", new_path, strerror(errno));
        }
    }
}

/*
    execute - executes a function in a child process

    cmd: full command passed to shell string
    cwd: current working directory string
*/
void execute(char *cmd, char *cwd) {
    if (cmd[0] == 'c' && cmd[1] == 'd' && (cmd[2] == ' ' || cmd[2] == '\n')) { // Case 1: cd
        cd(cmd, cwd);
    } else if (cmd[0] == 'e' && cmd[1] == 'x' && cmd[2] == 'i' && cmd[3] == 't' && (cmd[4] == ' ' || cmd[4] == '\n')) { // Case 2: exit
        printf("\n");
        exit(0);
    } else { // Case 3: Non-shell builtins        
        printf("\n");
        cmd[strlen(cmd) - 1] = '\0';
        char *tokens[2048];
        char *tmp = strtok(cmd, " ");
        int argc = 0;

        while (tmp != NULL) {
            if ((tokens[argc] = (char *) malloc(strlen(tmp) + 1)) == NULL) {
                fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
            }
            strcpy(tokens[argc], tmp);
            argc++;
            tmp = strtok(NULL, " ");
        }

        tokens[argc] = NULL;
      
        pid_t pid;

        if ((pid = fork()) < 0) {
            fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
            return;
        } else if (pid == 0) { // Child
            execvp(tokens[0], tokens);
            fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));            
            exit(EXIT_FAILURE);
        }

        if (waitpid(pid, NULL, 0) == -1 && errno != EINTR) {
            fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));            
        }

        // Free all malloced tokens
        for (int i = 0; i < argc; i++) free(tokens[i]);
    }
}


volatile sig_atomic_t interrupted = 0;

/*
    handler - handles interrupt signal in shell
*/
static void handler(int sig, siginfo_t *siginfo, void *context) {
    interrupted = 1;
}

/*
    main - does stuff
*/
int main(int argc, char **argv) {
    struct sigaction act;

    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = handler;

    // act.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &act, NULL) == -1) {
        // Handle sigaction error
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }

    char cwd[PATH_MAX + 1];
    char command[4096 + 1];

    // Main loop of minishell
    while (1) {
        if (interrupted) {
            interrupted = 0;
        } else {
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // printing current working directory and wait for user input
            printf("\n[%s%s%s]$ ", BRIGHTBLUE, cwd, DEFAULT);
            if (fgets(command, sizeof(command), stdin) != NULL) {
                execute(command, cwd);
            } else {
                if (errno == EINTR) {
                    printf("\n");
                    errno = 0;
                } else if (feof(stdin)) {
                    printf("\n");
                    return EXIT_SUCCESS;
                } else if (ferror(stdin)) {
                    fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
