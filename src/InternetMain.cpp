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
#include "DeviceManagerThread.h"


/*--@Autowired--*/
IThreadPoolPtr threadPool = Implementation<IThreadPool>::type::GetInstance();


// Helper for WiFiHealthCheckerThread
Void StartWiFiHealthCheckerThread() {
    IRunnablePtr thread = std::make_shared<WiFiHealthCheckerThread>();
    Bool ok = threadPool->Execute(thread);
    if (ok) printf("[INFO] WiFiHealthCheckerThread submitted to pool\n");
    else    printf("[ERROR] Failed to submit WiFiHealthCheckerThread\n");
}

// Helper for InternetHealthCheckerThread
Void StartInternetHealthCheckerThread() {
    IRunnablePtr thread = std::make_shared<InternetHealthCheckerThread>();
    Bool ok = threadPool->Execute(thread);
    if (ok) printf("[INFO] InterjkjnetHealthCheckerThread submitted to pool\n");
    else    printf("[ERROR] Failed to submit InternetHealthCheckerThread\n");
}

// Helper for TcpServerThread
Void StartTcpServerThread() {
    IRunnablePtr thread = std::make_shared<TcpServerThread>();
    Bool ok = threadPool->Execute(thread);
    if (ok) printf("[INFO] TcpServerThread submitted to pool\n");
    else    printf("[ERROR] Failed to submit TcpServerThread\n");
}

// Helper for MqttClientThread
Void StartMqttClientThread() {
    IRunnablePtr thread = std::make_shared<MqttClientThread>();
    Bool ok = threadPool->Execute(thread);
    if (ok) printf("[INFO] MqttClientThread submitted to pool\n");
    else    printf("[ERROR] Failed to submit MqttClientThread\n");
}

// Helper for RequestManagerThread
Void StartRequestManagerThread() {
    IRunnablePtr thread = std::make_shared<RequestManagerThread>();
    Bool ok = threadPool->Execute(thread);
    if (ok) printf("[INFO] RequestManagerThread submitted to pool\n");
    else    printf("[ERROR] Failed to submit RequestManagerThread\n");
}

// Helper for DeviceManagerThread
Void StartDeviceManagerThread() {
    IRunnablePtr thread = std::make_shared<DeviceManagerThread>();
    Bool ok = threadPool->Execute(thread);
    if (ok) printf("[INFO] DeviceManagerThread submitted to pool\n");
    else    printf("[ERROR] Failed to submit DeviceManagerThread\n");
}

// Main function now just orchestrates
Void StartBackgroundThreads() {
    StartWiFiHealthCheckerThread();
    StartInternetHealthCheckerThread();
    StartTcpServerThread();
    StartMqttClientThread();
    StartRequestManagerThread();
    StartDeviceManagerThread();
}



extern "C" void app_main(void) {
    StartBackgroundThreads();
    Thread::Sleep(2000);
}


#endif