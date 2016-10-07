#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"
extern app_main();


static void main_task(void* args)
{
    app_main();
    vTaskDelete(NULL);
}

int main(int argc,char *argv[])
{
    //xTaskCreatePinnedToCore(&main_task, "main",...)
    xTaskHandle hmainTask;

    xTaskCreate( main_task, "UDPRxTx", configMINIMAL_STACK_SIZE, NULL /*task param*/ , tskIDLE_PRIORITY + 1, &hmainTask );

    vTaskStartScheduler();
}


/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* The co-routines are executed in the idle task using the idle task hook. */
    vCoRoutineSchedule();	/* Comment this out if not using Co-routines. */

#ifdef __GCC_POSIX__
    struct timespec xTimeToSleep, xTimeSlept;
        /* Makes the process more agreeable when using the Posix simulator. */
        xTimeToSleep.tv_sec = 1;
        xTimeToSleep.tv_nsec = 0;
        nanosleep( &xTimeToSleep, &xTimeSlept );
#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void vMainQueueSendPassed( void )
{
    /* This is just an example implementation of the "queue send" trace hook. */
    //uxQueueSendPassedCount++;
}
/*-----------------------------------------------------------*/
