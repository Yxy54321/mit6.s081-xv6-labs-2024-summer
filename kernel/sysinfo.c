#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "stat.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_sysinfo(void){

  struct proc *p = myproc();
  uint64 addr; // user pointer to struct sysinfo
  struct sysinfo si;

  si.freemem = cntfreebytes();
  si.nproc = cntproc();

  argaddr(0, &addr);
  if(copyout(p->pagetable, addr, (char *)&si, sizeof(si)) < 0)
      return -1;
  return 0;
}