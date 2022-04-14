#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256
#define ARGS_BUFFER 30

// The input from user splitted on spaces
char *handeled_input[ARGS_BUFFER];
char input_str_copy[BUFFER_SIZE];

/**
 * @brief Tuple for background tasks
 * 
 */
typedef struct background_tasks
{
    pid_t pid;
    char command[BUFFER_SIZE];
} background_tasks;

void print_shell()
{
    printf("\n\n\n******************"
           "************************");
    printf("\n\n\n****The famous little unix shell****");
    printf("\n\n\t- USE AT YOUR OWN RISK :) -");
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

void handle_cd()
{
    if (strlen(handeled_input[1]) == 0)
    {
        printf("The path is empty.\n");
        return;
    }
    printf("Dir: %s\n", handeled_input[1]);
    printf("Correct\n");
    chdir(handeled_input[1]); // Changes the dir to the one stored in data[1]
}

void add_backgroundtask(int pid, char *command)
{
    background_tasks *bt = malloc(sizeof(struct background_tasks));

    bt->pid = pid;
    *bt->command = command;
}

void print_backgroundtasks()
{
    struct background_tasks bt[3];
    for (int z = 0; z < 2; z++) {
        printf("%d pid: %d -> command: %s\n", z, bt->pid, bt->command);
    }
    // struct background_tasks *ptr = 0;
    // while (ptr != NULL)
    // {
    //     printf("[pid %d] %s\n", ptr->pid, ptr->command);
    //     ptr = ptr++;
    // }
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

/**
 * 
 * @return 1 if there are redirections (< or >), 0 otherwise
 */
int IO_redirection()
{
    for (int i = 0; i < ARGS_BUFFER; i++)
    {
        if (strcmp(handeled_input[i], ">"))
            return 1;
        if (strcmp(handeled_input[i], "<"))
            return 1;
    } 
    return 0;
}

int check_backgroundtask()
{
    for (int x = 0; x < ARGS_BUFFER; x++){
        if (handeled_input[x] == NULL) {             
            if (strstr(handeled_input[x-1], "&"))
            {
                return 1;
            }
            return 0;
        }
    }
}

void syscmd_exec(char **command, char *input)
{
    pid_t pid = fork();
    
    if (pid < 0)
    {
        printf("Could not fork a child.\n");
        return;
    }
    if (pid > 0)
    {
        if (check_backgroundtask())
        {
            add_backgroundtask(pid, input);
        }

        else 
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
                printf("Exit status [%s] = %d\n", input, es);
            }
        }
    }
    else if (pid == 0)
    {   
        if (IO_redirection())
        {
            execl("/bin/sh", "/bin/sh", "-c", input, NULL);
        }
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

    printf("\033[1;33m");
    print_shell();
    
    while (1) 
    {
        printf("\033[1;32m");
        print_dir();
        printf(": ");
        printf("\033[0m");
        fgets(input_str, BUFFER_SIZE, stdin);
        // fflush(stdin);
        strcpy(input_str_copy, input_str);     // Copies the string taken from the user to be printed in Ecit status
        strtok(input_str_copy, "\n\r");        // Removes any unawanted space-like characters

        handle_input(input_str);

        if (strcmp(handeled_input[0], "jobs") == 0)
            print_backgroundtasks();
        else if (strcmp(handeled_input[0], "cd") == 0)
            handle_cd();
        else
            syscmd_exec(handeled_input, input_str_copy);
    }

    // char* argument_list[] = {"/bin/echo", "test", NULL};

    // if (execvp("/bin/echo", argument_list) < 0)
    //     printf("could not do it");

    return 0;
}
