/*
 * Blinky Example for nRF9151 Feather / Myriota HyperPulse Dev Kit
 *
 * Este ejemplo hace parpadear el LED RGB integrado del nRF9151 Feather.
 * El LED esta conectado al pin D7 (P0.03 en el nRF9151).
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(blinky, LOG_LEVEL_INF);

/* Obtener la especificacion del LED desde el DeviceTree */
#define LED0_NODE DT_ALIAS(led0)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "LED0 node is not defined in the devicetree"
#endif

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* Intervalo de parpadeo en milisegundos */
#define BLINK_INTERVAL_MS 1000

int main(void)
{
    int ret;
    bool led_state = true;

    LOG_INF("Blinky example starting on nRF9151 Feather");
    LOG_INF("Board: %s", CONFIG_BOARD);

    /* Verificar que el GPIO esta listo */
    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("GPIO device not ready");
        return -1;
    }

    /* Configurar el pin del LED como salida */
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure LED pin: %d", ret);
        return ret;
    }

    LOG_INF("LED configured successfully. Starting blink loop...");

    /* Loop principal - parpadeo infinito */
    while (1) {
        /* Toggle del LED */
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            LOG_ERR("Failed to toggle LED: %d", ret);
            return ret;
        }

        led_state = !led_state;
        LOG_DBG("LED state: %s", led_state ? "ON" : "OFF");

        /* Esperar antes del siguiente toggle */
        k_msleep(BLINK_INTERVAL_MS);
    }

    return 0;
}
