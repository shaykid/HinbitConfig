#include "HinbitConfig.h"

// UUIDs for BLE
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Initialize BLE
void HinbitConfig::initializeBLE() {
    BLEDevice::init("HinbitESP_" + String((uint32_t)ESP.getEfuseMac(), HEX));  // Use unique device name
    pServer = BLEDevice::createServer();
    
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->setValue("Initialized");  // Initial characteristic value
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Minimum preferred connection interval
    pAdvertising->setMinPreferred(0x12);  // Maximum preferred connection interval
    pAdvertising->start();
}

// Begin config setup
void HinbitConfig::begin() {
    initFileSystem();
    startConfig();
    initializeBLE();
}

// Activate BLE device based on config
void HinbitConfig::setBLEDeviceActive() {
    initializeBLE();
}

// Function to send characteristic value in chunks if it exceeds the BLE limit (20 bytes)
void notifyInChunks(BLECharacteristic* characteristic, String data) {
    const int maxChunkSize = 20;  // BLE notification limit is 20 bytes
    int dataLength = data.length();
    
    for (int i = 0; i < dataLength; i += maxChunkSize) {
        String chunk = data.substring(i, i + maxChunkSize);
        characteristic->setValue(chunk.c_str());
        characteristic->notify();  // Notify the client with the current chunk

        delay(500);  // Small delay to ensure proper notification handling
    }
}

// Handle BLE terminal requests
void HinbitConfig::handleBLERequests() {
    if (pCharacteristic == nullptr) {
        Serial.println("Error: pCharacteristic is null.");
        return;
    }

    // Get the characteristic value as an Arduino String
    String request = pCharacteristic->getValue().c_str();

    if (request.length() > 0) {
        Serial.print("Just Received BLE Command: ");
        Serial.println(request);  // Print received BLE command to Serial Monitor

        String response = "";  // Store response here to notify

        if (request.startsWith("SET")) {
            int commaIndex1 = request.indexOf(',');
            int commaIndex2 = request.indexOf(',', commaIndex1 + 1);
            
            String key = request.substring(commaIndex1 + 1, commaIndex2);
            String value = request.substring(commaIndex2 + 1);
            
            setConfigValue(key, value);
            response = "OK";  // Set response to OK after SET command
        }
        
        else if (request.startsWith("GET")) {
            int commaIndex = request.indexOf(',');
            String key = request.substring(commaIndex + 1);
            String value = getConfigValue(key);
            response = value;  // Return the value for the GET command
        }

        else if (request == "LIST") {
            String keys = listConfigKeys();
            response = keys;  // Return the list of keys
        }

        // If response is larger than 20 bytes, send it in chunks
        notifyInChunks(pCharacteristic, response);

        // Debugging: Print the response to Serial
        Serial.print("Just Sent BLE Response: ");
        Serial.println(response);
    }

    delay(100);  // Small delay
}

// Set config key-value pair
void HinbitConfig::setConfigValue(String key, String value) {
    writeConfig(key, value);
}

// Get value from config based on key
String HinbitConfig::getConfigValue(String key) {
    return readConfig(key);
}

// List all config keys
String HinbitConfig::listConfigKeys() {
    // Check if config.json exists in LittleFS
    if (!LittleFS.exists("/config.json")) {
        Serial.println("config.json does not exist.");
        return "";
    }

    // Open config.json and load into a DynamicJsonDocument
    File file = LittleFS.open("/config.json", "r");
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, file);
    file.close();

    String result = "";  // String to accumulate all key-value pairs

    // Iterate over all key-value pairs in the document
    Serial.println("All Config key-value pairs:");
    for (JsonPair kv : doc.as<JsonObject>()) {
        String key = kv.key().c_str();
        String value = kv.value().as<String>();
        
        // Print each key-value pair to the serial monitor
        Serial.println(key + ": " + value);

        // Append each key-value pair to the result string
        result += key + ": " + value + "\n";
    }

    return result;  // Return the string containing all key-value pairs
}

// Initialize file system and handle formatting
void HinbitConfig::initFileSystem() {
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS, attempting to format...");
        formatLittleFS();
        if (!LittleFS.begin()) {
            Serial.println("Failed to mount LittleFS after formatting.");
            return;
        }
    }
}

// Format the LittleFS file system
void HinbitConfig::formatLittleFS() {
    Serial.println("Formatting LittleFS...");
    if (LittleFS.format()) {
        Serial.println("LittleFS formatted successfully.");
    } else {
        Serial.println("Failed to format LittleFS.");
    }
}

// Start config, create default if missing
bool HinbitConfig::startConfig() {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("config.json not found. Creating a default one.");
        createDefaultConfig();
        printConfig();
        return false;
    } else {
        Serial.println("config.json exists in FS.");
        printConfig();
        return true;
    }
}

// Create default configuration file
void HinbitConfig::createDefaultConfig() {
    Serial.println("Creating default config.json...");

    DynamicJsonDocument doc(4096);
    doc["WIFI"]["SSID"] = "MySSID";
    doc["WIFI"]["PASS"] = "MyPassword";
    doc["LAST_DATE"] = "01-10-1971";
    doc["CITY"] = "רעננה";
    doc["FILES_PATH"] = "https://hinbit.com/HalachaTimes/";
    doc["TIME_ZONE"] = "Asia/Jerusalem";
    doc["UPDATE_FILES"] = "true";
    doc["UPDATE_TIME"] = "true";
    doc["SUNRISE_SECONDS_ADJUST"] = 30;
    doc["DEVICE_NAME"] = "HINBIT_DEVICE";
    doc["CODE_VERSION"] = "2.4";

    File file = LittleFS.open("/config.json", "w");
    serializeJson(doc, file);
    file.close();
}

// Read configuration value based on key
String HinbitConfig::readConfig(String key) {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("config.json does not exist.");
        return "";
    }

    File file = LittleFS.open("/config.json", "r");
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, file);
    file.close();

    String nestedKey = key;
    nestedKey.replace("-", ".");
    JsonVariant value = doc[nestedKey];

    if (!value.isNull()) {
        return value.as<String>();
    } else {
        return "Key not found";
    }
}

// Write configuration value to key
void HinbitConfig::writeConfig(String key, String value) {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("config.json does not exist.");
        return;
    }

    File file = LittleFS.open("/config.json", "r");
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, file);
    file.close();

    String nestedKey = key;
    nestedKey.replace("-", ".");
    doc[nestedKey] = value;

    file = LittleFS.open("/config.json", "w");
    serializeJson(doc, file);
    file.close();
}

// Print configuration or specific key
void HinbitConfig::printConfig(String key) {
    if (!LittleFS.exists("/config.json")) {
        Serial.println("config.json does not exist.");
        return;
    }

    File file = LittleFS.open("/config.json", "r");
    DynamicJsonDocument doc(4096);
    deserializeJson(doc, file);
    file.close();

    if (key == "") {
        serializeJson(doc, Serial);
        Serial.println();
    } else {
        String nestedKey = key;
        nestedKey.replace("-", ".");
        JsonVariant value = doc[nestedKey];

        if (!value.isNull()) {
            Serial.print(key);
            Serial.print(": ");
            Serial.println(value.as<String>());
        } else {
            Serial.println("The Key not found in config.json");
        }
    }
}

// List all files in LittleFS
void HinbitConfig::listLittleFSFiles() {
    Serial.println("Listing LittleFS files:");
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while (file) {
        Serial.print("FILE: ");
        Serial.print(file.name());
        Serial.print(" SIZE: ");
        Serial.println(file.size());
        file = root.openNextFile();
    }
}
