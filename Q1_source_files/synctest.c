#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
  int i, j, pid, child[3], written = 0;
  for(i = 0; i < 3; i++) {
    if(sem_init(i, i == 0) < 0) {
      printf(2, "sem_init failed\n");
      exit();
    }
  }
  for(i = 0; i < 3; i++) {
    pid = fork();
    if(pid < 0) {
      printf(2, "fork failed\n");
      for(j = 0; j < written; j++) {
        kill(child[j]);
      }
      while(written-- > 0) {
        wait();
      }
      exit();
    }
    if(pid == 0) {
      for(j = 0; j < 5; j++) {
        if(sem_wait(i) < 0) {
          exit();
        }
        printf(1, "%c ", 'A' + i);
        if(sem_post((i + 1) % 3) < 0) {
          exit();
        }
      }
      exit();
    }
    child[written++] = pid;
  }
  for(i = 0; i < 3; i++) {
    wait();
  }
  printf(1, "\nDone\n");
  exit();
}
