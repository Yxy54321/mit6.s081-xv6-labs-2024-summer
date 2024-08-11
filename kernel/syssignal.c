#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

void
switchctx()
{
    struct proc* p= myproc();
    struct trapframe copy = *(p->trapframe); 
    *(p->trapframe)=p->almctx;
    p->trapframe-> kernel_satp = copy.kernel_satp;
    p->trapframe-> kernel_sp = copy.kernel_sp;
    p->trapframe-> kernel_trap = copy.kernel_trap;
    p->trapframe-> kernel_hartid = copy.kernel_hartid;
    p->trapframe-> a0 = copy.a0;
}

uint64 
sys_sigalarm(void)
{
    struct proc* p= myproc();
    int itv;
    uint64 handler;
    argint(0,&itv);
    argaddr(1,&handler);

    p->interval = itv;
    p->handler = handler;
    p->ticks=0;
    return 0;
};

uint64 
sys_sigreturn(void)
{
    struct proc* p= myproc();
    p->flag=0;
    switchctx();
    return 0;
};