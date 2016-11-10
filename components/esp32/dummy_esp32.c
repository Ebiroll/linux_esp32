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
