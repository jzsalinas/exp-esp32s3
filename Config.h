#ifndef CONFIG_H
#define CONFIG_H

//=============================================================================
// CONFIGURACIÓN DE EXPERIMENTOS
//=============================================================================

// Selecciona qué servidor compilar y ejecutar (solo uno a la vez para optimizar recursos)
#define ENABLE_HTTP_SERVER 1
#define ENABLE_RTSP_SERVER 0

//=============================================================================
// CONFIGURACIÓN DE CÁMARA (OV5640)
//=============================================================================
#define CAMERA_MODEL_ESP32S3_EYE
#include "camera_pins.h"

//=============================================================================
// CONFIGURACIÓN DE RED
//=============================================================================
#include "secrets.h"

// MAX_CLIENTS para el RTSP
#define MAX_RTSP_CLIENTS 4

#endif // CONFIG_H
