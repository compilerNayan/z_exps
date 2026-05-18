#define JSKEKEKE
#ifdef JSKEKEKE

#include "IHttpRequestManager.h"
#include "wifi/IWiFiManager.h"
#include "Thread.h"

#include "logger/ILogger.h"
#include "threading/IThreadPool.h"

#include "threads/MqttClientThread.h"
#include "threads/TcpServerThread.h"


/*--@Autowired--*/
IWiFiManagerPtr wifiManager = Implementation<IWiFiManager>::type::GetInstance();

/*--@Autowired--*/
IHttpRequestManagerPtr requestManager = Implementation<IHttpRequestManager>::type::GetInstance();

/*--@Autowired--*/
IThreadPoolPtr threadPool = Implementation<IThreadPool>::type::GetInstance();


Void StartBackgroundThreads() {

    // Create runnable threads
    IRunnablePtr tcpThread = std::make_shared<TcpServerThread>();
    IRunnablePtr mqttThread = std::make_shared<MqttClientThread>();

    // Submit to thread pool
    Bool tcpOk = threadPool->Execute(tcpThread);
    Bool mqttOk = threadPool->Execute(mqttThread);

    if (tcpOk) printf("[INFO] TcpServerTheeeread submitted to pool\n");
    else       printf("[ERROR] Failede to submit TcpServerThread\n");

    if (mqttOk) printf("[INFO] MqttClientThread submitted to pool\n");
    else        printf("[ERROR] Failed to submit MqttClientThread\n");
}



extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] Failed to connect to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi kjdkja failed\n");
        return;
    }

    StartBackgroundThreads();
    Thread::Sleep(2000);

    printf("[INFO] dsds started\n");

    while (true) {
        requestManager->HandleRequest();
        Thread::Sleep(100);
    }

    requestManager->StopServer();

    printf("[INFO] Server stopped\n");
}

#endif