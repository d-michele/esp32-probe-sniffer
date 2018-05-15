/*
 * 1. Open up the project properties
 * 2. Visit C/C++ General > Preprocessor Include Paths, Macros, etc
 * 3. Select the Providers tab
 * 4. Check the box for "CDT GCC Built-in Compiler Settings"
 * 5. Set the compiler spec command to "xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}""
 * 6. Rebuild the index
*/


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
#include <map>
#include <memory>
#include <stdio.h>
#include "memory"
#include <iostream>


#define	LED_GPIO_PIN			GPIO_NUM_4
#define	WIFI_CHANNEL_MAX		(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(500)
#define WIFI_MGMT_PROBE_REQ 0x40

using namespace std;

static wifi_country_t wifi_country = {.cc="CN", .schan=1, .nchan=13, .policy=WIFI_COUNTRY_POLICY_AUTO};

extern "C" {
	void app_main(void);
}

typedef struct {
	unsigned frame_ctrl:16;
	unsigned duration_id:16;
	uint8_t addr1[6]; /* receiver address */
	uint8_t addr2[6]; /* sender address */
	uint8_t addr3[6]; /* filtering address */
	unsigned sequence_ctrl:16;
} wifi_ieee80211_mac_hdr_t;

typedef struct {
	//unsigned tag_number:8;
	//unsigned tag_len:8;
	uint8_t tag_number;
	uint8_t tag_len;
	unsigned char ssid[0];
} ssid_parameter_set;

typedef struct {
	wifi_ieee80211_mac_hdr_t hdr;

	ssid_parameter_set ssid_prmtr;
	uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

typedef struct {
	uint8_t timestamp;
	unsigned beacin_interval:16;
	unsigned capability_info:16;

}wifi_iee80211_mac_body_t;


// typedef struct 
// {
// 	unsigned:8;
// } subType;



static esp_err_t event_handler(void *ctx, system_event_t *event);
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
static const char *wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);
//static void vendor_wifi_sniffer_packet_handler (void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6],
//		const vendor_ie_data_t *vnd_ie, int rssi);


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
	map <string,string> map;
	shared_ptr<string> ptr = make_shared<string>("cane");
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
    //esp_wifi_set_vendor_ie(true, WIFI_VND_IE_TYPE_PROBE_REQ, 1, NULL);
	//esp_wifi_set_vendor_ie_cb(&vendor_wifi_sniffer_packet_handler, NULL);
	esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void
wifi_sniffer_set_channel(uint8_t channel)
{

	esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

const char *
wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type)
{
	switch(type) {
	case WIFI_PKT_MGMT: return "MGMT";
	case WIFI_PKT_DATA: return "DATA";
	default:
	case WIFI_PKT_MISC: return "MISC";
	}
}

string
wifi_sniffer_packet_subtype2str(uint8_t subtype) {
	switch (subtype) {
		case WIFI_MGMT_PROBE_REQ: return string("PROBE REQ");
		default: 
			return string("");
	}
}

void
wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{

	if (type != WIFI_PKT_MGMT) {
		return;
	}
	uint8_t subtype;
	//printf("%u", type);
	//memcpy(subtype, buff + 0x30, 1);
//	subtype = (unsigned char*)buff + 0x1C;
//	printf("porcaccio gesu: %02x", *subtype);
//	if (*subtype == 0x40) {
//		printf("leskere");
//	}
//	for (int i = 0; i < 100; i++) {
//		subtype = (unsigned char*) buff + i;
//		printf("%02x", *subtype);
//		if (i % 16 == 0)
//			printf("\n");
//	}

	const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
	const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
	const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

	subtype = hdr->frame_ctrl;
	if (subtype != 0x40) {
			return;
	}

	uint8_t len = ipkt->ssid_prmtr.tag_len;

	//cout << len;
	cout << (int)ipkt->ssid_prmtr.tag_number << " " << (int)ipkt->ssid_prmtr.tag_len << "\n"; 
	for (uint8_t i = 0; i < len; i++){
		cout << ipkt->ssid_prmtr.ssid[i];
	}
	//const wifi_beacon_payload *bpay = ipkt->payload;
	cout << "SSID= " << wifi_sniffer_packet_subtype2str(subtype);
	//cout << hex << hdr->frame_ctrl;
	printf("PACKET TYPE=%s, CHAN=%02d, RSSI=%02d,"
		" ADDR1=%02x:%02x:%02x:%02x:%02x:%02x,"
		" ADDR2=%02x:%02x:%02x:%02x:%02x:%02x,"
		" BSSID=%02x:%02x:%02x:%02x:%02x:%02x \n",
		wifi_sniffer_packet_type2str(type),
		ppkt->rx_ctrl.channel,
		ppkt->rx_ctrl.rssi,
		/* ADDR1 */
		hdr->addr1[0],hdr->addr1[1],hdr->addr1[2],
		hdr->addr1[3],hdr->addr1[4],hdr->addr1[5],
		/* ADDR2 */
		hdr->addr2[0],hdr->addr2[1],hdr->addr2[2],
		hdr->addr2[3],hdr->addr2[4],hdr->addr2[5],
		/* ADDR3 */
		hdr->addr3[0],hdr->addr3[1],hdr->addr3[2],
		hdr->addr3[3],hdr->addr3[4],hdr->addr3[5]
	);

}

// void
// vendor_wifi_sniffer_packet_handler (void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6],
// 		const vendor_ie_data_t *vnd_ie, int rssi) {
// 	printf("topp");
//}





