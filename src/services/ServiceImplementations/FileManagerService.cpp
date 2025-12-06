#include "FileManagerService.h"

FileManagerService::FileManagerService()
{
    settings = new SettingsModel();
    isOperative = false;
}



String FileManagerService::getFileData(String &path)
{
    boolean recoveryJson = false;
    // Initialize SPIFFS
    if (!LittleFS.begin())
    {
        serialService->logWarning("An Error has occurred while mounting SPIFFS (read file in rom)", getNameService(), "loadSettings(String path)");
        serialService->logWarning("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)", getNameService(), "loadSettings(String path)");
        isOperative = false;
        return "";
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
            ((SerialService *)getServiceByCollector("SerialService"))->logWarning("Errore nell'aprire il file", getNameService(), "getFileData(String &path)");
            return "";
        }

        // Leggi il contenuto del file e immagazzinalo in una stringa
        while (file.available())
        {
            fileContent += (char)file.read();
        }
        file.close();
        return fileContent;
    }else{
        ((SerialService *)getServiceByCollector("SerialService"))->logWarning("File non esiste", getNameService(), "getFileData(String &path)");
    }

    return "";
}

void FileManagerService::onInitServiceCollector()
{
    serialService = ((SerialService *)getServiceByCollector("SerialService"));
    isOperative = true;
}

boolean FileManagerService::writeFile(String &path, String &content, bool create)
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

boolean FileManagerService::fileExists(String &path)
{
    return LittleFS.exists(path);
}