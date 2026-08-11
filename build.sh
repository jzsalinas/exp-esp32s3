#!/bin/bash
set -e

# Asegurar que arduino-cli esté instalado y configurado
if ! command -v arduino-cli &> /dev/null
then
    echo "[!] arduino-cli no se encontró en el PATH. Instalándolo localmente en ./bin..."
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
    export PATH=$PATH:./bin
fi

echo "[1/4] Configurando core de ESP32 y actualizando índices..."
arduino-cli config init --overwrite
arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32

echo "[2/4] Instalando dependencias (Micro-RTSP)..."
# Micro-RTSP no está en el índice oficial con ese nombre exacto, clonamos directo de GitHub
mkdir -p ~/Arduino/libraries
if [ ! -d "$HOME/Arduino/libraries/Micro-RTSP" ]; then
    git clone https://github.com/geeksville/Micro-RTSP.git ~/Arduino/libraries/Micro-RTSP
else
    echo "Micro-RTSP ya está instalado en ~/Arduino/libraries/"
fi

echo "[3/4] Compilando el proyecto..."
# FQBN: esp32:esp32:esp32s3
# Flag PSRAM=opi habilita el uso de la memoria RAM externa necesaria para la cámara
arduino-cli compile \
  --fqbn esp32:esp32:esp32s3 \
  --board-options "PSRAM=opi,CDCOnBoot=default" \
  ./exp-esp32s3.ino

echo "[4/4] ¡Compilación finalizada con éxito! Flasheando la placa..."
# NOTA: Según las reglas (AGENTS.md), ya estás en el grupo dialout, así que omitimos sudo
arduino-cli upload -p /dev/ttyACM1 --fqbn esp32:esp32:esp32s3 ./exp-esp32s3.ino

echo "=============================================="
echo "¡Flasheo completado!"
echo "Presiona el botón RST una vez para arrancar tu servidor RTSP."
