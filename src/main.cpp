#include <cstdio>

#include "OsalDefines.h"
#include "esp_log.h"

static const char *TAG = "hello";

extern "C" void app_main(void) {
    printf("hello world\n");
    ESP_LOGI(TAG, "hello world");
}
