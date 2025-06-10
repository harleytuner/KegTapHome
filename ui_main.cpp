// FILE: ui_main.cpp
#include "ui_main.h"
#include "Page_Image.h"
#include "Page_KegLevel.h"

static lv_obj_t *tabview;

void ui_main_init() {
  tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40); 
  
  lv_obj_t *ui_image_page = lv_tabview_add_tab(tabview, "Image");
  lv_obj_t *ui_keg_level_page = lv_tabview_add_tab(tabview, "Keg Level");

  setup_ui_image_page(ui_image_page);
  setup_ui_keg_level_page(ui_keg_level_page);
}

void ui_main_loop() {
  uint16_t active_tab_id = lv_tabview_get_tab_act(tabview);

  if (active_tab_id == 0) { 
    loop_image_page();
  } else if (active_tab_id == 1) { 
    loop_keg_level_page();
  }
}