# Experimento 01: Optimización del Servidor RTSP en ESP32-S3-CAM

## Resumen del Entorno
- **Hardware:** ESP32-S3-CAM N16R8 (16MB Flash, 8MB PSRAM OPI).
- **Sensor:** OV5640.
- **Protocolo Base:** RTSP sobre Wi-Fi (Librería `Micro-RTSP`).

## Problemas Encontrados y Soluciones

Durante el desarrollo inicial del streaming de video mediante RTSP, nos topamos con una serie de fallas críticas originadas tanto en el software (librería de terceros y drivers) como en el hardware (capacidades físicas del SoC y de la red).

### 1. Crasheo de la Placa (`LoadProhibited` Panic)
- **Síntoma:** Al intentar reproducir el video con VLC, la placa colapsaba y se reiniciaba inmediatamente imprimiendo un Guru Meditation Error en la terminal.
- **Causa Raíz:** Un bug en `Micro-RTSP`. Cuando VLC se conectaba con un comando `OPTIONS` y cerraba rápidamente el socket TCP, la librería reducía un contador de referencias UDP a números negativos, lo que causaba que la placa intentara cerrar un socket `NULL` cuando VLC se volvía a conectar.
- **Solución:** Parche local en `CStreamer.cpp`. Se envolvió la reducción del contador de referencias con un condicional de seguridad:
  ```cpp
  void CStreamer::ReleaseUdpTransport(void) {
      if ( m_udpRefCount > 0 ) {
          --m_udpRefCount;
          if ( m_udpRefCount == 0 ) {
              // ... cierra sockets
          }
      }
  }
  ```

### 2. Inundación del Buffer UDP (`error sending udp packet`)
- **Síntoma:** Al intentar transmitir un fotograma completo (que se divide en ~40 paquetes UDP), el monitor serial se llenaba de errores indicando que no se podía enviar el paquete, resultando en pantallas negras y pérdida de imagen en el cliente.
- **Causa Raíz:** La librería enviaba los 40 paquetes UDP en un bucle cerrado en menos de 1 milisegundo. La pequeña antena Wi-Fi del ESP32 no tiene el rendimiento para despachar paquetes tan rápido, colapsando la memoria LwIP del chip.
- **Solución:** Parche local en `CStreamer.cpp`. Se añadió un `delay(3)` entre el envío de cada paquete, pero **exclusivamente** si hay algún cliente usando UDP. De este modo, la radio Wi-Fi tiene tiempo para transmitir físicamente cada pedazo.

### 3. Latencia Masiva en Redes Lentas (Embotellamiento TCP)
- **Síntoma:** Utilizando TCP, el video llegaba sin cortes pero con un retraso (delay) masivo de 5 a 10 segundos.
- **Causa Raíz:** Al configurar la cámara a 25 FPS (40ms), se generaba aproximadamente 1 MB/s de video. Como la conexión Wi-Fi local estaba congestionada/débil, no podía enviar 1 MB/s, lo que causaba que el protocolo TCP (que tiene backpressure natural) bloqueara el sistema. Ese "tranque" provocaba que la transmisión se quedara minutos en el pasado.
- **Solución (Balance de Ancho de Banda):** Redujimos los FPS a 5 (`msecPerFrame = 200`) y ajustamos la calidad del JPEG a `config.jpeg_quality = 20`. Esto redujo el ancho de banda a unos ~80 KB/s, que cruzan instantáneamente cualquier red inestable sin formar colas de espera.

### 4. Latencia Restante por el Buffer del Hardware ("Grab Mode")
- **Síntoma:** A pesar de haber arreglado la red y usar TCP, el stream seguía teniendo un pequeño retraso persistente (~1-2 segundos extra).
- **Causa Raíz:** El chip OV5640 captura fotos sin parar a 25fps en el fondo, llenando el framebuffer de la PSRAM. Al pedir imágenes a solo 5 FPS (nuestro ritmo del software), el driver de la cámara nos estaba devolviendo la foto **más vieja** estancada en el buffer (comportamiento por defecto: `CAMERA_GRAB_WHEN_EMPTY`).
- **Solución:** En la configuración del hardware, se forzó a que la cámara entregue siempre el fotograma MÁS RECIENTE, descartando el historial obsoleto.
  ```cpp
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST; 
  ```

### 5. Parpadeo Intermitente Lavado (Interferencia Electromagnética / EMI)
- **Síntoma:** De pronto la imagen mostraba destellos, se lavaba de color blanco/violeta o mostraba bandas purpuras del lado izquierdo.
- **Causa Raíz:** Caída de voltaje del regulador analógico (`AVDD`) o inyección directa de microondas en el cable plano (ribbon) de la cámara. Al usar la antena Wi-Fi, la placa demanda picos de 500mA, causando bajones que enloquecen al sensor OV5640.
- **Solución de Software:** Redujimos el reloj de la cámara a la mitad (`config.xclk_freq_hz = 10000000;`). Al ir más lento, es mucho más resistente a la caída de voltaje y genera menos ruido de conmutación en la placa. (Es posible requerir acciones de hardware como alejar el cable plano de la antena).

## Recomendaciones para el Cliente Final
Para lograr la latencia mínima absoluta (~200ms) durante pruebas locales, se recomienda abandonar VLC (que tiene políticas agresivas de buffer) y usar `ffplay` por TCP con los siguientes argumentos para apagar todos sus algoritmos de retraso:

```bash
ffplay -rtsp_transport tcp -fflags nobuffer -flags low_delay -probesize 32 -analyzeduration 0 -sync ext rtsp://<ESP32_IP>:8554/mjpeg/1
```
