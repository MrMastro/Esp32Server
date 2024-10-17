#include "SettingService.h"
#include <models/InitialSettingSaveModel/InitialSettingSaveModel.h>

SettingService::SettingService()
{
    settings = new SettingsModel();
    isOperative = false;
}

SettingsModel SettingService::getSettings()
{
    if (!isOperative)
    {
        // todo throwError(ERROR_CODE::SERVICE_ERROR, "Service not avaible, error to load settings", "getSettingByName");
        return SettingsModel();
    }
    return *settings;
}

String SettingService::getJsonSettings()
{
    if (!isOperative)
    {
        // todo throwError(ERROR_CODE::SERVICE_ERROR, "Service not avaible, error to load settings", "getSettingByName");
        return String();
    }
    return settings->toJson();
}

boolean SettingService::saveSettings(String path, SettingsModel s)
{
    String content = s.toJson();
    if (!writeFile(path, content))
    {
        serialService->logWarning("An Error has occurred while save settings", getNameService(), "saveSettings");
        return false;
    }
    return true;
}

boolean SettingService::saveInitialSettings(String path, InitialSettingSaveModel s)
{
    settings->initialEffect = s.initialEffect;
    settings->initialDeltaT = s.initialDeltaT;
    settings->initialR = s.initialR;
    settings->initialG = s.initialG;
    settings->initialB = s.initialB;
    String content = settings->toJson();
    if (!writeFile(path, content))
    {
        serialService->logWarning("An Error has occurred while save settings", getNameService(), "saveSettings");
        return false;
    }
    return true;
}

boolean SettingService::changeSetting(String key, String value)
{
    SettingsModel newSettings;
    newSettings.fromJson(settings->toJson());
    if (key == "communicationMode")
    {
        newSettings.communicationMode = value;
        saveSettings(SETTINGS_FILE_LOCATION_PATH, newSettings);
        return true;
    }
    else
    {
        return false;
    }
}

void SettingService::loadSettings(String path)
{
    boolean recoveryJson = false;
    // Initialize SPIFFS
    if (!LittleFS.begin())
    {
        serialService->logWarning("An Error has occurred while mounting SPIFFS (read file in rom)", getNameService(), "loadSettings(String path)");
        serialService->logWarning("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)", getNameService(), "loadSettings(String path)");
        isOperative = false;
        return;
    }

    String fileContent = "";

    // Verifica se il file esiste
    if (LittleFS.exists(path))
    {
        File file;
        // Apri il file in modalità lettura
        file = LittleFS.open(path, "r");
        if (!file)
        {
            ((SerialService *)getServiceByCollector("SerialService"))->logWarning("Errore nell'aprire il file", getNameService(), "loadSettings(String path)");
            Serial.println("Errore nell'aprire il file");
            isOperative = false;
            return;
        }

        // Leggi il contenuto del file e immagazzinalo in una stringa
        while (file.available())
        {
            fileContent += (char)file.read();
        }
        file.close();
    }
    else
    {
        serialService->logWarning("File non trovato, path: " + path, getNameService(), "loadSettings(String path)");
        serialService->logWarning("Carico impostazioni di default", getNameService(), "loadSettings(String path)");
        String pathFolder = SETTINGS_FOLDER_LOCATION_PATH;
        if (!LittleFS.mkdir(pathFolder)) {
            Serial.println("Errore nella creazione della cartella");
        } else {
            Serial.println("Cartella creata con successo");
        }
        String defaultContent = SettingsModel::getDefault().toJson();
        writeFile(path, defaultContent, true);
        fileContent = defaultContent;
    }

    Serial.println("Contenuto del file:");
    Serial.println(fileContent);
    if (fileContent.isEmpty())
    {
        serialService->logWarning("Contenuto del file vuoto", getNameService(), "loadSettings(String path)");
        serialService->logWarning("Carico impostazioni di default", getNameService(), "loadSettings(String path)");
        fileContent = SettingsModel::getDefault().toJson();
    }
    // carico il contenuto di filecontent dentro loadSettings
    boolean res = settings->fromJson(fileContent);
    Serial.println(res);
    if (!res)
    {
        serialService->logWarning("Errore file json, ripristino json", getNameService(), "loadSettings(String path)");
        SettingsModel defaultSettingsModel = SettingsModel::getDefault();
        String defaultContent = defaultSettingsModel.toJson();
        writeFile(path, defaultContent);
        fileContent = defaultContent;
        settings->fromJson(defaultContent);
    }
    isOperative = true;
}

void SettingService::onInitServiceCollector()
{
    serialService = ((SerialService *)getServiceByCollector("SerialService"));
}

boolean SettingService::writeFile(String &path, String &content, bool create)
{
    // Crea e scrivi il contenuto predefinito nel file
    File file = LittleFS.open(path, "w", create);
    if (!file)
    {
        serialService->logWarning("Errore nella creazione del file", getNameService(), "loadSettings(String path)");
        isOperative = false;
        return false;
    }
    file.print(content);
    file.close();
    return true;
}