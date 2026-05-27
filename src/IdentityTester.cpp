#define IKSKSKHFHFH
#ifdef IKSKSKHFHFH

#include "service/IDeviceService.h"

#include "provider/IDeviceConfigProvider.h"

/* @Autowired */
IDeviceServicePtr deviceService;

/* @Autowired */
IDeviceConfigProviderPtr deviceConfigProvider;

extern "C" void app_main(void) {
    printf("IdentityTssester\n");
    deviceService->Refresh();
}

#endif // IKSKSKHFHFH