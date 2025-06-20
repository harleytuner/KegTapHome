// FILE: bsp_lv_port.cpp
#include "bsp_lv_port.h"
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t g_lvgl_mutex = NULL;

bool lvgl_lock(int timeout_ms) {
    if (g_lvgl_mutex == NULL) {
        g_lvgl_mutex = xSemaphoreCreateMutex();
    }

    const TickType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTake(g_lvgl_mutex, timeout_ticks) == pdTRUE;
}

void lvgl_unlock(void) {
    xSemaphoreGive(g_lvgl_mutex);
}
