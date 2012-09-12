/* The main function in this file is called by the scheduler
 * before beginning task switching.  The end user should use
 * this function to create tasks that should run at boot, or
 * to perform any other actions that need to take place at the
 * end of boot. 
 *
 * main() is defined in kernel/sched.h */

#include <stddef.h>
#include <stdio.h>
#include <dev/resource.h>
#include <dev/shared_mem.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>

static void unit_tests(void);
static void ipctest(void);
static void memreader(void);

struct semaphore abandoned_sem = {
    .lock = 0,
    .held_by = NULL,
    .waiting = NULL
};

void abandon(void);
void attempt_acquire(void);

int ipc_success = 0;

void main(void) {
    new_task(&unit_tests, 1, 0);
}

void unit_tests(void) {
    //printf("Print Test...Test passed.\r\n");

    //printf("IPC Test...");
    //ipctest();

    printf("Abandoned semaphore test...");
    abandon();
}

void ipctest() {
    rd_t memrd = open_shared_mem();
    swrite(memrd, "IPC Test passed.\r\n");
    new_task(&memreader, 3, 0);
}

void memreader(void) {
    char buf[20];
    rd_t memrd = curr_task->task->top_rd - 1;

    read(memrd, buf, 18);
    buf[19] = 0x00;

    puts(buf);

    close(memrd);
}

void abandon(void) {
    acquire(&abandoned_sem);
    new_task(&attempt_acquire, 1, 0);
}

void attempt_acquire(void) {
    acquire(&abandoned_sem);
    printf("Abandoned semaphore test passed.\r\n");
    release(&abandoned_sem);
}
