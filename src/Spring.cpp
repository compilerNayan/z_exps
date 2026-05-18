#define JSKEKEKE
#ifdef JSKEKEKE

#include "IHttpRequestManager.h"
#include "wifi/IWiFiManager.h"
#include "Thread.h"


/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

/*--@Autowired--*/
IHttpRequestManagerPtr requestManager = Implementation<IHttpRequestManager>::type::GetInstance();

extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] Failed to connect to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi cesdonnection failed\n");
        return;
    }

    requestManager->StartServer();

    printf("[INFO] Server started\n");

    while (true) {
        requestManager->RetrieveRequest();
        Thread::Sleep(1000);
    }

    requestManager->StopServer();

    printf("[INFO] Server stopped\n");
}

#endif