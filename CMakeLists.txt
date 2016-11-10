cmake_minimum_required(VERSION 2.4)
PROJECT(linux_esp32)
#SET (CMAKE_BUILD_TYPE Release)
SET (CMAKE_BUILD_TYPE Debug)

enable_testing()

set(IDF_PATH /home/olas/esp/esp-idf)
set(LDF_PATH /home/olas/esp/linux_esp32)
set(THIS_PATH /home/olas/esp/qemu_esp32)

ADD_DEFINITIONS(-D TEST )

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -static-libstdc++")
set(CMAKE_C_FLAGS "-g -std=c99")


INCLUDE_DIRECTORIES(
${IDF_PATH}/components/nvs_flash/include/
${IDF_PATH}/components/esp32/include/
${IDF_PATH}/components/esp32/include
${IDF_PATH}/components/driver/include/
${IDF_PATH}/components/log/include/
${THIS_PATH}/build/include/
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
${LDF_PATH}/freeRTOS/croutine.c
${LDF_PATH}/freeRTOS/list.c
${LDF_PATH}/freeRTOS/queue.c
${LDF_PATH}/freeRTOS/tasks.c
${LDF_PATH}/freeRTOS/portable/MemMang/heap_3.c
${LDF_PATH}/freeRTOS/portable/GCC/Posix/port.c
${LDF_PATH}/freeRTOS/freertos/croutine.h
${LDF_PATH}/freeRTOS/freertos/FreeRTOS.h
${LDF_PATH}/freeRTOS/freertos/list.h
${LDF_PATH}/freeRTOS/freertos/mpu_wrappers.h
${LDF_PATH}/freeRTOS/freertos/portable.h
${LDF_PATH}/freeRTOS/freertos/projdefs.h
${LDF_PATH}/freeRTOS/freertos/queue.h
${LDF_PATH}/freeRTOS/freertos/semphr.h
${LDF_PATH}/freeRTOS/freertos/StackMacros.h
${LDF_PATH}/freeRTOS/freertos/task.h
)


ADD_EXECUTABLE(linux_esp32
../qemu_esp32/main/main.c
main_task.c
)

TARGET_LINK_LIBRARIES(linux_esp32 freertos dummy pthread)


#add_test(test1 testFunctions)


ADD_DEFINITIONS(-D NOTHING_To_DEFINE_FOR_NOW )

