#define JDJEJED
#ifdef JDJEJED

#include "HttpClientTester.h"
#include "wifi/IWiFiManager.h"

void RunAllTests() {
    HttpClientTester tester;
    tester.RunAllTests();
}


/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] hkhdsdsdssdsdsdsdsds to sdj to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi cesdonnection failed\n");
        return;
    }

    RunAllTests();
}

#endif