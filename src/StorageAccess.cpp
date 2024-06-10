#include "StorageAccess.h"
#include "Protocol.h"

#define FORMAT_SPIFFS_IF_FAILED true
//
String StorageAccess::loadStringFromFile(String path) {
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS Mount Failed");
        return String("");
    }
    listDir(SPIFFS, "/", 0);
    Serial.println(path);
    if (!SPIFFS.exists(path)) {
        Serial.println(" did not exist in the file system !!!!");
        String errorMessage = path + " file not found in in the file system";
        Errors::raiseUserInputAlarm(errorMessage);
        return ""; //empty String checkable with isEmpty
    }
    String content = readFile(SPIFFS, path);
    SPIFFS.end();
    Serial.println("Found file. Content read from file:");
    Serial.println(content);
    return content;
}
//
// saves by overwriting. Needs full ile name including /
void StorageAccess::createFileWithContent(String fileName, String content) {
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS Mount Failed");
    }
    Serial.println("\nin saveStringInFile for content");
    Serial.println(content);
    //String path = String("/") + String(fileName);
    String path = String(fileName);
    writeFile(SPIFFS, path, content);
    Serial.println();
    SPIFFS.end();
}
//
void StorageAccess::changeDefaults() {  // todo delete function

}
//
String StorageAccess::readFile(fs::FS &fs, String path) {
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        Serial.println("- failed to open file for reading");
        return String("");
    }
    String content = file.readString();
    file.close();
    return content;
}
//
void StorageAccess::writeFile(fs::FS &fs, String path, String content) {
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(content)) {
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
//
void StorageAccess::writeFile(fs::FS &fs, const char *path, String content) {
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(content)) {
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
//
void StorageAccess::deleteFile(const String &path) {
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    Serial.printf("Deleting file: %s\r\n", path);
    if (SPIFFS.remove(path)) {
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
    StorageAccess::dir();
    SPIFFS.end();
}
//
void StorageAccess::dir() {
    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    listDir(SPIFFS, "/", 0);
    SPIFFS.end();
}
//
void StorageAccess::listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("                 SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}