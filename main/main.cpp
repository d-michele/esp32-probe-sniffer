#include "freertos/FreeRTOS.h"

#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <string>
#include <string.h>
#include <map>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "memory"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "ProbeReq.h"
#include "80211Packet.h"
#include "synchronizeboard.h"


#define ESP_WIFI_SSID      	CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS      	CONFIG_ESP_WIFI_PASSWORD
#define MAX_STA_CONN       	CONFIG_MAX_STA_CONN
#define SERVER_ADDR         CONFIG_SERVER_ADDRESS
#define SERVER_PORT         CONFIG_SERVER_PORT

#define	LED_GPIO_PIN			GPIO_NUM_4
#define	WIFI_CHANNEL_MAX		(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(500)

#define MSG "hello world"

static const char *TAG = "main application";

extern "C" {
    void app_main(void);
}

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t wifi_event_group;
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

//static const char *TAG = "simple wifi";

int connect_to_server();
void wifi_init_sta();

using namespace std;

void wifi_sniffer_init(void) {
	//nvs_flash_init();
    //	tcpip_adapter_init();
	// ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	// wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	// ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) ); /* set country for channel range [1, 13] */
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	// ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
	// ESP_ERROR_CHECK( esp_wifi_start() );
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}
/*
void app_main(void) {
	uint8_t level = 0, channel = 1;

	// setup 
	wifi_sniffer_init();
	gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

	ESP_LOGD("prova", "Hello mondo2");
	ESP_LOGD("sizeof", "%u", sizeof(wifi_ieee80211_mac_hdr_t));
	
	// loop 
	while (true) {
		gpio_set_level(LED_GPIO_PIN, level ^= 1);
		vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
		wifi_sniffer_set_channel(channel);
		channel = (channel % WIFI_CHANNEL_MAX) + 1;
    	}
    }
*/

void app_main()
{
	uint8_t channel = 1;

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // Wifi mode STATION
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
    wifi_sniffer_init();
    // Wait until the device is connected to the wifi
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
    		false, true, portMAX_DELAY);
    //ESP_LOGI(TAG, "Fase di connessione terminata"); // togliere
    SynchronizeBoard::obtain_time();
    //SynchronizeBoard::print_time();
    connect_to_server();

    #ifdef FIXED_CHANNEL
    	wifi_sniffer_set_channel(FIXED_CHANNEL);
    #endif
    
    while (true) {
		#ifndef FIXED_CHANNEL
    		vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
			wifi_sniffer_set_channel(channel);
			channel = (channel % WIFI_CHANNEL_MAX) + 1;
		#else
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        #endif
    	

    }
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR " join, AID=%d", 
                MAC2STR(event->event_info.sta_connected.mac),
                event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:" MACSTR "leave, AID=%d",
                MAC2STR(event->event_info.sta_disconnected.mac),
                event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/*void wifi_init_softap()
{
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config;
    memcpy(wifi_config.ap.ssid ,ESP_WIFI_SSID, sizeof(ESP_WIFI_SSID));
    wifi_config.ap.ssid_len = strlen(ESP_WIFI_SSID),
    memcpy(wifi_config.ap.password, ESP_WIFI_PASS, sizeof(ESP_WIFI_PASS));
    wifi_config.ap.max_connection = MAX_STA_CONN;
    wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s",
             ESP_WIFI_SSID, ESP_WIFI_PASS);
}*/

void
wifi_sniffer_set_channel(uint8_t channel)
{

	ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
}

void wifi_init_sta()
{
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config;
    memset(&wifi_config,0, sizeof(wifi_config));
    memcpy(wifi_config.ap.ssid ,ESP_WIFI_SSID, sizeof(ESP_WIFI_SSID));
    memcpy(wifi_config.ap.password, ESP_WIFI_PASS, sizeof(ESP_WIFI_PASS));


    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    //sniffer configuration
    ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) ); /* set country for channel range [1, 13] */
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );


    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", 
            ESP_WIFI_SSID, ESP_WIFI_PASS);

    //activate sniffer
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);    
}



int espx_last_socket_errno(int socket) {
    int ret = 0;
    u32_t optlen = sizeof(ret);
    getsockopt(socket, SOL_SOCKET, SO_ERROR, &ret, &optlen);
    return ret;
}

esp_err_t connect_to_server() {
    int errCode;
    int ret_conn;
    struct sockaddr_in serverAddress;
    int sErrCode;

    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_ADDR, &serverAddress.sin_addr.s_addr);
    serverAddress.sin_port = htons(8008);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "ERROR: error creating socket";
        return ESP_FAIL;
    }
    cout << "Connecting to: " << inet_ntoa(serverAddress.sin_addr) << 
            ":" << ntohs(serverAddress.sin_port);
    ret_conn = connect(sock, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
    if (ret_conn != 0) {
        sErrCode = espx_last_socket_errno(sock);
        errCode = errno;
        cout << "ERROR: fail to connect" << endl <<
                "Socket Error code: " << sErrCode << endl <<
                "errno :" << errCode << endl;
        close(sock);
        return ESP_FAIL;
    }
    cout << "connection done";
    // char *ptr = "hello world";
    // u16_t writeByte = (u16_t) size_t(ptr); 
    if ((ret_conn = write(sock, MSG, strlen(MSG))) != strlen(MSG)) {
        errCode = errno;
        cout << "error writing file data on disk" << endl <<
                "errno: " << errCode << endl;
        return ESP_FAIL;
    }
      cout << "sent" << endl;

      return ESP_OK;
}
	
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{

	if (type != WIFI_PKT_MGMT) {
		return;
	}
	char buf[32 + 1];
	uint8_t subtype;
	int8_t rssi;
	uint8_t channel;
	uint8_t ssidLen; 

	string ssid;
	array<uint8_t,6> sAddr;
	array<uint8_t,6> dAddr;
	array<uint8_t,6> bssid;
	

	const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
	const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
	const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

	/* filter only PROBE REQ packets */
	subtype = hdr->frame_ctrl;
	subtype = subtype & 0xF0;

	if (subtype != 0x40) {
			return;
	}

	ssidLen = ipkt->ssid_prmtr.tag_len;
	memcpy(buf, ipkt->ssid_prmtr.ssid, ssidLen);

	buf[ssidLen] = '\0';
	ssid = buf;
	rssi = ppkt->rx_ctrl.rssi;
	channel = ppkt->rx_ctrl.channel;
	channel &= 0xF0;
	
	copy(begin(hdr->addr1), end(hdr->addr1), begin(sAddr));
	copy(begin(hdr->addr2), end(hdr->addr2), begin(dAddr));
	copy(begin(hdr->addr3), end(hdr->addr3), begin(bssid));
	
	ProbeReq probe(type, subtype, ppkt->rx_ctrl.channel,
		rssi, ssid, ssidLen, sAddr, dAddr, bssid);

	cout << probe;
}