#include <string>
#include <map>
#include <optional>
#include "nvs_flash.h"
#include "esp_log.h"
#include <cstring>

#ifndef NVS_UTIL_H
#define NVS_UTIL_H

class NVSUtil {
    
    private:
    static constexpr const char* TAG = "NVS_Util";

    public:
        
    static std::optional<std::string> tryGetValueFromNVS(const std::string& key, const std::string& namespaceName = "storage") {
        
        nvs_handle_t nvs;
        esp_err_t err = nvs_open(namespaceName.c_str(), NVS_READONLY, &nvs);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
            return std::nullopt;
        }

        size_t required_size = 0;
        err = nvs_get_str(nvs, key.c_str(), nullptr, &required_size);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "value %s not found", key.c_str());
            nvs_close(nvs);
            return std::nullopt;
        } else if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting key %s: %s", key.c_str(), esp_err_to_name(err));
            nvs_close(nvs);
            return std::nullopt;
        }

        std::string result(required_size, '\0');
        err = nvs_get_str(nvs, key.c_str(), result.data(), &required_size);
        nvs_close(nvs);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting parameter %s: %s", key.c_str(), esp_err_to_name(err));
            return std::nullopt;
        }

        // Remove potential null terminator from NVS (as std::string already manages it)
        result.resize(strlen(result.c_str()));

        return result;
    }

    static bool hasValueInNVS(const std::string& key, const std::string& namespaceName = "storage") {
        nvs_handle_t nvs;
        esp_err_t err = nvs_open(namespaceName.c_str(), NVS_READONLY, &nvs);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
            return false;
        }

        size_t required_size = 0;
        err = nvs_get_str(nvs, key.c_str(), nullptr, &required_size);
        nvs_close(nvs);

        if (err == ESP_ERR_NVS_NOT_FOUND) {
            return false; // Key does not exist
        } else if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error checking key %s: %s", key.c_str(), esp_err_to_name(err));
            return false; // Error occurred
        }

        return true; // Key exists
    }

    static void addValueToNVS(const std::string& key, const std::string& value, const std::string& namespaceName = "storage") {
        nvs_handle_t nvs;
        esp_err_t err = nvs_open(namespaceName.c_str(), NVS_READWRITE, &nvs);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
            return;
        }

        err = nvs_set_str(nvs, key.c_str(), value.c_str());
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error setting key %s: %s", key.c_str(), esp_err_to_name(err));
        } else {
            err = nvs_commit(nvs);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error committing changes to NVS: %s", esp_err_to_name(err));
            }
        }

        nvs_close(nvs);
    }

    static void removeValueFromNVS(const std::string& key, const std::string& namespaceName = "storage") {
        nvs_handle_t nvs;
        esp_err_t err = nvs_open(namespaceName.c_str(), NVS_READWRITE, &nvs);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error opening NVS: %s", esp_err_to_name(err));
            return;
        }

        err = nvs_erase_key(nvs, key.c_str());
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error removing key %s: %s", key.c_str(), esp_err_to_name(err));
        } else {
            err = nvs_commit(nvs);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error committing changes to NVS: %s", esp_err_to_name(err));
            }
        }

        nvs_close(nvs);
    }
};

#endif


