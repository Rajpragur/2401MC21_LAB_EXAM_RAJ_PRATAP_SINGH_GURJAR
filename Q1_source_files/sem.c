#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

#define SEMA_TOTAL 8
static struct {
  struct spinlock lk;
  struct {
    int val;
    int used;
  } semaphore[SEMA_TOTAL];
} sem_table;

void seminittable(void) {
  initlock(&sem_table.lk, "semtable");
}

int sem_init(int id, int val) {
  if(id < 0 || id >= SEMA_TOTAL || val < 0) {
    return -1;
  }
  acquire(&sem_table.lk);
  sem_table.semaphore[id].used = 1;
  sem_table.semaphore[id].val = val;
  if(val > 0) {
    wakeup(&sem_table.semaphore[id]);
  }
  release(&sem_table.lk);
  return 0;
}

int sem_wait(int id) {
  if(id < 0 || id >= SEMA_TOTAL) {
    return -1;
  }
  acquire(&sem_table.lk);
  if(!sem_table.semaphore[id].used) {
    release(&sem_table.lk);
    return -1;
  }
  while(sem_table.semaphore[id].val == 0) {
    if(myproc()->killed) {
      release(&sem_table.lk);
      return -1;
    }
    sleep(&sem_table.semaphore[id], &sem_table.lk);
  }
  sem_table.semaphore[id].val--;
  release(&sem_table.lk);
  return 0;
}

int sem_post(int id) {
  if(id < 0 || id >= SEMA_TOTAL) {
    return -1;
  }
  acquire(&sem_table.lk);
  if(!sem_table.semaphore[id].used || sem_table.semaphore[id].val == 0x7fffffff) {
    release(&sem_table.lk);
    return -1;
  }
  sem_table.semaphore[id].val++;
  wakeup(&sem_table.semaphore[id]);
  release(&sem_table.lk);
  return 0;
}
