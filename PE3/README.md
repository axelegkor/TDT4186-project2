# PE 3

## Links helpful shell-ex: 
- https://www.geeksforgeeks.org/making-linux-shell-c/
- https://github.com/kalpishs/Unix-Shell-Implementation-in-C

## Waitpid() ex
From https://www.ibm.com/docs/en/ztpf/1.1.0.15?topic=apis-waitpidobtain-status-information-from-child-process:
z
```
#include <tpf/sysapi.h>
#include <signal.h>
#include <sys/wait.h>
⋮

pid_t child_pid;
pid_t rc_pid;
int   chld_state;
int   my_opt=WNOHANG;
⋮
/* Create a child process*/
child_pid = tpf_fork(&create_parameters);
⋮
/* Check status of child process*/
rc_pid = waitpid( child_pid, &chld_state, WNOHANG);
if (rc_pid > 0)
{
  if (WIFEXITED(chld_state)) {
    printf("Child exited with RC=%d\n",WEXITSTATUS(chld_state));
  }
  if (WISIGNALED(chld_state)) {
    printf("Child exited via signal %d\n",WTERMSIG(chld_state));
  }
}
else
/* If no error, continue*/
{
  if (rc_pid < 0) {
    if (errno == ECHILD) {
      printf("Child does not exist\n");
    }
    else {
      printf("Bad argument passed to waitpid\n");
      abort();
    }
  }
}
⋮
```