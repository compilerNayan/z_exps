#include <cstdio>
#include "esp_log.h"

#include "wifi/IHotspotManager.h"
#include "FileManagerTests.h"
#include "MkdirTests.h"

/* @Autowired */
IHotspotManagerPtr hotspotManager = Implementation<IHotspotManager>::type::GetInstance();

extern "C" void app_main(void) {
    printf("Starting FileManagerTests\n");
    RunFileManagerTests();
    //TestMkdirAndList();
    TestFlatVsNestedAndLength();
 /*   ESP_LOGI("HotspotMain", "Starting hotspot demo");

    CStdString ssid = "Tiger";
    Optional<CStdString> password;   // empty → open hotspot

    Bool started = hotspotManager->Start(ssid, password, 4);
    if (started) {
        printf("Hotspot started successfully\n");
    } else {
        printf("Failed to start hotspot\n");
    }

    // Loop forever, check every 1 second
    while (true) {
        if (hotspotManager->IsActive()) {
            auto ip = hotspotManager->GetIPAddress();
            if (ip.has_value()) {
                printf("Hotspot IP: %s\n", ip.value().c_str());
            }

            Int clients = hotspotManager->GetConnectedClients();
            if (clients > 0) {
                printf("Clients connected: %d\n", clients);
                auto clientList = hotspotManager->ListClients();
                for (auto &mac : clientList) {
                    printf("Client MAC: %s\n", mac.c_str());
                }
            } else {
                printf("No clients connected\n");
            }
        } else {
            printf("Hotspot not active\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1 second
    } */
}
