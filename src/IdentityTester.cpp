#define IKSKSKHFHFH
#ifdef IKSKSKHFHFH

#include "service/IDeviceService.h"

/* @Autowired */
IDeviceServicePtr deviceService;

extern "C" void app_main(void) {
    printf("IdentityTssester\n");
    deviceService->Refresh();
}

#endif // IKSKSKHFHFH