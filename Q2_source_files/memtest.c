#include "types.h"
#include "stat.h"
#include "user.h"

int global_variable = 123;

int main(void) {
  int ready[2], release[2], bef, dur, aft, pid;
  char byte = 'x';
  printf(1, "Free pages at start: %d\n", freepages());
  if(sbrk(10 * 4096) == (char *)-1) {
    printf(2, "sbrk failed\n");
    exit();
  }
  printf(1, "Free pages after sbrk: %d\n", freepages());
  printf(1, "Global VA: 0x%x PA: 0x%x\n", (uint)&global_variable, va2pa((int)&global_variable));
  printf(1, "va2pa(0x7fffffff): %d\n", va2pa(0x7fffffff));

  if(pipe(ready) < 0) {
    printf(2, "pipe failed\n");
    exit();
  }
  if(pipe(release) < 0) {
    printf(2, "pipe failed\n");
    close(ready[0]);
    close(ready[1]);
    exit();
  }
  bef = freepages();
  printf(1, "Free pages before fork: %d\n", bef);
  pid = fork();
  if(pid < 0) {
    printf(2, "fork failed\n");
    exit();
  }
  if(pid == 0) {
    close(ready[0]);
    close(release[1]);
    printf(1, "Child global VA: 0x%x PA: 0x%x\n", (uint)&global_variable,
           va2pa((int)&global_variable));
    write(ready[1], &byte, 1);
    read(release[0], &byte, 1);
    close(ready[1]);
    close(release[0]);
    exit();
  }
  close(ready[1]);
  close(release[0]);
  if(read(ready[0], &byte, 1) != 1) {
    printf(2, "child synchronization failed\n");
    wait();
    exit();
  }
  dur = freepages();
  printf(1, "Parent global VA: 0x%x PA: 0x%x\n", (uint)&global_variable,
         va2pa((int)&global_variable));
  printf(1, "Free pages while child lives: %d\n", dur);
  write(release[1], &byte, 1);
  wait();
  aft = freepages();
  printf(1, "Free pages after wait: %d\n", aft);
  printf(1, "Restored to pre-fork count: %s\n", aft == bef ? "yes" : "NO");
  close(ready[0]);
  close(release[1]);
  exit();
}
