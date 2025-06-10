// FILE: bsp_lv_port.h
#ifndef BSP_LV_PORT_H
#define BSP_LV_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

bool lvgl_lock(int timeout_ms);
void lvgl_unlock(void);

#ifdef __cplusplus
}
#endif