#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "netif/etharp.h"
#include "netif/etharp.h"

//extern void echo_application_thread(void *pvParameters);


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}



int app_main(void)
{
    // To ensure proper MMU setup, not needed
    //cpu_configure_region_protection();
    nvs_flash_init();
    //system_init();
    printf("Initiated\n");


    return 0;
}

