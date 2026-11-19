# Sceglie il dispositivo di destinazione dell'upload OTA (ambiente Esp32Server).
#
# In ota_local.ini (o platformio.ini):
#   [ota]
#   target = esp32_1              ; nome del dispositivo da aggiornare
#
#   [ota_esp32_1]                  ; una sezione [ota_<nome>] per ogni dispositivo
#   ip = 172.17.252.192
#   password = ...              ; devicePassword del suo settings.json
#
# Il target si può anche scegliere al volo senza toccare i file, con la variabile d'ambiente OTA_TARGET:
#   PowerShell:  $env:OTA_TARGET = "esp32_2"; pio run -e Esp32Server -t upload
#
# PlatformIO non supporta variabili annidate tipo ${ota_${ota.target}.ip}: per questo serve lo script.

import os

Import("env")  # noqa: F821 (fornito da PlatformIO)

config = env.GetProjectConfig()  # noqa: F821

target = os.environ.get("OTA_TARGET", "").strip() or config.get("ota", "target", "").strip()
section = "ota_" + target


def fail(message):
    print("\n[OTA] ERRORE: " + message + "\n")
    env.Exit(1)  # noqa: F821


# I controlli servono solo quando si fa l'upload: la sola compilazione dell'ambiente OTA deve funzionare sempre
if "upload" in COMMAND_LINE_TARGETS:  # noqa: F821
    if not target:
        fail("manca 'target' nella sezione [ota] di ota_local.ini")
    if not config.has_section(section):
        known = [s[4:] for s in config.sections() if s.startswith("ota_")]
        fail("sezione [%s] non trovata. Dispositivi definiti: %s" % (section, ", ".join(known) or "nessuno"))

    ip = config.get(section, "ip", "").strip()
    password = config.get(section, "password", "").strip()
    if not ip or ip == "0.0.0.0":
        fail("ip non impostato per il dispositivo '%s' (sezione [%s])" % (target, section))
    if not password:
        fail("password non impostata per il dispositivo '%s' (sezione [%s])" % (target, section))

    env.Replace(UPLOAD_PORT=ip, UPLOAD_FLAGS=["--port=3232", "--auth=" + password])  # noqa: F821
    print("[OTA] destinazione: %s (%s)" % (target, ip))
