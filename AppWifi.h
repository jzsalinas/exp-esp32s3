#ifndef APP_WIFI_H
#define APP_WIFI_H

#include <Arduino.h>
#include <WiFi.h>

// Inicializa la conexión Wi-Fi y desactiva el ahorro de energía
void initAppWifi();

// Verifica el estado del Wi-Fi y reconecta automáticamente si es necesario
void checkWifiReconnect();

#endif // APP_WIFI_H
