#include <stdbool.h> 
#include <sys/time.h>
#include <stddef.h>
#include <esp_system.h>


long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

/*
 * strlcpy - like strcpy/strncpy, doesn't overflow destination buffer,
 * always leaves destination null-terminated (for len > 0).
 */
size_t strlcpy(char *dest, const char *src, size_t len) {
    size_t ret = strlen(src);

    if (len != 0) {
	if (ret < len)
	    strcpy(dest, src);
	else {
	    strncpy(dest, src, len - 1);
	    dest[len-1] = 0;
	}
    }
    return ret;
}



void system_init(void)
{
}


uint32_t system_get_time(void)
{
    return(current_timestamp());
}

/* get CCOUNT register (if not present return 0) */
unsigned xthal_get_ccount(void) {
    return 0;
}

int xPortGetCoreID 	( 	void  		) 	{
    return 0;
}

uint32_t g_ticks_per_us_pro=1024;



esp_err_t esp_register_shutdown_handler(shutdown_handler_t handler)
{
    return(ESP_OK);
}