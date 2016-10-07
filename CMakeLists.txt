cmake_minimum_required(VERSION 2.4)
PROJECT(linux_esp32)
#SET (CMAKE_BUILD_TYPE Release)
SET (CMAKE_BUILD_TYPE Debug)

enable_testing()


IF(WIN32)
ADD_DEFINITIONS(-D WIN32 )
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11 -static-libstdc++")
ELSE(WIN32)
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
ENDIF(WIN32)


INCLUDE_DIRECTORIES(
freeRTOS
freeRTOS/freertos
freeRTOS/portable/GCC/Posix
)

SET (LIBRARY_TYPE STATIC)


ADD_LIBRARY(freertos
freeRTOS/croutine.c
freeRTOS/list.c
freeRTOS/queue.c
freeRTOS/tasks.c
freeRTOS/portable/MemMang/heap_3.c
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


ADD_EXECUTABLE(main
../qemu_esp32/main/main.c
)

TARGET_LINK_LIBRARIES(main freertos)


#add_test(test1 testFunctions)


ADD_DEFINITIONS(-D NOTHING_To_DEFINE_FOR_NOW )

