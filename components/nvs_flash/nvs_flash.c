#include <stdbool.h>
#include "esp_system.h"

/** Initialise NVS flash storage with default flash sector layout

    Temporarily, this region is hardcoded as a 12KB (0x3000 byte)
    region starting at 24KB (0x6000 byte) offset in flash.
*/
esp_err_t nvs_flash_init(void)
{
}
