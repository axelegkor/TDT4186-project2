#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

char *handeled_input[BUFFER_SIZE];

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
    char cwd[BUFFER_SIZE];
    getcwd(cwd, sizeof(cwd));
    printf("%s", cwd);
}

void handle_cd(char *handled_input[])
{
    if (strcmp(handled_input[0], "cd") == 0)
    {
        if (strlen(handled_input[1]) == 0)
        {
            printf("The path is empty.\n");
            return;
        }
        printf("Dir: %s\n", handled_input[1]);
        printf("Correct\n");
        chdir(handled_input[1]); // Changes the dir to the one stored in data[1]
    }
}

void handle_input(char input[BUFFER_SIZE])
{ 
    int i = 0;
    char *usr_input = strtok(input, " ");
    char *data[BUFFER_SIZE]; // Contains the input from the user, data[0]: commands and data[1]: arguments

    while (usr_input)
    {
        data[i++] = usr_input;
        usr_input = strtok(NULL, " ");
    }

    // Handles if the argument contains "\n"
    strtok(data[1], "\n\r");   // Cleaner way to handle this: https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input

    int empty_index = 0;    
    for (int x = 0; x < 255; x++)
    {
        if (data[x] == NULL) 
        {
            empty_index = x;
            break;
        }
    }
    
    for (int q = 0; q < empty_index - 1; q++)
    {
        handeled_input[q] = data[q];
    }
    handeled_input[empty_index] = NULL;

    for (int p = 0; p < 3; p++){
        printf("%d: %s\n", p, handeled_input[p]);
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
    char input_str[BUFFER_SIZE];

    while (1) 
    {
        print_dir();
        printf(": ");
        fgets(input_str, BUFFER_SIZE, stdin);

        handle_input(input_str);

        if (strcmp(handeled_input[0], "cd") == 0)
            handle_cd(handeled_input);
        else
            syscmd_exec(handeled_input);
    }

    // char* argument_list[] = {"/bin/echo", "test", NULL};

    // if (execvp("/bin/echo", argument_list) < 0)
    //     printf("could not do it");

    return 0;
}
