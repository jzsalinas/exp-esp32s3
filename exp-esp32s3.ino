#include <Arduino.h>
#include "Config.h"
#include "AppCamera.h"
#include "AppWifi.h"

#if ENABLE_RTSP_SERVER
#include "ServerRTSP.h"
#endif

#if ENABLE_HTTP_SERVER
#include "ServerHTTP.h"
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n====================================");
  Serial.println("[INIT] Iniciando ESP32-S3 Cam Server");
  Serial.println("====================================");

  // Inicializar Cámara
  if (!initAppCamera()) {
    Serial.println("[FATAL] Cámara no encontrada o falla de hardware. Deteniendo.");
    while (true) { delay(100); }
  }

  // Inicializar Wi-Fi
  initAppWifi();

  // Inicializar Servidores (Dependiendo de la configuración en Config.h)
#if ENABLE_RTSP_SERVER
  initServerRTSP();
#endif

#if ENABLE_HTTP_SERVER
  initServerHTTP();
#endif

  Serial.println("\n[INIT] Todo listo, entrando al loop principal.");
}

void loop() {
  // Lógica de resiliencia Wi-Fi compartida
  checkWifiReconnect();

  // Lógica de los servidores
#if ENABLE_RTSP_SERVER
  loopServerRTSP();
#endif

#if ENABLE_HTTP_SERVER
  // El HTTP Server corre en su propia FreeRTOS task internamente, 
  // así que no necesitamos pollear nada en el loop para él.
  delay(1000);
#endif
}
