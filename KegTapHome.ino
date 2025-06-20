// FILE: KegTapHandle.ino
#include "pin_config.h"
#include "bsp_cst816.h" 
#include "ui_main.h"    
#include "app_qmi8658.h"

Arduino_DataBus *bus = new Arduino_ESP32SPI(LCD_DC, LCD_CS, LCD_SCK, LCD_MOSI, -1);
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT);
lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_buf;
lv_disp_drv_t disp_drv;
lv_indev_drv_t indev_drv;

bool sd_card_initialized = false;

void setup() {
  Serial.begin(115200);
  
  // CORRECTED INITIALIZATION ORDER BASED ON WAVESHARE DEMO
  
  // 1. Manually initialize the shared SPI bus first.
  // The demo uses pins 39, 40, 38 for SCK, MISO, MOSI. CS is handled by SD.begin.
  SPI.begin(39, 40, 38, -1); 

  // 2. Now initialize the SD card on the bus we just configured.
  Serial.println("Mounting SD Card...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Card Mount Failed! Check card formatting (FAT32).");
    sd_card_initialized = false;
  } else {
    Serial.println("SD Card mounted successfully.");
    sd_card_initialized = true;
  }

  // 3. Initialize the Display. It will use the same, already-running SPI bus.
  Serial.println("Initializing Display...");
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  gfx->begin();
  gfx->fillScreen(BLACK);

  // 4. Initialize the rest of the hardware and software
  Wire.begin(48, 47);
  bsp_touch_init(&Wire, 0, LCD_WIDTH, LCD_HEIGHT);
  app_qmi8658_init();

  lv_init();
  disp_buf = (lv_color_t *)malloc(sizeof(lv_color_t) * LCD_WIDTH * 40);
  lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, LCD_WIDTH * 40);
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  
  ui_main_init();
  app_qmi8658_run();

  Serial.println("Setup Complete.");
}

void loop() {
  lv_timer_handler(); 
  ui_main_loop();
  delay(5);
}

void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1);
  lv_disp_flush_ready(disp_drv);
}

void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  uint16_t x, y;
  bsp_touch_read();
  if (bsp_touch_get_coordinates(&x, &y)) {
    data->point.x = x; data->point.y = y; data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}