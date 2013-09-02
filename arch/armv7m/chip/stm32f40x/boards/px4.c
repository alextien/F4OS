#include <arch/chip/gpio.h>
#include <dev/hw/led.h>
#include <board_config.h>

/* There is no lis302dl, so config is empty */
struct lis302dl_accel_config lis302dl_accel_config;

struct hmc5883_mag_config hmc5883_mag_config = {
    .valid = BOARD_CONFIG_VALID_MAGIC,
    .parent_name = "i2c2",
};

/* GPIO LEDs available - Red, Blue */
const struct led leds_avail[] = {{STM32F4_GPIO_PB14, 0}, {STM32F4_GPIO_PB15, 0}};

const int num_leds = sizeof(leds_avail)/sizeof(leds_avail[0]);
