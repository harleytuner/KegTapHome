// FILE: Page_Image.cpp
#include "Page_Image.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "bsp_lv_port.h"

void check_and_load_image();

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

static lv_obj_t *beer_logo;
static lv_img_dsc_t img_dsc;
static File logoFile;
static uint8_t *img_buf = NULL;
static volatile bool file_is_ready_on_sd = false;

class PageImage_ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { Serial.println("ImagePage: Device connected"); }
    void onDisconnect(BLEServer* pServer) { Serial.println("ImagePage: Device disconnected"); BLEDevice::startAdvertising(); }
};

class PageImage_CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String value = pCharacteristic->getValue();
        if (value.length() > 0) {
            if (value == "START") {
                if(logoFile) { logoFile.close(); }
                if (sd_card_initialized) {
                    logoFile = SD.open("/logo.bin", FILE_WRITE);
                }
                return;
            }
            if (value == "END") {
                if(logoFile) { logoFile.close(); }
                file_is_ready_on_sd = true;
                return;
            }
            if(logoFile) { logoFile.write((const uint8_t*)value.c_str(), value.length()); }
        }
    }
};

void setup_ui_image_page(lv_obj_t *page) {
    lv_obj_set_style_bg_color(page, lv_color_hex(0x000000), LV_PART_MAIN);
    beer_logo = lv_img_create(page);
    lv_obj_align(beer_logo, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(page);
    lv_label_set_text(label, "Send a logo via BLE");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -20);

    BLEDevice::init("KegTap");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new PageImage_ServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR | BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new PageImage_CharacteristicCallbacks());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    file_is_ready_on_sd = true;
}

void check_and_load_image() {
    if (!sd_card_initialized) {
        return;
    }

    if(lvgl_lock(-1)) {
        if(SD.exists("/logo.bin")) {
            logoFile = SD.open("/logo.bin");
            if (logoFile && logoFile.size() > 0) {
                if (img_buf) { free(img_buf); img_buf = NULL; }
                img_buf = (uint8_t *)ps_malloc(logoFile.size());
                if (img_buf == NULL) {
                    Serial.println("Failed to allocate memory for image!");
                } else {
                    logoFile.read(img_buf, logoFile.size());
                    img_dsc.header.w = 160;
                    img_dsc.header.h = 160;
                    img_dsc.data_size = logoFile.size();
                    img_dsc.header.cf = LV_IMG_CF_RGB565; // Changed LV_IMG_CF_TRUE_COLOR to LV_IMG_CF_RGB565
                    img_dsc.data = img_buf;
                    lv_img_set_src(beer_logo, &img_dsc);
                }
                logoFile.close();
            }
        }
        lvgl_unlock();
    }
}

void loop_image_page() {
    if(file_is_ready_on_sd) {
        file_is_ready_on_sd = false;
        check_and_load_image();
    }
}