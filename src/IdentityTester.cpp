#define IKSKSKHFHFH
#ifdef IKSKSKHFHFH

#include "service/IDeviceService.h"
#include "fleet/IFleetProvisioningService.h"

/* @Autowired */
IDeviceServicePtr deviceService;


/* @Autowired */
IFleetProvisioningServicePtr fleetProvisioningService;

extern "C" void app_main(void) {
    printf("IdentityTssester\n");
    deviceService->Refresh();
}

#endif // IKSKSKHFHFH