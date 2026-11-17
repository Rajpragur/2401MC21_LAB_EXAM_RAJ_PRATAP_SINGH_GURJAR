#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[];
struct run {
  struct run *next;
};

struct {
  struct spinlock lk;
  int use_lock;
  int free_cnt;
  struct run *freelist;
} kmem;

void kinit1(void *vstart, void *vend) {
  initlock(&kmem.lk, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void kinit2(void *vstart, void *vend) {
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void freerange(void *vstart, void *vend) {
  char *p;
  p = (char *)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char *)vend; p += PGSIZE) {
    kfree(p);
  }
}

void kfree(char *v) {
  struct run *r;
  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP) {
    panic("kfree");
  }
  memset(v, 1, PGSIZE);
  if(kmem.use_lock) {
    acquire(&kmem.lk);
  }
  r = (struct run *)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  kmem.free_cnt++;
  if(kmem.use_lock) {
    release(&kmem.lk);
  }
}

char *kalloc(void) {
  struct run *r;

  if(kmem.use_lock) {
    acquire(&kmem.lk);
  }
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
    kmem.free_cnt--;
  }
  if(kmem.use_lock) {
    release(&kmem.lk);
  }
  return (char *)r;
}

int kfreepages(void) {
  int count;
  if(kmem.use_lock) {
    acquire(&kmem.lk);
  }
  count = kmem.free_cnt;
  if(kmem.use_lock) {
    release(&kmem.lk);
  }
  return count;
}
