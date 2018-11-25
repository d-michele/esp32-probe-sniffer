#ifndef PDSPROJECT_ESP32_PDS_H
#define PDSPROJECT_ESP32_PDS_H

#include "freertos/event_groups.h"

#define ESP_WIFI_SSID      	CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS      	CONFIG_ESP_WIFI_PASSWORD
#define MAX_STA_CONN       	CONFIG_MAX_STA_CONN
#define	LED_GPIO_PIN			GPIO_NUM_4               /* blinking blue led */
#define	WIFI_CHANNEL_MAX		(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(500)
#define FIXED_CHANNEL 1                                /* fixed channel to sniff */
#define STACK_SIZE 4096                                 /* consumer task size */
#define RINGBUF_SIZE 4096                               /* size of ringbuffer */
#define SERVER_ADDR         CONFIG_SERVER_ADDRESS
#define SERVER_PORT         CONFIG_SERVER_PORT
#define SNTP_SERVER_IP 		CONFIG_SNTP_SERVER_IP
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG                   /* level of logging */

/* logging tag */
extern const char *TAG;

extern EventGroupHandle_t wifi_event_group;

extern const int WIFI_CONNECTED_BIT;

#endif //PDSPROJECT_ESP_32_PDS_H
