#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char **argv) {
    char cwd[PATH_MAX + 1];

    // Main loop of minishell
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("[%s]$", cwd);
        }
    }

    return EXIT_SUCCESS;
}
