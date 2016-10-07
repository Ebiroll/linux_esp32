cmake_minimum_required(VERSION 2.4)
PROJECT(linux_esp32)
#SET (CMAKE_BUILD_TYPE Release)
SET (CMAKE_BUILD_TYPE Debug)

enable_testing()

set(IDF_PATH /home/olas/esp/esp-idf)


ADD_DEFINITIONS(-D TEST )

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -static-libstdc++")
set(CMAKE_C_FLAGS "-g -std=c99")


INCLUDE_DIRECTORIES(
${IDF_PATH}/components/nvs_flash/include/
${IDF_PATH}/components/esp32/include/
freeRTOS
freeRTOS/freertos
freeRTOS/portable/GCC/Posix
)

SET (LIBRARY_TYPE STATIC)

ADD_LIBRARY(dummy
components/nvs_flash/nvs_flash.c
components/esp32/dummy_esp32.c
)

ADD_LIBRARY(freertos
freeRTOS/croutine.c
freeRTOS/list.c
freeRTOS/queue.c
freeRTOS/tasks.c
freeRTOS/portable/MemMang/heap_3.c
freeRTOS/portable/GCC/Posix/port.c
freeRTOS/freertos/croutine.h
freeRTOS/freertos/FreeRTOS.h
freeRTOS/freertos/list.h
freeRTOS/freertos/mpu_wrappers.h
freeRTOS/freertos/portable.h
freeRTOS/freertos/projdefs.h
freeRTOS/freertos/queue.h
freeRTOS/freertos/semphr.h
freeRTOS/freertos/StackMacros.h
freeRTOS/freertos/task.h
)


ADD_EXECUTABLE(linux_esp32
../qemu_esp32/main/main.c
main_task.c
)

TARGET_LINK_LIBRARIES(linux_esp32 freertos dummy pthread)


#add_test(test1 testFunctions)


ADD_DEFINITIONS(-D NOTHING_To_DEFINE_FOR_NOW )

