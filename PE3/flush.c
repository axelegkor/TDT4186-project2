#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void print_shell()
{
    printf("\n\n\n******************"
           "************************");
    printf("\n\n\n****The famous little unix shell****");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n*******************"
           "***********************\n\n");
}

void handle_input(char input[256])
{ 
    int i = 0;
    char *usr_input = strtok(input, " ");
    char *data[2];

    while (usr_input)
    {
        data[i++] = usr_input;
        usr_input = strtok(NULL, " ");
    }

    for (i = 0; i < 2; i++)
    {
        printf("%s\n", data[i]);
    }
}

void syscmd_exec(char **command)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Could not fork a child.\n");
        return;
    }
    else if (pid == 0)
    {
        printf("child");
        if (execvp(command[0], command) < 0)
        {
            printf("The command coudl not be executed.\n");
        }
        else {
           int status;

            if(waitpid(pid, &status, 0) == -1) {
                printf("Waitpid failed");
                exit(EXIT_FAILURE);
            }
            if ( WIFEXITED(status) ) {
                int es = WEXITSTATUS(status);
                printf("Exit status was %d\n", es);
            }
        }
        exit(0);
    }
    else
    {
        wait(NULL);
        return;
    }
}

int main()
{
    // Finds the current current working directory
    char path[200];
    getcwd(path, 200);

    char input_str[256];

    // print_shell();

    printf("%s: ", path);
    fgets(input_str, 256, stdin);

    handle_input(input_str);

    return 0;
}
