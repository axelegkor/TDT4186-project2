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
typedef struct node_t {
    pid_t pid;
    //char *command;
    struct node_t *next;
} background_tasks;


background_tasks *head = NULL;

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

int get_status(pid_t PID) {
    int stat;
    if (waitpid(PID, &stat, WNOHANG))
    {
        if (WIFEXITED(stat))
        {
            return WEXITSTATUS(stat);
        }
    }
    return -1;
}

/**
 * @brief Adds a new node to linked list
 * @param the pid of the node the head of list and command line
 * @return the updated head
 * 
 */
background_tasks * add_background_task(pid_t pid, background_tasks *head) {
    background_tasks *new_node;
    new_node = (background_tasks *) malloc(sizeof(background_tasks));
    new_node->pid = pid;
    new_node->next= head;
    head = new_node;
    return head;
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

void print_all_tasks() {
    background_tasks *current_node = head;
    printf("\n\n\n--- ALL tasks ---\n");
   	while ( current_node != NULL) {
        printf("PID: %d, status: %d\n",current_node->pid, get_status(current_node->pid));
        current_node = current_node->next;
    }
}
void print_active_tasks(background_tasks *head) {
    background_tasks *current_node = head;
    printf("--- active tasks ---\n");
   	while ( current_node != NULL) {
        int status = get_status(current_node->pid);
        if(status != -1) {
            printf("PID: %d, status: %d\n",current_node->pid, status); 
        }
        current_node = current_node->next;
    }
}

void print_zombie_tasks(background_tasks *head) {
    background_tasks *current_node = head;
    printf("--- zombie tasks ---\n");
   	while ( current_node != NULL) {
        int status = get_status(current_node->pid);
        if(status == -1) {
            printf("PID: %d\n",current_node->pid);
        }
        /*
        if(get_status(current_node->pid) == -1) {
            //printf("Zombie: %d, status: %d\n",current_node->pid, get_status(current_node->pid));

        }
        */
        current_node = current_node->next;
    }
}

background_tasks * remove_and_print_zombie_tasks(background_tasks *head) {
    background_tasks *current_node = head;
    background_tasks *prev_node;
    while ( current_node != NULL) {
        if (get_status(current_node->pid) != -1) {
            printf("zombie: %d", current_node->pid);
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
            head = add_background_task(pid, head);
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

/*
void print_zombie_tasks(Node *head) {
    Node *current_node = head;
   	while ( current_node != NULL) {
        printf("%d ", current_node->data);
        current_node = current_node->next;
}
*/

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
            print_active_tasks(head);
        else if (strcmp(handeled_input[0], "cd") == 0)
            handle_cd();
        else
            syscmd_exec(handeled_input, input_str_copy); 
    }

    print_all_tasks(head);
    // char* argument_list[] = {"/bin/echo", "test", NULL};

    // if (execvp("/bin/echo", argument_list) < 0)
    //     printf("could not do it");

    return 0;
}
