#include "AppWifi.h"
#include "Config.h"

void initAppWifi() {
  Serial.print("\n[WIFI] Conectando a ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Importante: desactivar sleep previene latencia en LwIP (el stack TCP/IP)
  WiFi.setSleep(false);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n[WIFI] ¡Conectado!");
  Serial.print("[WIFI] Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void checkWifiReconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[WIFI] ¡Señal perdida! Reconectando...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    
    Serial.println("\n[WIFI] ¡Reconectado!");
    Serial.print("[WIFI] Dirección IP: ");
    Serial.println(WiFi.localIP());
  }
}
