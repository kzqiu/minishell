#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"

void cd(char *cmd, char *cwd) {
    char *new_path = cmd + 3;

    if (new_path[0] == '\0' || new_path[0] == '\n') { // Case: go to home dir
        struct passwd *user;
        if ((user = getpwuid(getuid())) == NULL) {
            fprintf(stderr, "\nError: Could not get the current user's information. %s.\n", strerror(errno));
        } else {
            chdir(user->pw_dir);
        }
    } else { // Case: go to new_path (abs and relative)
        new_path[strlen(new_path) - 1] = '\0';
        if (chdir(new_path) != 0) {
            fprintf(stderr, "\nError: Could not change directory. %s.\n", strerror(errno));
        }
    }
}

void execute(char *cmd, char *cwd) {
    if (cmd[0] == 'c' && cmd[1] == 'd' && (cmd[2] == ' ' || cmd[2] == '\n')) { // Case 1: cd
        cd(cmd, cwd);
    } else if (cmd[0] == 'e' && cmd[1] == 'x' && cmd[2] == 'i' && cmd[3] == 't' && (cmd[4] == ' ' || cmd[4] == '\n')) { // Case 2: exit
        printf("\n");
        exit(0);
    } else { // Case 3: Non-shell builtins        
        cmd[strlen(cmd) - 1] = '\0';
        char *tokens[2048];
        char *tmp = strtok(cmd, " ");
        int argc = 0;

        while (tmp != NULL) {
            tokens[argc] = (char *) malloc(strlen(tmp) + 1);
            strcpy(tokens[argc], tmp);
            argc++;
            tmp = strtok(NULL, " ");
        }

        tokens[argc] = NULL;

        // char **test = tokens;

        // while (*test != NULL) {
        //     printf("%s ", *test);
        //     test++;
        // }
       
        pid_t pid;

        if ((pid = fork()) < 0) {
            fprintf(stderr, "Error: fork failed. %s.\n", strerror(errno));
            return;
        } else if (pid == 0) { // Child
            execvp(tokens[0], tokens);
            fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));            
            exit(EXIT_FAILURE);
        }

        waitpid(pid, NULL, 0);

        // Free all malloced tokens
        for (int i = 0; i < argc; i++) free(tokens[i]);
    }
}

int main(int argc, char **argv) {
    char cwd[PATH_MAX + 1];
    char command[4096 + 1];

    // Main loop of minishell
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "Error: Could not get current directory. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // printing current working directory and wait for user input
        printf("\n[%s%s%s]$ ", BRIGHTBLUE, cwd, DEFAULT);
        fgets(command, sizeof(command), stdin);

        execute(command, cwd);
    }

    return EXIT_SUCCESS;
}
