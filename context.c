/* Context switching fro F4OS!
 * Michael Pratt <michael@pratt.im> */

#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "mem.h"
#include "context.h"
#include "heap.h"
#include "mpu.h"
#include "usermode.h"

void user_prefix(void) {
    uint32_t *memory;

    /* Allocate memory for the stack, must be aligned to stack size for MPU */
    memory = malloc(STKSIZE*4, 1);

    /* Give unprivileged access to the allocated stack */
    *MPU_RNR = (uint32_t) (1 << USER_MEM_REGION);
    *MPU_RBAR = (uint32_t) memory;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE((mpu_size(STKSIZE*4))) | MPU_RASR_SHARE_NOCACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RW | MPU_RASR_XN;
    /* MPU Base address must be aligned with the MPU region size */

    /* Switch to the process stack and set it to the
     * top of the allocated memory */
    enable_psp(memory+STKSIZE);

    unprivileged_test();

    /* Raise privilege */
    _svc(0);

    /* Test context switching */
    //_svc(1);

    disable_psp();
}

void svc_handler(uint32_t *svc_args) {
    uint32_t svc_number;
    uint32_t return_address;

    uint32_t *psp_addr;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument (r0) is svc_args[0] */
    svc_number = ((char *)svc_args[6])[-2];
    return_address = svc_args[6];

    switch (svc_number) {
        case 0x0:
            /* Raise Privilege, but only if request came from the kernel */
            /* DEPRECATED: All code executed until _svc returns is privileged,
             * so raising privileges shouldn't ever be necessary. */
            if (return_address >= (uint32_t) &_skernel && return_address < (uint32_t) &_ekernel) {
                raise_privilege();
            }
            else {
                panic();
            }
            break;
        case 0x1:
            /* Save context, do something, and restore context */
            psp_addr = save_context();
            asm ("mov r5, #5    \n");
            restore_context(psp_addr);
        default:
            break;
    }
}
