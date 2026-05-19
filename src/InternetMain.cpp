#define KSKSJDH
#ifdef KSKSJDH

#include "Thread.h"

#include "logger/ILogger.h"
#include "threading/IThreadPool.h"

#include "WiFiHealthCheckerThread.h"
#include "InternetHealthCheckerThread.h"
/*#include "LocalServerThread.h"
#include "CloudServerThread.h"
#include "RequestManagerThread.h"*/


/*--@Autowired--*/
IThreadPoolPtr threadPool = Implementation<IThreadPool>::type::GetInstance();


Void StartBackgroundThreads() {

    // Create runnable threads
    IRunnablePtr wifiHealthCheckerThread = std::make_shared<WiFiHealthCheckerThread>();

    IRunnablePtr internetHealthCheckerThread = std::make_shared<InternetHealthCheckerThread>();

    // Submit to thread pool
    Bool wifiHealthCheckerThreadOk = threadPool->Execute(wifiHealthCheckerThread);
    Bool internetHealthCheckerThreadOk = threadPool->Execute(internetHealthCheckerThread);



    if (wifiHealthCheckerThreadOk) printf("[INFO] WiFiHealthCheckerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit WiFiHealthCheckerThread\n");

    if (internetHealthCheckerThreadOk) printf("[INFO] InternetHealthCheckerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit InternetHealthCheckerThread\n");
}



extern "C" void app_main(void) {
    StartBackgroundThreads();
    Thread::Sleep(2000);
}


#endif