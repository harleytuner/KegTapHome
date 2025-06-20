// FILE: KegTapHome.ino

#define LV_CONF_INCLUDE_SIMPLE 1 // IMPORTANT: This must be at the very top, before any LVGL includes

#include "pin_config.h" // Contains updated LVGL type declarations
#include "bsp_cst816.h"
#include "app_qmi8658.h"
#include "ui_main.h"

#include <Arduino_ST7789.h> // ADD THIS LINE: Include the specific display driver header

// Global objects declared in pin_config.h
// These extern declarations in pin_config.h will now correctly refer to these definitions
// after lv_conf.h is properly processed.
Arduino_GFX *gfx = nullptr; // Initialize to nullptr
lv_draw_buf_t draw_buf;
lv_color_t *disp_buf;
lv_disp_drv_t disp_drv;
lv_indev_drv_t indev_drv;
bool sd_card_initialized = false;

// LVGL display flush callback function
void my_disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  // gfx->draw16bitBeRGBBitmap is for Big Endian. Confirm if your display expects Little Endian.
  // If it expects Little Endian, use gfx->draw16bitRGBBitmap.
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);
  lv_disp_flush_ready(drv);
}

// LVGL touchpad read callback function
void my_touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  uint16_t touchpad_x, touchpad_y;
  bool pressed = bsp_touch_get_coordinates(&touchpad_x, &touchpad_y);

  if (pressed) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting KegTapHome...");

  // Initialize display using a specific driver like Arduino_ST7789
  // You may need to change Arduino_ST7789 if your display uses a different controller.
  // The constructor parameters are typically: Arduino_DataBus *bus, int8_t rst_pin, uint8_t rotation, bool IPS_panel
  // Or: Arduino_ST7789(Arduino_DataBus *bus, int8_t rst, int16_t w, int16_t h, uint8_t rotation, bool is_invert);
  // Given your pin_config, assuming ST7789 with a generic SPI bus.
  // Adjust the constructor call based on your exact display module's driver and its constructor.
  // A common constructor might be: Arduino_ST7789(int8_t dc, int8_t cs, int8_t rst); for specific pinouts
  // Given LCD_SCK, LCD_MOSI, LCD_MISO, LCD_DC, LCD_RST, LCD_CS, we're likely dealing with a hardware SPI setup.
  // Re-instantiate gfx with the correct display driver (e.g., ST7789 for 240x320 resolution)
  // The exact constructor depends on your specific Arduino_GFX_Library version and display.
  // Here's a common approach for ST7789 with hardware SPI:
  gfx = new Arduino_ST7789(LCD_DC, LCD_CS, LCD_RST); // Using common pins for ST7789 direct connection
  // You might need to specify the SPI bus directly if not default:
  // gfx = new Arduino_ST7789(&SPI, LCD_DC, LCD_CS, LCD_RST); // Example if using a specific SPI object

  gfx->begin(); // Call begin() without width/height. Dimensions are typically handled internally by the specific driver.
  gfx->setRotation(1); // Adjust rotation as needed for your display
  gfx->fillScreen(BLACK);

  // Initialize backlight
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH); // Turn on backlight

  // Initialize SD Card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card Mount Failed");
    sd_card_initialized = false;
  } else {
    Serial.println("SD Card Initialized.");
    sd_card_initialized = true;
  }

  // Initialize LVGL
  lv_init();

  // Allocate display buffer
  // Ensure LV_COLOR_DEPTH is defined as 16 in lv_conf.h
  disp_buf = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * LCD_WIDTH * 40); // Allocate for 40 lines
  if (disp_buf == NULL) {
    Serial.println("LVGL disp_buf allocation failed!");
    while (1);
  }

  // Corrected lv_draw_buf_init call for LVGL v8.x signature
  // lv_result_t lv_draw_buf_init(lv_draw_buf_t * draw_buf, uint32_t w, uint32_t h, lv_color_format_t cf, uint32_t stride, void* buf1, uint32_t buf1_size)
  lv_draw_buf_init(&draw_buf, LCD_WIDTH, 40, LV_COLOR_FORMAT_RGB565, LCD_WIDTH * sizeof(lv_color_t), disp_buf, LCD_WIDTH * 40 * sizeof(lv_color_t));

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf; // Assign the draw buffer
  lv_disp_drv_register(&disp_drv);

  // Initialize touchpad
  Wire.begin(); // Initialize I2C for touchpad
  if (!bsp_touch_init(&Wire, 1, LCD_WIDTH, LCD_HEIGHT)) { // Check rotation (1, 2, 3, or 0)
    Serial.println("Touchpad initialization failed!");
  } else {
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
  }

  // Initialize IMU
  app_qmi8658_init();
  app_qmi8658_run();

  // Initialize UI
  ui_main_init();

  Serial.println("Setup Complete!");
}

void loop() {
  lv_timer_handler(); // Let LVGL do its work
  ui_main_loop();     // Call your UI loop for page-specific logic
  bsp_touch_read();   // Continuously read touch data
  delay(5);           // Small delay
}