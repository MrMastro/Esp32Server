#include "SettingService.h"

SettingService::SettingService()
{
    isOperative = false;
}

SettingsModel SettingService::getSettings()
{
    if (!isOperative)
    {
        throwError(ERROR_CODE::SERVICE_ERROR, "Service not avaible, error to load settings", "getSettingByName");
        return SettingsModel();
    }
    return settings;
}

void SettingService::loadSettings(String path)
{
    // Initialize SPIFFS
    if (!LittleFS.begin())
    {
        logInfoln("An Error has occurred while mounting SPIFFS (read file in rom)");
        logInfoln("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)");
        isOperative = false;
        return;
    }

    File file;
    String fileContent = "";

    // Verifica se il file esiste
    if (LittleFS.exists(path))
    {

        // Apri il file in modalità lettura
        file = LittleFS.open(path, "r");
        if (!file)
        {
            Serial.println("Errore nell'aprire il file");
            isOperative = false;
            return;
        }

        // Leggi il contenuto del file e immagazzinalo in una stringa
        while (file.available())
        {
            fileContent += (char)file.read();
        }
    }
    else
    {
        logInfoln("settings.json non presente lo creo");

        // Crea e scrivi il contenuto predefinito nel file
        file = LittleFS.open(path, "w");
        if (!file)
        {
            logInfoln("Errore nella creazione del file");
            isOperative = false;
            return;
        }

        // Scrivi il contenuto predefinito
        logInfoln("File creato con il contenuto predefinito.");
        file.print(defaultContentSettings);
        fileContent = defaultContentSettings;
    }

    // Chiudi il file
    file.close();

    // Stampa il contenuto del file
    logInfoln("Caricate le seguenti impostazioni:\n"+fileContent);

    // carico il contenuto di filecontent dentro loadSettings
    SettingsModel deviceSettings;
    deviceSettings.fromJson(fileContent);
    settings = deviceSettings;
    isOperative = true;
}
