# Experimento 02: Modularización y Servidor HTTP M-JPEG

## Resumen del Entorno
- **Hardware:** ESP32-S3-CAM N16R8 (16MB Flash, 8MB PSRAM OPI).
- **Sensor:** OV5640.
- **Protocolos Activos:** RTSP (Micro-RTSP) y HTTP (esp_http_server) intercambiables por macros.

## Metas del Experimento
Tras alcanzar la estabilidad con RTSP, el objetivo era implementar un servidor HTTP M-JPEG nativo. HTTP suele ser más versátil para la integración web (se puede embeber directo en una etiqueta `<img>` de HTML) y para scripts de Computer Vision (OpenCV lo consume fácilmente vía URL).

## Reestructuración Arquitectónica

Para evitar un archivo `.ino` monolítico inmanejable, dividimos el proyecto original (`rtsp-ribboncam.ino`) en varios componentes de clase profesional:

1. **`Config.h`**: Se añadió un panel de control global que permite alternar los módulos usando macros (`ENABLE_HTTP_SERVER` vs `ENABLE_RTSP_SERVER`). Esto previene que el ESP32 malgaste recursos en servidores que no se están usando.
2. **`AppCamera.h/cpp`**: Aisla toda la lógica (pines, relojes y buffers) de inicialización física de la cámara.
3. **`AppWifi.h/cpp`**: Maneja la conexión y el algoritmo de autoreconexión si se pierde la señal.
4. **`ServerRTSP.h/cpp`**: Encapsula las librerías `Micro-RTSP`.
5. **`ServerHTTP.h/cpp`**: La nueva implementación basada en `esp_http_server`.

## Problemas Encontrados y Soluciones

### 1. El Conflicto de Variables Globales (Multiple Definition)
- **Síntoma:** Al compilar los nuevos `.cpp`, `arduino-cli` lanzaba errores de `multiple definition of 'WIFI_SSID'` y `WIFI_PASSWORD`.
- **Causa Raíz:** El archivo `secrets.h` declaraba `const char*` en el espacio de nombres global, lo que causaba que al incluirlo en diferentes archivos (`AppCamera.cpp`, `AppWifi.cpp`), cada archivo tuviera su propia copia conflictiva al enlazarse (link stage).
- **Solución:** Se transformó el archivo a usar directivas de preprocesador `#define WIFI_SSID "..."`.

### 2. Lentitud Progresiva del HTTP (El Síntoma de la "Máquina del Tiempo")
- **Síntoma:** Al conectarse por HTTP (`192.168.1.8/stream`), el video empezaba en tiempo real, pero se iba enlenteciendo gradualmente con el paso de los segundos hasta acumular varios segundos de retraso.
- **Causa Raíz:** Arrastramos el parche de `vTaskDelay(200)` (5 FPS) del experimento de RTSP. En HTTP, la función `httpd_resp_send_chunk` ya se bloquea automáticamente por la presión nativa de la red TCP (backpressure). Al sumarle nuestro `delay` manual de 200ms a esa espera nativa, desfasábamos la lectura de cuadros de la cámara con respecto a la red, acumulando latencia.
- **Solución:** Remover el delay limitador y permitir que el protocolo TCP (Control de Congestión) dictamine la velocidad naturalmente. El delay se redujo a `vTaskDelay(1)` solo para acariciar el Watchdog del RTOS.

### 3. Orientación Invertida de la Cámara
- **Síntoma:** La imagen HTTP aparecía de cabeza en comparación al RTSP original.
- **Causa Raíz:** El código estándar para ESP32-CAM suele invertir el OV5640 (`set_vflip` y `set_hmirror`) porque la lente está montada al revés físicamente en la placa. Sin embargo, en el diseño *ribboncam* (lente en cable flexible), la orientación natural era correcta.
- **Solución:** Se retiraron ambas llamadas de inversión de hardware en `AppCamera.cpp`.

## Resultado Final
Un flujo de video M-JPEG extremadamente limpio en `http://<ESP_IP>/stream`, de latencia nula, que escala automáticamente a la capacidad máxima del Wi-Fi disponible y no crashea bajo estrés. Ideal para enviar a módulos de Computer Vision.
