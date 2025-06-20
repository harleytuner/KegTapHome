// FILE: app_qmi8658.cpp
#include "app_qmi8658.h"
#include "FastIMU.h"
#include "Page_KegLevel.h"
#include "bsp_lv_port.h" 

#define IMU_ADDRESS 0x6B
QMI8658 IMU;

calData calib = { 0 };
AccelData accelData;

void app_qmi8658_task(void *arg) {
  while (1) {
    IMU.update();
    IMU.getAccel(&accelData);
    
    update_keg_level_data(accelData.accelX, accelData.accelY, accelData.accelZ);
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void app_qmi8658_init(void) {
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
  }
}

void app_qmi8658_run(void) {
  xTaskCreate(app_qmi8658_task, "qmi8658_task", 4096, NULL, 1, NULL);
}