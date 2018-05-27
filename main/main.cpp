#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include "freertos/task.h"
#include "sdkconfig.h"
#include <string>
#include <string.h>
#include <map>
#include <memory>
#include <stdio.h>
#include "memory"
#include <iostream>
#include <iomanip>
#include "80211Packet.h"
#include "ProbeReq.h"

#define	LED_GPIO_PIN			GPIO_NUM_4
#define	WIFI_CHANNEL_MAX		(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(500)

using namespace std;

static wifi_country_t wifi_country = {.cc="CN", .schan=1, .nchan=13, .policy=WIFI_COUNTRY_POLICY_AUTO};

extern "C" {
	void app_main(void);
}

static esp_err_t event_handler(void *ctx, system_event_t *event);
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);

void app_main(void) {
	uint8_t level = 0, channel = 1;

	/* setup */
	wifi_sniffer_init();
	gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

	ESP_LOGD("prova", "Hello mondo2");
	ESP_LOGD("sizeof", "%u", sizeof(wifi_ieee80211_mac_hdr_t));
	
	/* loop */
	while (true) {
		gpio_set_level(LED_GPIO_PIN, level ^= 1);
		vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
		wifi_sniffer_set_channel(channel);
		channel = (channel % WIFI_CHANNEL_MAX) + 1;
    	}
}

esp_err_t
event_handler(void *ctx, system_event_t *event)
{

	return ESP_OK;
}

void
wifi_sniffer_init(void)
{

	nvs_flash_init();
    	tcpip_adapter_init();
    	ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_country(&wifi_country) ); /* set country for channel range [1, 13] */
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
    	ESP_ERROR_CHECK( esp_wifi_start() );
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void
wifi_sniffer_set_channel(uint8_t channel)
{
	ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
}

void
wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
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
