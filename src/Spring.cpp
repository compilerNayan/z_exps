#define JSKEKEKE
#ifdef JSKEKEKE

#include "IHttpRequestManager.h"
#include "wifi/IWiFiManager.h"
#include "Thread.h"

#include "logger/ILogger.h"
#include "IThreadPool.h"

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
    Bool tcpOk = pool->Execute(tcpThread);
    Bool mqttOk = pool->Execute(mqttThread);

    if (logger) {
        if (tcpOk) logger->Info(Tag::Untagged, "TcpServerThread submitted to pool");
        else       logger->Error(Tag::Untagged, "Failed to submit TcpServerThread");

        if (mqttOk) logger->Info(Tag::Untagged, "MqttClientThread submitted to pool");
        else        logger->Error(Tag::Untagged, "Failed to submit MqttClientThread");
    }
}



extern "C" void app_main(void) {
    if (!wifiManager->Connect("Garfield", Optional<StdString>{"123Madhu$$"})) {
        printf("[ERROR] Failed to connect to WiFi\n");
        return;
    }
    if (!wifiManager->WaitForConnection(10000) || !wifiManager->IsConnected()) {
        printf("[ERROR] WiFi cesdonnection failed\n");
        return;
    }

    StartBackgroundThreads();
    Thread::Sleep(2000);

    requestManager->StartServer();

    printf("[INFO] Server started\n");

    while (true) {
        requestManager->RetrieveRequest();
        Thread::Sleep(1000);
    }

    requestManager->StopServer();

    printf("[INFO] Server stopped\n");
}

#endif