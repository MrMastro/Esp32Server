#include "SettingService.h"

SettingService::SettingService()
{
    settings = new SettingsModel();
    isOperative = false;
}

SettingsModel SettingService::getSettings()
{
    if (!isOperative)
    {
        throwError(ERROR_CODE::SERVICE_ERROR, "Service not avaible, error to load settings", "getSettingByName");
        return SettingsModel();
    }
    return *settings;
}

void SettingService::loadSettings(String path)
{
    SerialService* serialService = ( (SerialService*) getServiceByCollector("SerialService"));
    Serial.println("loadSettings");
    boolean recoveryJson = false;
    // Initialize SPIFFS
    if (!LittleFS.begin())
    {
        serialService->logWarning("An Error has occurred while mounting SPIFFS (read file in rom)", getNameService(),"loadSettings(String path)");
        serialService->logWarning("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)", getNameService(),"loadSettings(String path)");
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
            ( (SerialService*) getServiceByCollector("SerialService"))->logWarning("Errore nell'aprire il file", getNameService(),"loadSettings(String path)");
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
        serialService->logWarning("File non trovato, path: " + path , getNameService(),"loadSettings(String path)");
        serialService->logWarning("Carico impostazioni di default" , getNameService(),"loadSettings(String path)");
        String defaultContent =  SettingsModel::getDefault().toJson();
        writeFile(file, path, defaultContent);
        fileContent = defaultContent;
    }

    // Chiudi il file
    file.close();

    // Stampa il contenuto del file
    serialService->logInfoln("Caricate le seguenti impostazioni:\n"+fileContent, getNameService());

    // carico il contenuto di filecontent dentro loadSettings
    boolean res = settings->fromJson(fileContent);
    if(!res){
        serialService->logWarning("Errore file json, ripristino json", getNameService(),"loadSettings(String path)");
        SettingsModel defaultSettingsModel = SettingsModel::getDefault();
        String defaultContent =  defaultSettingsModel.toJson();
        writeFile(file, path, defaultContent);
        fileContent = defaultContent;
        settings->fromJson(defaultContent);
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
