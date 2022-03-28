#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>
#include <readline/readline.h>


void print_shell() 
{
    printf("\n\n\n******************"
        "************************");
    printf("\n\n\n****The famous little unix shell****");
    printf("\n\n\t-USE AT YOUR OWN RISK-");
    printf("\n\n\n*******************"
        "***********************");
}

void input_usr(char* input) 
{
    strtok(input, " ");
    printf("%s", input);
}

void syscmd_exec(char** command) 
{
    pid_t pid = fork();

    if (pid < 0) {
        printf("Could not fork a child.\n");
        return;
    } else if (pid == 0) {
        if (execvp(command[0], command) < 0) {
            printf("The command coudl not be executed.\n");
        } exit(0);
    } else {
        wait(NULL);
        return;
    }
}

int main()
{    
    // Finds the current current working directory
    char path[200];
    getcwd(path, 200);
    char input_str[1000];
    
    printf("\n%s: ", path);
    scanf(":", input_str);

    // print_shell();
    input_usr(input_str);
    // scanf("\nWhat do you want to do? ");
    
    
    return 0;
}
