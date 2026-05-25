extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "esp_event.h"
    #include "esp_log.h"
    #include "esp_netif.h"
    #include "esp_sntp.h"
    #include "esp_wifi.h"
    #include "mqtt_client.h"
    #include "nvs_flash.h"
    #include <sys/time.h>
    #include <errno.h>
    #include <string.h>
}

static const char *TAG = "fleet_provision";

static const char ROOT_CA_PEM[] = R"PEM(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)PEM";

#if 1  // Fleet provisioning claim credentials (disabled for debug)
static const char CLAIM_CERT_PEM[] = R"PEM(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUbhSAAK4ZdEPpLubg50xG+hhiqygwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDUyNDEyMzIz
NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMWhTCQTdnTqEIjlPdsy
Iaurha08yJGEiGpehUqOWad9hLK93LE/hTrmzqXDXNBmXJqm6SA0K3Kz/fSNEdWx
uldrAfdziErz3Cd3Kw693RYtMuGH3/nVLxTD6JycL5lPqsKuHKgjdLNxrcYqRXuU
MLtQXxj7fpB3xZEhV5yX/KZ+raJm9xYP7pcF3zWfZXQ5jw8kHqYjsOvF43Xz5GJh
TCYIqSMGDU72NT03AQY2WXYwF2Y3FjmbEoamYF7Hh9gzCLezqOiCgPHrNHpv/3md
XYeibvO2uJFMulRJQK2lY5wcbPAsAhaRn71bLyi19SkFP0m8IuziA8pNHUd1Qjoh
EgsCAwEAAaNgMF4wHwYDVR0jBBgwFoAUDM0GONaRXxKeVvJ8fttSwTuYExYwHQYD
VR0OBBYEFHw4IjjpVe6g5LrQ6gxVTY8LaV5IMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQC0gLuywa34JgURcspGSonH/taz
d2PyBke+3CDRGcO/6gD7JW7Qgczj8WRyS6Aze7oo+/1LM4sTd/tq+Bw7HXrpub5g
AV065pSSQdjl3RyOfgo8QTpMgfEHgaH2WAAsjUhMlHJxjFXvigkzzoIIMtJGMdat
Kp3yjmVO06DyEnAP12iEvNx+jW3i2LgUR58HxSUOnNer4mfWxiQ8XeIggVN4QLN3
R4pCGWaX1jh7pyxWkB0MC3WT3+lsAmT/ATrC8GCB6JaefLFeEW3W5J2yYFiQAINj
YL/Wh5H6YWqXNBAsxEbfOu5/uwPW/HkHZN/1tkIKHnJlgTKvEKTQtkwd2fIs
-----END CERTIFICATE-----
)PEM";

static const char CLAIM_PRIVATE_KEY_PEM[] = R"PEM(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAxaFMJBN2dOoQiOU92zIhq6uFrTzIkYSIal6FSo5Zp32Esr3c
sT+FOubOpcNc0GZcmqbpIDQrcrP99I0R1bG6V2sB93OISvPcJ3crDr3dFi0y4Yff
+dUvFMPonJwvmU+qwq4cqCN0s3GtxipFe5Qwu1BfGPt+kHfFkSFXnJf8pn6tomb3
Fg/ulwXfNZ9ldDmPDyQepiOw68XjdfPkYmFMJgipIwYNTvY1PTcBBjZZdjAXZjcW
OZsShqZgXseH2DMIt7Oo6IKA8es0em//eZ1dh6Ju87a4kUy6VElAraVjnBxs8CwC
FpGfvVsvKLX1KQU/Sbwi7OIDyk0dR3VCOiESCwIDAQABAoIBAQCm+VeGZ3c+4n+j
2eOALgPqshlYO7HN2KcVD9UYH9s5usAB8I3kp6FeGaHgxYXGgaWd+IWg8tL+r3pS
Q1odLXMd5sgblN5DTXPlWDbl1p5T3U5O5GjlASwtJDSFFSTWKExC/a4ylPpFnjdO
5WodNiBSMl6Wke3mC3u92ejW4kbRN1v/Ej8iCOzUpEcLwznVzLJiXcEuv9I2OlaS
UTKUjnFkgkKDFGrlUb52VTQBm5WuQh2krNaF1dIt1jdjgl3tkLNptXxkm0VHsED6
TFbE6QfZHrzKwD/Ro9MzqJhrpnFmEzS+aFHR3U0W8AWZPqsdsyT3BBcpKlfe1Wyw
PKjdYUiZAoGBAOpcplg12mQsUatMOeOxL3gxIKNngaVvFsNgyYLZzPnj6IhkhRM9
36e6WKtiuUmfVP71VY+yNqJOhlGXv9VzXOmd+xBHBb/phby/8OOSKrjBGzaQR87r
U4rAruX4w+XYSuCFM7a6n0fhnrqHPbtzq5/mYt60WlM4/76G3fHuQMOtAoGBANfg
dUgzTfPgrExcHokaiLVZ32vbFTL02OX2yojcZpi2wYZO++434mXOMZ0Pp2WJ4LtX
YYNV7OkcxrcDdV/w9CG+PmetomqbyVyMwsMl+G5Px3ZVpD7AQkI+U3bIeHsOz653
TzGoNVmJF9VzrusH75nuiN9VVMutmuWVux3RgSOXAoGAXXEHWdBO/PW2ske4JRJ2
TMO5sa0JQf6i4s9qSOOyGEhIZGk/rcZTsDpaNCsumS4P43/J13tE0T3hzBJnCCHw
/6xfd8P5hOUEz5bv4qxaoENOKFkqaFPngUEF+udOpAuy/wXc8pRCAZTxhol5kY2y
9hQTg5wBD4GAZmS7hURrShkCgYArZDWOxqkzjQR08Ms+ebPi5+atskKnlGmIh7uc
GNkSdyGdEoE3F8qM9g4o6L2X/kfKwDycfof5L0yN+sb7+t3HboryogcUcK3fyXdY
bGhsG+MW2Q4DgveOgyKZrJ/y9D4wblDfxVS743+D6xXMrjKt+TFP2sIJkqKg5SDQ
VI0ljwKBgF1LTirlf5RffwVfznbYjAGx8q3oqZ89+8PZo+Gi+v/wBJKFQJnSBRoM
o9kV+L0pOJyKXTRkNdRu6BadeQElV1zYxTSw4kfPxClS9W2nbF8F3nr2kknkNxH+
17RW1/TuGY30btn+KItTgZFcOTGU7nNkRbiOAHHkHs/RmEAFksJg
-----END RSA PRIVATE KEY-----
)PEM";
#endif

#if 0
// AwsIotCoreConfigProvider credentials (thing: nktestesp) — active for debug
static const char DEVICE_CERT_PEM[] = R"PEM(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUQp8M9BjYXE+idNY/5RfHjjn6IAMwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDUwNzEzMzU0
MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC


'AQoCggEBALhEYqYgbC1ScGYcnQ3m
i99U+1M+4P5EtoqXdErdksw1hlOb1eb9fw+QB8h5PqqGJZsms78GBMuv5gI+CHJU
pKBFmXYoDAB8dpYZX+K+nJyF6knOzauHjMlCu2h9zGBrqQBYvJLZS+jQr0TZgVi2
81lHZX+aesCPZuCCfMXZGWnrSD7fc6IPevwZaLDZEGPG8A/K5bl6c7KGjRNMJ5t/
GOKn4L5CzrTnJEUcuHuRa1Pdhuvht8NwNhYv6Wxs4svzQwhhcE3S1ROPmgFlFVg6
2fdCT16voxiDDl4C4aZAhxwyq0W7S9/7NUpzWZ56Jb/rmEO/MOTzcBg3SKhi1r4A
fTMCAwEAAaNgMF4wHwYDVR0jBBgwFoAUy1bwv08wSVTXzRwdV/DT54YC8JEwHQYD
VR0OBBYEFOFD0h3HUO3qjg48WutYEnmhrtMTMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB+cBYXaJAcsTPePLgQjr6qDKa/
IQEVpjDhc03PPKxZNXROYd0OzHUuz5ZZzI0Oe0L61+kOr+nbH1HCoZviQluTyPbx
cnHmpkKs9Rvw6xEkEdGpq+krMuvy7XzE2pB5uRbeBt4eIMNwaWY1fhwHEfmepj4W
l8BtsbsYRx7ETouB0JyU1NOmab6TmHvnq5gpWsK94vkO8T9r0OccSGOup9fO8y7u
jA7ffKm53+CKcf5jT68VZWfO8QZFmUPn7NqxyScn5eu8FsVAYoQ1tW2RDG+boMvI
qe4M49X49JJDhluDRaLSjBn9JO27dTccEcQzTvxjSsGC/6Dn4aU80JDjy7A+
-----END CERTIFICATE-----
)PEM";

static const char DEVICE_PRIVATE_KEY_PEM[] = R"PEM(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAuERipiBsLVJwZhydDeaL31T7Uz7g/kS2ipd0St2SzDWGU5vV
5v1/D5AHyHk+qoYlmyazvwYEy6/mAj4IclSkoEWZdigMAHx2lhlf4r6cnIXqSc7N
q4eMyUK7aH3MYGupAFi8ktlL6NCvRNmBWLbzWUdlf5p6wI9m4IJ8xdkZaetIPt9z
og96/BlosNkQY8bwD8rluXpzsoaNE0wnm38Y4qfgvkLOtOckRRy4e5FrU92G6+G3
w3A2Fi/pbGziy/NDCGFwTdLVE4+aAWUVWDrZ90JPXq+jGIMOXgLhpkCHHDKrRbtL
3/s1SnNZnnolv+uYQ78w5PNwGDdIqGLWvgB9MwIDAQABAoIBADU14jrQ0AxCaxzB
lITIF4oVc0H1vWkPlJru43EfDN4VcsXAEYPTnhZzZBv7DyKU/FUSVwOJjGb5U3VA
TpYdZTC3CHq7U8PsLR5pE2xZ2MXGUgKgSnoG+ZvAvneNJDCPxkzIUyQgxa/kW0iJ
erWQlzMnz+KKTHpArteUNs23RIR2JIPR1zsPbvaDJHzdZWSnKQOumtZleKYf1Nv8
9tJGC5NZi7OcKJlmS0Cz3H1dULDQWRvkR8Wqy8KybAXebVTPY33jRyy0lrdXy9mD
hPrUEuu3D2iTQw+qz4wH17iaO5g0tnhwo1JaohiMGmuMHAYIKBuWl53OaQ8GBr9M
uX0emIkCgYEA22p8s0qPhuyB29Ct9eW0XEK5Vkmo944PJts54xhqE0BWu2loZ9O6
+vmhzFqVANmBvIs9NVqB/y8+PB8mstT+7f42MOpHTLe6DiSGjYjx9YWYtjIo+xkB
ksNvC6+GH2MDXVQvr4RMSzI9+e8g1hv9mPVxTWG21v/25j0DY4EDIp8CgYEA1v2c
idDC2C6tARYydW8y5eYnJ/DqpQpP4wyjZvnPpMZZ+nLRSCN05rADCvQ6P1oVGVvg
NX7glj9zA17oSw5jGfMfNSmgExfoYw+XP+iX2nNJN1ncFa01cbe5GzPwVcDkHJ8p
+bxVuWvzs0KOkrEygpN8XXC7NpyZbDxVMxFgkO0CgYEAjusNzrsCQiBLdaK3rTOy
B2Zu0oWoSZd+xxCE5CdC8Mc3t1sxZm/DmzZ+7V0TMTp8BxOrCtWpV06uevrPeizC
4MJ1CeeQCaiiyBOCkS5ibEpY7lQ93VCsdaxW6IWsF0WnK2NqrCjAiTpwpBveh5gz
1y4IXx8GuYSAjfev/WBLaoECgYASRn6/DFrzy4vzUrK5XtmBx32+nHEl1GmUEGf3
gfbPt/jAHHCD5Fe0oHOxw68L4Zl3jWLSyVVIEF6zg6bVcoSkGRGnEEHGPbsIaLPf
v+Lgs/oeVWorAY1QfGCk2lZsw7fgYvtLemIDIBUYQOkjuSJlDSp+Ou3/J3K3S62m
aKy/xQKBgQCxLxXrvOineXzBeNXewSmtpkyUsrRbQRuTXkusSibZlAnFiVH5fnrM
gkssBwD5OAULIkPsefvrji0lc3/KQ8g+QUg/XBJZrnfj7bRi/XY6s7UqmNmxvZTK
ZgfIjvc/bSZo6CWAI1KvVUbJF7FDhSmcRX/Lrx7zVbeMjGbhZkRLng==
-----END RSA PRIVATE KEY-----
)PEM";

#endif

static esp_mqtt_client_handle_t mqtt_client = nullptr;
static bool mqtt_started = false;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

static bool sync_time(void) {
    time_t now = 0;
    time(&now);
    if (now > 1700000000) {
        ESP_LOGI(TAG, "System time already set");
        return true;
    }

    ESP_LOGI(TAG, "Syncing time via SNTP...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();

    for (int i = 0; i < 15; ++i) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        time(&now);
        if (now > 1700000000) {
            esp_sntp_stop();
            ESP_LOGI(TAG, "SNTP sync OK, epoch=%ld", (long)now);
            return true;
        }
        ESP_LOGI(TAG, "Waiting for SNTP... (%d/15)", i + 1);
    }

    esp_sntp_stop();
    ESP_LOGE(TAG, "SNTP sync failed");
    return false;
}

static void start_mqtt_client(void) {
    if (mqtt_started) {
        return;
    }

    if (!sync_time()) {
        return;
    }

    mqtt_started = true;

    ESP_LOGI(TAG, "Starting MQTT client...");

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = "mqtts://a2hlcpmplecdfa-ats.iot.us-east-1.amazonaws.com";
    mqtt_cfg.broker.verification.certificate = ROOT_CA_PEM;
    mqtt_cfg.credentials.client_id = "esp32-claim";
    mqtt_cfg.credentials.authentication.certificate = CLAIM_CERT_PEM;
    mqtt_cfg.credentials.authentication.key = CLAIM_PRIVATE_KEY_PEM;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

static void wifi_event_handler(void *arg, esp_event_base_t base, int32_t event_id, void *event_data) {
    if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        auto *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        start_mqtt_client();
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    auto event = (esp_mqtt_event_handle_t)event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED: {
            ESP_LOGI(TAG, "Connected with device certificate (nktestesp)");
            const char *payload = "{\"parameters\":{\"SerialNumber\":\"KKABC123\"}}";
            esp_mqtt_client_publish(event->client,
                "$aws/provisioning-templates/NayanTemplate/provision/json",
                payload, 0, 1, 0);
            break;
        }
        case MQTT_EVENT_DATA: {
            ESP_LOGI(TAG, "Received data on topic: %.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "Payload: %.*s", event->data_len, event->data);
            break;
        }
        case MQTT_EVENT_ERROR: {
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle) {
                ESP_LOGE(TAG, "error_type=%d tls_esp_err=0x%x tls_stack_err=0x%x sock_errno=%d (%s)",
                    event->error_handle->error_type,
                    event->error_handle->esp_tls_last_esp_err,
                    event->error_handle->esp_tls_stack_err,
                    event->error_handle->esp_transport_sock_errno,
                    strerror(event->error_handle->esp_transport_sock_errno));
                if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
                    ESP_LOGE(TAG, "MQTT CONNACK refused, code=%d",
                        event->error_handle->connect_return_code);
                }
            }
            break;
        }
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI(TAG, "MQTT connecting...");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            break;
        default:
            ESP_LOGI(TAG, "Unhandled event id=%d", event->event_id);
            break;
    }
}

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting Fleet Provisioning enrollment...");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, "Garfield");
    strcpy((char *)wifi_config.sta.password, "123Madhu$$");
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}
