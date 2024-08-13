// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  int pagenums;
  char* ref_pages;
  char *ends;
} kmem;

void get_pagenums(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    kmem.pagenums++;  //引用计数
  }
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  
  get_pagenums(end, (void*)PHYSTOP);
  kmem.ref_pages=end;
  for (int i = 0; i < kmem.pagenums; i++)
  {
    kmem.ref_pages[i]=0;
  }
  kmem.ends=kmem.ref_pages+kmem.pagenums;

  freerange(kmem.ends, (void*)PHYSTOP);
  //printf("pagenums=%d\n",kmem.pagenums);
}

int page_index(uint64 pa)
{
  pa=PGROUNDDOWN(pa);
  int ans=(pa-(uint64)kmem.ends)/PGSIZE;
  if(ans<0 || ans>kmem.pagenums)
  {
    printf("ans: %d, pa: %p, kmem.ends : %p",ans,pa,kmem.ends);
    panic("page_index failed!\n");
  }
  return ans;
}

void increase(void *pa)
{
  int index=page_index((uint64)pa);
  acquire(&kmem.lock);
  kmem.ref_pages[index]++;
  release(&kmem.lock);
}

void decrese(void *pa)
{
  int index=page_index((uint64)pa);
  acquire(&kmem.lock);
  kmem.ref_pages[index]--;
  release(&kmem.lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    kfree(p);
  }
    
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  int index = page_index((uint64)pa);
  if(kmem.ref_pages[index]>1)
  {
    decrese(pa);
    return ;
  }
  if(kmem.ref_pages[index]==1)
  {
    decrese(pa);
  }

  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
  {
    memset((char*)r, 5, PGSIZE); // fill with junk
    increase(r);
  }
  
  return (void*)r;
}