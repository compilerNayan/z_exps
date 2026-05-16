#ifdef MEEEE
#include "internal/05-server/03-variant/02-espidf/01-EspidfTcpServer.h"
#include <thread>
#include <chrono>
#include <cstdio>

#include "wifi/IWiFiManager.h"

/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] Failed to connect to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi cesdonnection failed\n");
        return;
    }

    EspidfTcpServer server;
    if (!server.Start()) {
        printf("[ERROR] Server failed to start\n");
        return;
    }

    printf("[INFO] Server started, listening on port 8080\n");

    while (server.IsRunning()) {
        auto msgOpt = server.ReceiveMessage(std::nullopt);
        if (msgOpt.has_value()) {
            IoTMessage msg = msgOpt.value();
            printf("[INFO] Received message: GUID=%s, Payload=%s\n",
                   msg.guid.c_str(), msg.payload.c_str());

            // Build HTTP response body
            StdString body = "Hello there " + msg.payload;

            // Compose minimal HTTP/1.1 response
            char httpBuf[512];
            snprintf(httpBuf, sizeof(httpBuf),
                     "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/plain\r\n"
                     "Content-Length: %zu\r\n"
                     "\r\n"
                     "%s",
                     body.size(), body.c_str());

            IoTMessage response;
            response.guid = msg.guid;
            response.payload = StdString(httpBuf);

            if (!server.SendMessage(response, std::nullopt)) {
                printf("[ERROR] Failedeh to send response for GUID=%s\n", msg.guid.c_str());
            } else {
                printf("[INFO] Sent response for GUID=%s\n", msg.guid.c_str());
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    server.Stop();
    printf("[INFO] Server stopped\n");
}
#endif