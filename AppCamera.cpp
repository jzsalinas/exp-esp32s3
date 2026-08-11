#include "AppCamera.h"
#include "Config.h"

bool initAppCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  // OPTIMIZACIONES CLAVE
  config.xclk_freq_hz = 10000000; // 10 MHz para evitar EMI y caídas de voltaje
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 20; // Menos ancho de banda
    config.fb_count = 2;      
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_LATEST; // Cero latencia
    Serial.println("PSRAM encontrada. Usando buffers ampliados.");
  } else {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 20;
    config.fb_count = 1;
    Serial.println("PSRAM NO encontrada. Operando en modo memoria restringida.");
  }

  // Iniciar la cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Fallo en la inicialización de la cámara, error 0x%x", err);
    return false;
  }
  
  sensor_t * s = esp_camera_sensor_get();
  if (s->id.PID == OV5640_PID) {
    // Si necesitas invertirla de nuevo, descomenta estas líneas:
    // s->set_vflip(s, 1);
    // s->set_hmirror(s, 1);
  }
  
  Serial.println("Cámara inicializada correctamente.");
  return true;
}
