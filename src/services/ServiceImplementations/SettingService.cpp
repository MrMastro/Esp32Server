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
    boolean recoveryJson = false;
    // Initialize SPIFFS
    if (!LittleFS.begin())
    {
        logWarning("An Error has occurred while mounting SPIFFS (read file in rom)","loadSettings(String path)");
        logWarning("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)","loadSettings(String path)");
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
        String defaultContent =  SettingsModel::getDefault().toJson();
        writeFile(file, path, defaultContent);
        fileContent = defaultContent;
    }

    // Chiudi il file
    file.close();

    // Stampa il contenuto del file
    logInfoln("Caricate le seguenti impostazioni:\n"+fileContent);

    // carico il contenuto di filecontent dentro loadSettings
    SettingsModel deviceSettings;
    boolean res = deviceSettings.fromJson(fileContent);
    if(res){
       settings = deviceSettings;
    }else{
        logWarning("Errore file json, ripristino json","loadSettings(String path)");
        SettingsModel defaultSettingsModel = SettingsModel::getDefault();
        String defaultContent =  defaultSettingsModel.toJson();
        writeFile(file, path, defaultContent);
        fileContent = defaultContent;
        settings = defaultSettingsModel;
    }
    isOperative = true;
}

boolean SettingService::writeFile(fs::File &file, String &path, String &content)
{
    // Crea e scrivi il contenuto predefinito nel file
    file = LittleFS.open(path, "w");
    if (!file)
    {
        logWarning("Errore nella creazione del file","loadSettings(String path)");
        isOperative = false;
        return false;
    }

    // Scrivi il contenuto predefinito
    logInfoln("File creato con il contenuto predefinito.");
    file.print(content);
    return true;
}
