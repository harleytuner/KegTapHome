// FILE: Page_KegLevel.cpp (Corrected)
#include "Page_KegLevel.h"
#include <cmath> 

static lv_obj_t *angle_label; 
static lv_obj_t *keg_percent_label;
static volatile float current_angle = 0.0f;

void setup_ui_keg_level_page(lv_obj_t *page) {
    lv_obj_set_style_bg_color(page, lv_color_hex(0x101020), LV_PART_MAIN);
    
    keg_percent_label = lv_label_create(page);
    lv_obj_set_style_text_font(keg_percent_label, &lv_font_montserrat_14, 0); 
    lv_obj_set_style_text_color(keg_percent_label, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(keg_percent_label, "100%");
    lv_obj_align(keg_percent_label, LV_ALIGN_CENTER, 0, -20);
    
    angle_label = lv_label_create(page);
    lv_obj_set_style_text_font(angle_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(angle_label, lv_color_hex(0xCCCCCC), 0);
    lv_label_set_text(angle_label, "Angle: 0.0°");
    lv_obj_align(angle_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void update_keg_level_data(float ax, float ay, float az) {
    float angle = atan2(ay, az) * 180.0 / PI;
    current_angle = angle;
}

void loop_keg_level_page() {
    if (angle_label) {
        lv_label_set_text_fmt(angle_label, "Angle: %.1f°", current_angle);
    }
}