#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include "Errors.h"

class StorageAccess final {
public:
	static void createFileWithContent(String fileName, String content);
	static void dir(); 
	static void changeDefaults();
	static String loadStringFromFile(String fileName);
	String load(String path) ;
	static void deleteFile(const String& path);
private:
	static void writeFile(fs::FS &fs, const char *path, String content);
	static void writeFile(fs::FS &fs, String path, String content);
	static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
	static String readFile(fs::FS &fs, const String);
};

#endif /* CONFIG_H_ */
#pragma once
