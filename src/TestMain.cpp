#define JDJEJED
#ifdef JDJEJED

#include "HttpClientTester.h"
#include "wifi/IWiFiManager.h"
#include "clock/IClockSynchronizer.h"

void RunAllTests() {
    HttpClientTester tester;
    tester.RunAllTests();
}


/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] hkhdsdsdssdsdsdsdsdsds to sdj to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi cesdonnection failed\n");
        return;
    }

    if (!Implementation<IClockSynchronizer>::type::GetInstance()->SyncIfNeeded(
            "pool.ntp.org", 15000, 500, "UTC0")) {
        printf("[ERROR] SNTP time sync failed\n");
        return;
    }

    RunAllTests();
}

#endif