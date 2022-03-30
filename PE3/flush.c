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

void print_dir() 
{
    // Finds the current current working directory
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    printf("%s", cwd);
}

void handle_input(char input[256])
{ 
    int i = 0;
    char *usr_input = strtok(input, " ");
    char *data[2];

    printf("før while: %s, %d\n", data[1], 1);

    while (usr_input)
    {
        data[i++] = usr_input;
        usr_input = strtok(NULL, " ");
    }

    printf("etter while: %s, %d\n", data[1], 1);

    for (i = 0; i < 2; i++)
    {
        printf("%s, %d\n", data[i], i);
    }

    if (strcmp(data[0], "cd") == 0)
    {
        printf("\nDir tastet inn: %s", data[1]);
        printf("rikitg\n");
        chdir(data[1]);
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
        if (execvp(command[0], command) < 0)
        {
            printf("The command coudl not be executed.\n");
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
    char input_str[256];
    // print_shell();

    while (1)
    {
        print_dir();
        printf(": ");
        fgets(input_str, 256, stdin);

        handle_input(input_str);
    }

    return 0;
}
