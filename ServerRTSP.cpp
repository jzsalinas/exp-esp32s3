#include "ServerRTSP.h"

#if ENABLE_RTSP_SERVER

#include "esp_camera.h"
#include <WiFi.h>
#include "OV2640Streamer.h"
#include "CRtspSession.h"

static WiFiServer* rtspServer = nullptr;
static CStreamer* streamer = nullptr;
static WiFiClient activeClients[MAX_RTSP_CLIENTS];
static int currentClientIdx = 0;
static uint32_t lastimage = 0;

// Wrapper de la cámara OV5640 adaptado para la librería
class HardwareCamera : public OV2640 {
public:
  HardwareCamera() {}
  void run() override {
    // Ya no hacemos nada aquí porque no controlamos el fb globalmente, 
    // OV2640Streamer espera hacer fb = m_cam->getfb(), etc.
    // En este wrapper la librería esp32-camera asume que el frame es pedido localmente.
  }
};

static OV2640* camWrapper = nullptr;

void initServerRTSP() {
  camWrapper = new OV2640(); // Asume que esp_camera_init() ya fue llamado
  streamer = new OV2640Streamer(camWrapper);
  
  rtspServer = new WiFiServer(8554);
  rtspServer->begin();
  
  lastimage = millis();
  Serial.println("[RTSP] Servidor RTSP iniciado en el puerto 8554");
}

void loopServerRTSP() {
  if (!streamer || !rtspServer) return;

  streamer->handleRequests(0);

  // FPS a 5 para evitar embotellamiento TCP
  uint32_t msecPerFrame = 200; 
  uint32_t now = millis();
  
  if (streamer->anySessions()) {
    if (now > lastimage + msecPerFrame || now < lastimage) {
      streamer->streamImage(now);
      lastimage = now;
    }
  }

  WiFiClient newClient = rtspServer->accept();
  if (newClient) {
    Serial.print("[RTSP] Cliente conectado desde: ");
    Serial.println(newClient.remoteIP());
    activeClients[currentClientIdx] = newClient;
    streamer->addSession(&activeClients[currentClientIdx]);
    currentClientIdx = (currentClientIdx + 1) % MAX_RTSP_CLIENTS;
  }
}

#endif // ENABLE_RTSP_SERVER
