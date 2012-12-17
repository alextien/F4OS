#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <dev/resource.h>
#include <dev/registers.h>
#include <dev/hw/gpio.h>

#include <dev/hw/spi.h>
#include <dev/periph/discovery_accel.h>

typedef struct discovery_accel {
        spi_dev *spi_port;
        uint8_t read_ctr;
} discovery_accel;

extern spi_dev spi1;

char discovery_accel_read(void *env, int *error) __attribute__((section(".kernel")));
int discovery_accel_write(char d, void *env) __attribute__((section(".kernel")));
int discovery_accel_close(resource *env) __attribute__((section(".kernel")));
static void discovery_accel_cs_high(void) __attribute__((section(".kernel")));
static void discovery_accel_cs_low(void) __attribute__((section(".kernel")));

static void discovery_accel_cs_high(void) {
    *GPIO_ODR(GPIOE) |= GPIO_ODR_PIN(3);
}

static void discovery_accel_cs_low(void) {
    *GPIO_ODR(GPIOE) &= ~(GPIO_ODR_PIN(3));
}

rd_t open_discovery_accel(void) {
    /* --- Set up CS pin and set high ---*/
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

    /* PE3 */
    gpio_moder(GPIOE, 3, GPIO_MODER_OUT);
    gpio_otyper(GPIOE, 3, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOE, 3, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOE, 3, GPIO_OSPEEDR_50M);
    
    /* ------ Properly idle CS */
    discovery_accel_cs_high();

    discovery_accel *accel = kmalloc(sizeof(discovery_accel));
    if (!accel) {
        printk("OOPS: Could not allocate space for discovery accel resource.\r\n");
        return -1;
    }

    resource *new_r = create_new_resource();
    if(!new_r) {
        printk("OOPS: Could not allocate space for discovery accel resource.\r\n");
        kfree(accel);
        return -1;
    }

    /* We expect that spi1 was init'd in bootmain.c */
    accel->spi_port = &spi1;
    accel->spi_port->write(0x20, 0x47, &discovery_accel_cs_high, &discovery_accel_cs_low);
    accel->read_ctr = 0;

    new_r->env = accel;
    new_r->writer = &discovery_accel_write;
    new_r->reader = &discovery_accel_read;
    new_r->closer = &discovery_accel_close;
    new_r->sem = &spi1_semaphore;

    return add_resource(curr_task->task, new_r);
}

char discovery_accel_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    discovery_accel *accel = (discovery_accel *)env;
    if(accel->read_ctr > 2)
        accel->read_ctr = 0;
    return (char)accel->spi_port->read(0x29 + 2*accel->read_ctr++, &discovery_accel_cs_high, &discovery_accel_cs_low);
}

int discovery_accel_write(char d, void *env) {
    /* No real meaning to this yet */
    return -1;
}

int discovery_accel_close(resource *res) {
    kfree(res->env);
    return 0;
}