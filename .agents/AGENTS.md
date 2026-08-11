## Visión del Proyecto y Filosofía

- **Nombre:** wifi-analyzer
- **Licencia:** MIT (Open Source, totalmente documentado).
- **Idioma de Comunicación con el Usuario:** Español (tono colega, profesional, preciso y colaborativo).
- **Documentación:** No usar íconos como accesorios.
- **Estrategia de codificación:** Escribir siempre basado en la solicitud del usuario, no hacer mock sin que el usuario lo sepa, si no se puede hacer algo y se necesita de un mock de forma temporal, informar siempre al usuario del mock creado, para evitar confusión entre una prueba real y una simulada.

## Git Protocol (Auto-Summary)
- **NEVER** stage or commit changes autonomously. 
- **WAIT** for the trigger: "puedes commitear todos los cambios?".
**Action on Trigger:**
1. **Cleanup:** Delete any `.log`, `.tmp`, o temporary text files created during the session.
2. **Auto-Summarize:** Analyze all changes and generate a concise, professional commit message describing what was done.
3. **Execute:** Run `git add .` followed by `git commit -m "[Tu resumen generado]"`.
4. **Verification:** Run `git --no-pager log -1` to show the final hash and confirm the commit actually exists.

## Notas Técnicas y Perfil de Hardware (ESP32)
Para futuros proyectos o módulos, asume las siguientes configuraciones y especificaciones comprobadas para el hardware de adquisición:

- **Modelo exacto:** ESP32-S3-CAM N16R8 (16 MB Flash + 8 MB PSRAM).
- **Sensor óptico:** Cuenta con ranura FPC para cámaras tipo OV2640 / OV5640.
- **Conexión USB:** Utilizar exclusivamente el puerto USB-UART (chip CH343P), NO utilizar el puerto USB-OTG para conexiones seriales/flasheo estándar.
- **Modo Flasheo (Download Mode):** Para poder subir un nuevo firmware, es obligatorio forzar el modo descarga manualmente en la placa. Secuencia: Mantener presionado el botón `BOOT`, luego presionar y soltar `RST`, y finalmente soltar `BOOT`. Tras finalizar el flasheo, se debe presionar `RST` una vez para iniciar la placa.
- **Permisos en Linux:** El usuario está agregado al grupo `dialout`, por lo tanto se debe conectar/manipular los puertos `/dev/ttyACM0` o `/dev/ttyUSB0` directamente, sin usar `sudo`.
- **Configuración del CLI/IDE (arduino-cli):** El FQBN correcto a utilizar es `esp32:esp32:esp32s3`. Es mandatorio compilar habilitando la PSRAM en modo **OPI PSRAM** para manejar los buffers grandes (historial de tramas o fotogramas JPEG).
