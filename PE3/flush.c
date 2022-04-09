#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256
#define ARGS_BUFFER 15

char *handeled_input[ARGS_BUFFER];

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

void handle_input(char input[ARGS_BUFFER])
{
    int i = 0;
    char delim[] = " ";
    char *ptr = strtok(input, delim);

    while (ptr != NULL)
    {
        handeled_input[i++] = ptr;
        ptr = strtok(NULL, delim);
    }

    for (int x = 0; x < ARGS_BUFFER; x++) 
    {
        strtok(handeled_input[x], "\n\r");
    }

    for (int p = 0; p < ARGS_BUFFER; p++){
        if (handeled_input[p] == NULL) { 
            printf("%d er NULL\n\n", p);
            break;
        }
        printf(":%d -> %s:\n", p, handeled_input[p]);
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
        printf("\033[1;32m");
        print_dir();
        printf(": ");
        printf("\033[0m");
        fgets(input_str, BUFFER_SIZE, stdin);
        // fflush(stdin);

        handle_input(input_str);

        // if (strcmp(handeled_input[0], "cd") == 0)
        //     handle_cd(handeled_input);
        // else
            syscmd_exec(handeled_input);
    }

    // char* argument_list[] = {"/bin/echo", "test", NULL};

    // if (execvp("/bin/echo", argument_list) < 0)
    //     printf("could not do it");

    return 0;
}
