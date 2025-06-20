// FILE: pin_config.h
#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include "lv_conf.h" 
#include <Arduino.h>
#include <lvgl.h> // Keep this include
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Pin definitions for LCD
#define LCD_SCK   39
#define LCD_MOSI  38
#define LCD_MISO  40
#define LCD_DC    42
#define LCD_RST   0
#define LCD_CS    45
#define LCD_BL    1
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

// Pin definitions for SD Card (sharing the same SPI bus)
#define SD_CS_PIN    41

// Forward declarations for global objects
extern Arduino_GFX *gfx;
extern lv_draw_buf_t draw_buf; // Changed lv_disp_draw_buf_t to lv_draw_buf_t
extern lv_color_t *disp_buf;
extern lv_disp_drv_t disp_drv;
extern lv_indev_drv_t indev_drv;
extern bool sd_card_initialized;

// Forward declarations for core functions
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

#endif // PIN_CONFIG_H