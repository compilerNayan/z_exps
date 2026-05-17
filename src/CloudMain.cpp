#include "internal/05-server/02-interface/02-IMqttClient.h"
#include <thread>
#include <chrono>
#include <cstdio>

#include "wifi/IWiFiManager.h"

/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

/*--@Autowired--*/
IMMqttClientPtr mqttClient = Implementation<IMMqttClient>::type::GetInstance();


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

    // Connect to MQTT broker
    if (!mqttClient->Connect()) {
        printf("[ERROR] Failed to connect to MQTT broker\n");
        return;
    }
    if (!mqttClient->WaitForConnection(10000) || !mqttClient->IsConnected()) {
        printf("[ERROR] MQTT connection failed\n");
        return;
    }

    // Main loop
    while (mqttClient->IsConnected()) {
        // Poll for messages on default topic (or any subscribed topic)
        auto msgOpt = mqttClient->ReceiveMessage("nknk32/sub");
        if (msgOpt.has_value()) {
            MqttMessage msg = msgOpt.value();
            printf("[INFO] Received message: GUID=%s, Payload=%s\n",
                   msg.guid.c_str(), msg.payload.c_str());

            // Build response payload
            StdString body = "Hello there " + msg.payload;

            MqttMessage response;
            response.guid = msg.guid;
            response.payload = body;

            // Publish response back to same topic
            if (!mqttClient->SendMessage("nknk32/pub", response)) {
                printf("[ERROR] Failed to send response for GUID=%s\n", msg.guid.c_str());
            } else {
                printf("[INFO] Sent response for GUID=%s\n", msg.guid.c_str());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    mqttClient->Disconnect();
    printf("[INFO] MQTT client disconnected\n");
}
