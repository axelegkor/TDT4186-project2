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


void handle_input(char input[256], char* strings[], size_t size)
{ 
    int i = 0;
    char *usr_input = strtok(input, " ");
    char *data[2]; // Contains the input from the user, data[0]: commands and data[1]: arguments

    while (usr_input)
    {
        data[i++] = usr_input;
        usr_input = strtok(NULL, " ");
    }

    // Handles if the argument contains "\n"
    strtok(data[1], "\n");   // Cleaner way to handle this: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input

    if (strcmp(data[0], "cd") == 0)
    {
        if (strlen(data[1]) == 0)
        {
            printf("The path is empty.\n");
            return;
        }

        printf("Dir: %s\n", data[1]);
        printf("Correct\n");
        chdir(data[1]); // Changes the dir to the one stored in data[1]
    }

    if (size >= 3)
    {
        strings[0] = data[0];
        strings[1] = data[1];
        strings[3] = NULL;
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
    else if (pid > 0)
    {
        int status;
            
        if (waitpid(pid, &status, 0) == -1) 
        {
            printf("Waitpid failed.\n");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) 
        {
            int es = WEXITSTATUS(status);
            printf("Exit status was %d\n", es);
        }
    }
    else if (pid == 0)
    {
        execvp(command[0], command);
        printf("The command could not be executed.\n");
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
    char *handeled_input[3];

    while (1) 
    {
        print_dir();
        printf(": ");
        fgets(input_str, 256, stdin);

        handle_input(input_str, handeled_input, sizeof(handeled_input));

        // for (int i = 0; i < 3; i++) {
        //     printf("h:%s, %d\n", handeled_input[i], i);
        // }

        if (strcmp(handeled_input[0], "cd") != 0)
            syscmd_exec(handeled_input);
    }

    // char* argument_list[] = {"/bin/echo", "test", NULL};

    // if (execvp("/bin/echo", argument_list) < 0)
    //     printf("could not do it");

    return 0;
}
