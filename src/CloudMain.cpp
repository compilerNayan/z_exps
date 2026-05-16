#include "internal/05-server/03-variant/02-espidf/02-EspidfAwsIotCoreServer.h"
#include <thread>
#include <chrono>
#include <cstdio>

#include "wifi/IWiFiManager.h"

/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

extern "C" void app_main(void) {
    // Connect to WiFi
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] Failed to connect to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi connection failed\n");
        return;
    }

    // Start AWS IoT Core server
    EspidfAwsIotCoreServer server;
    if (!server.Start()) {
        printf("[ERROR] AWS IoT Core server failed to start\n");
        return;
    }

    printf("[INFO] AWS IoT Core server started\n");

    // Main loop
    while (server.IsRunning()) {
        // Poll for messages on default topic (or any subscribed topic)
        auto msgOpt = server.ReceiveMessage(Optional<StdString>{"nknk32/sub"});
        if (msgOpt.has_value()) {
            IoTMessage msg = msgOpt.value();
            printf("[INFO] Received message: GUID=%s, Payload=%s\n",
                   msg.guid.c_str(), msg.payload.c_str());

            // Build response payload
            StdString body = "Hello there " + msg.payload;

            IoTMessage response;
            response.guid = msg.guid;
            response.payload = body;

            // Publish response back to same topic
            if (!server.SendMessage(response, Optional<StdString>{"nknk32/pub"})) {
                printf("[ERROR] Failed to send response for GUID=%s\n", msg.guid.c_str());
            } else {
                printf("[INFO] Sent response for GUID=%s\n", msg.guid.c_str());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    server.Stop();
    printf("[INFO] AWS IoT Core server stopped\n");
}
