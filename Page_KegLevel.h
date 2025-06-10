// FILE: Page_KegLevel.h
#ifndef PAGE_KEG_LEVEL_H
#define PAGE_KEG_LEVEL_H

#include "pin_config.h" 
void setup_ui_keg_level_page(lv_obj_t *page);
void loop_keg_level_page();
void update_keg_level_data(float ax, float ay, float az);

#endif