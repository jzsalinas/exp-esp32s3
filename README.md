# Exp-RibbonCam (ESP32-S3)

Este repositorio es un entorno de experimentación avanzado para transmisión de video en tiempo real utilizando la plataforma ESP32-S3-CAM equipada con un sensor OV5640 y memorias externas (PSRAM OPI). 

El objetivo principal es resolver los típicos cuellos de botella del IoT (latencia, congestión Wi-Fi, caídas de voltaje de RF) para lograr transmisiones de video lo suficientemente estables y de baja latencia como para alimentar modelos de **Computer Vision** (OpenCV, Reconocimiento Facial).

## Características del Proyecto
- **Arquitectura Modular:** El código fuente ha sido dividido en librerías (`AppCamera`, `AppWifi`, `ServerRTSP`, `ServerHTTP`) para mantener el orden de cara a nuevos experimentos.
- **Multiserver (Condicional):** Mediante macros en `Config.h`, puedes compilar el proyecto como un Servidor RTSP tradicional o un Servidor Web HTTP M-JPEG.
- **Optimizaciones Core:**
  - `CAMERA_GRAB_LATEST`: Forzamos el driver para evadir el framebuffer atrasado, garantizando 0 lag físico.
  - Mitigación de EMI (Interferencia Electromagnética): Reloj del bus rebajado a 10MHz para combatir los parpadeos púrpuras.
  - Autocorrección de Wi-Fi.

## Estructura del Código

```text
exp-esp32s3/
├── exp-esp32s3.ino       # Entry point principal (minimalista)
├── Config.h              # MACROS: Selecciona RTSP o HTTP aquí
├── AppCamera.cpp/h       # Inicialización optimizada del OV5640
├── AppWifi.cpp/h         # Conexión resitente de red
├── ServerHTTP.cpp/h      # (Exp 02) Servidor M-JPEG de latencia cero
├── ServerRTSP.cpp/h      # (Exp 01) Servidor RTSP parcheado contra crash
├── build.sh              # Script automatizado con arduino-cli
├── secrets.h             # (¡A crear!) Credenciales Wi-Fi locales
└── docs/experimentos/    # Bitácoras de cada experimento técnico
```

## Guía Rápida

1. Renombra `secrets_example.h` a `secrets.h` y coloca las contraseñas de tu red Wi-Fi local.
2. Ve a `Config.h` y elige qué servidor activar poniendo `1`:
   ```cpp
   #define ENABLE_HTTP_SERVER 1
   #define ENABLE_RTSP_SERVER 0
   ```
3. Conecta el ESP32-S3 a la PC vía USB y entra en **Download Mode** (Mantén BOOT, presiona RST, suelta BOOT).
4. Ejecuta el script de construcción local:
   ```bash
   ./build.sh
   ```
5. Presiona el botón RST de la placa y monitorea el puerto Serial para obtener la IP.
6. Abre `http://<IP>/stream` en tu navegador o VLC.

## Documentación de Experimentos
Si quieres saber por qué tomamos ciertas decisiones de diseño (como quitar el delay en HTTP o modificar las librerías de terceros), visita:
- [Experimento 01: Optimización RTSP](docs/experimentos/01-rtsp-optimizations.md)
- [Experimento 02: Modularización HTTP](docs/experimentos/02-http-streaming.md)
