#define KSKSJDH
#ifdef KSKSJDH

#include "Thread.h"

#include "logger/ILogger.h"
#include "threading/IThreadPool.h"

#include "WiFiHealthCheckerThread.h"


/*--@Autowired--*/
IThreadPoolPtr threadPool = Implementation<IThreadPool>::type::GetInstance();


Void StartBackgroundThreads() {

    // Create runnable threads
    IRunnablePtr wifiHealthCheckerThread = std::make_shared<WiFiHealthCheckerThread>();

    // Submit to thread pool
    Bool wifiHealthCheckerThreadOk = threadPool->Execute(wifiHealthCheckerThread);

    if (wifiHealthCheckerThreadOk) printf("[INFO] WiFiHealthCheckerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit WiFiHealthCheckerThread\n");
}



extern "C" void app_main(void) {
    StartBackgroundThreads();
    Thread::Sleep(2000);
}


#endif