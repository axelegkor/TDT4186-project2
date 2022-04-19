#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define ARGS_BUFFER 64
#define MAX_LEN 1024

// The input from user splitted on spaces
char *handeled_input[ARGS_BUFFER];
char input_str_copy[MAX_LEN];

/**
 * @brief Tuple for background tasks
 * 
 */
typedef struct node_t {
    pid_t pid;
    char *command;
    struct node_t *next;
} background_tasks;

// Keeps track of current head in linked list
background_tasks *head = NULL;


/**
 * @brief prints unix shell information
 * 
 */
void print_shell()
{
    printf("\n\n\n******************"
           "************************");
    printf("\n\n\n****The famous little unix shell****");
    printf("\n\n\t- ( ͡ᵔ ͜ʖ ͡ᵔ) -");
    printf("\n\n\n*******************"
           "***********************\n\n");
}

/**
 * @brief prints current directory 
 * 
 */
void print_dir() 
{
    // Finds the current current working directory
    char cwd[MAX_LEN];
    getcwd(cwd, sizeof(cwd));
    printf("%s", cwd);
}

/**
 * @brief Adds a new node to linked list
 * @param pid of the process to be executed
 * @param command the command to be executed
 * @param head the current head in the linked list
 * @return the updated head
 * 
 */
background_tasks * add_background_task(pid_t pid, char* command, background_tasks *head) {
    background_tasks *new_node;
    new_node = (background_tasks *) malloc(sizeof(background_tasks));
    new_node->pid = pid;
    new_node->next= head;
    new_node->command = malloc(sizeof(char)*MAX_LEN); 
    strcpy(new_node->command, command);
    head = new_node;
    return head;
}

/**
 * @brief changes directory to the given path
 * 
 */
void handle_cd()
{
    if (strlen(handeled_input[1]) == 0)
    {
        printf("The path is empty.\n");
        return;
    }
    chdir(handeled_input[1]); // Changes the dir to the one stored in data[1]
}


/**
 * @brief prints all active processes
 * 
 */
void print_all_tasks() {
    background_tasks *current_node = head;
    if(current_node != NULL) {
        printf("\n\n--- ALL tasks ---");
    }
   	while ( current_node != NULL) {
        printf("\nProcess ID: [%d] %s\n",current_node->pid, current_node->command);
        current_node = current_node->next;
    }
}

/**
 * @brief prints and removes executed processes
 * @param head current head in the linked list
 * @return updated head for the linked list
 * 
 */
background_tasks * remove_and_print_zombie_tasks(background_tasks *head) {
    background_tasks *current_node = head;
    background_tasks *prev_node;
    int status;
    int PID;
    while ( current_node != NULL) {
        PID = waitpid(current_node->pid, &status, WNOHANG | WUNTRACED);
        if (WIFEXITED(status))
        {
            printf("\nExit status [%s] = %d\n", current_node->command, WEXITSTATUS(status));
            if (current_node == head) {
                 head = current_node->next;
            } else {
                prev_node->next = current_node->next;
            }
        }
        prev_node = current_node;
        current_node = current_node->next;
    }
    return(head);
}

/**
 * @brief handles user input
 * @param input the input to be handled
 * 
 */
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
}

/**
 * @brief checks if there are any rediretions in the input string
 * @return 1 if there are redirections (< or >), 0 otherwise
 * 
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

/**
 * @brief checks if user adds a new background tasks
 * @return 1 if there is a new background task, 0 otherwise
 * 
 */
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

/**
 * @brief executes process
 * @param command the command to be executed
 * @param input the user input
 * 
 */
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
            head = add_background_task(pid, input, head);
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
        head = remove_and_print_zombie_tasks(head);
    }
    else if (pid == 0)
    {   
        if (IO_redirection())                                       // Check if the user input containd any redirections
        {
            execl("/bin/sh", "/bin/sh", "-c", input, NULL);         // Exec(3) funtion that executes rederiatons and allows you to write multiple redireations of the same type
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
    char input_str[MAX_LEN];

    printf("\033[1;33m");
    print_shell();
    
    while (1) 
    {
        printf("\033[1;32m");
        print_dir();
        printf(": ");
        printf("\033[0m");
        
        if (fgets(input_str, MAX_LEN, stdin) == NULL) 
        {
            printf("\nProgram was exited with EOF\n\n");        // Terminates program on control-D
            break;
        }

        strcpy(input_str_copy, input_str);     // Copies the string taken from the user to be printed in Exit status
        strtok(input_str_copy, "\n\r");        // Removes any unawanted space-like characters

        handle_input(input_str);

        if (strcmp(handeled_input[0], "jobs") == 0)
        {
            print_all_tasks(head);
            head = remove_and_print_zombie_tasks(head);
        }
        else if (strcmp(handeled_input[0], "cd") == 0)
            handle_cd();
        else
            syscmd_exec(handeled_input, input_str_copy); 
    }

    return 0;
}
