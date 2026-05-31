#ifdef IKSKSKHFHFH

#include "service/IDeviceService.h"
#include "fleet/IFleetProvisioningService.h"
#include "wifi/IWiFiManager.h"
#include "clock/IClockSynchronizer.h"
#include "Thread.h"

/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

/*--@Autowired--*/
IFleetProvisioningServicePtr fleetProvisioningService = Implementation<IFleetProvisioningService>::type::GetInstance();

extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] fj djsh sdj to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi dsdsdssdsds failed\n");
        return;
    } else {
        printf("[INFO] WiFi dskal successfully\n");
    }

    Thread::Sleep(3000);

    if (!Implementation<IClockSynchronizer>::type::GetInstance()->SyncIfNeeded(
            "pool.ntp.org", 15000, 500, "UTC0")) {
        printf("[ERROR] SNTP time sync failed\n");
        return;
    }
    fleetProvisioningService->EnrollDevice();
}

#endif // IKSKSKHFHFH