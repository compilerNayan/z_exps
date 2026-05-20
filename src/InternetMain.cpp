#define KSKSJDH
#ifdef KSKSJDH

#include "Thread.h"

#include "logger/ILogger.h"
#include "threading/IThreadPool.h"

#include "WiFiHealthCheckerThread.h"
#include "InternetHealthCheckerThread.h"
#include "TcpServerThread.h"
#include "MqttClientThread.h"
#include "RequestManagerThread.h"


/*--@Autowired--*/
IThreadPoolPtr threadPool = Implementation<IThreadPool>::type::GetInstance();


Void StartBackgroundThreads() {

    // Create runnable threads
    IRunnablePtr wifiHealthCheckerThread = std::make_shared<WiFiHealthCheckerThread>();

    IRunnablePtr internetHealthCheckerThread = std::make_shared<InternetHealthCheckerThread>();

    IRunnablePtr tcpServerThread = std::make_shared<TcpServerThread>();
    IRunnablePtr mqttClientThread = std::make_shared<MqttClientThread>();
    IRunnablePtr requestManagerThread = std::make_shared<RequestManagerThread>();

    // Submit to thread pool
    Bool wifiHealthCheckerThreadOk = threadPool->Execute(wifiHealthCheckerThread);
    Bool internetHealthCheckerThreadOk = threadPool->Execute(internetHealthCheckerThread);
    Bool tcpServerThreadOk = threadPool->Execute(tcpServerThread);
    Bool mqttClientThreadOk = threadPool->Execute(mqttClientThread);
    Bool requestManagerThreadOk = threadPool->Execute(requestManagerThread);

    if (wifiHealthCheckerThreadOk) printf("[INFO] WiFiHealthCheckerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit WiFiHealthCheckerThread\n");

    if (internetHealthCheckerThreadOk) printf("[INFO] InternetHealthCheckerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit InternetHealthCheckerThread\n");


    if (tcpServerThreadOk) printf("[INFO] TcpServerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit TcpServerThread\n");

    if (mqttClientThreadOk) printf("[INFO] MqttClientThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit MqttClientThread\n");

    if (requestManagerThreadOk) printf("[INFO] RequestManagerThread submitted to pool\n");
    else       printf("[ERROR] Failed to submit RequestManagerThread\n");

}



extern "C" void app_main(void) {
    StartBackgroundThreads();
    Thread::Sleep(2000);
}


#endif