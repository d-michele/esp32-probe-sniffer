#ifndef PDSPROJECT_CONSUMERTASK_H
#define PDSPROJECT_CONSUMERTASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include <iostream>
#include <map>
#include <iomanip>
#include <sstream>
#include "esp_wifi.h"
#include "esp_log.h"
#include "80211Packet.h"
#include "string.h"
#include "mbedtls/md5.h"
#include "ProbeReq.h"
#include "socketUtils.h"
#include "esp_heap_trace.h"

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG                   /* level of logging */
#define FIXED_CHANNEL 1                                /* fixed channel to sniff */

#define NUM_RECORDS 100

static heap_trace_record_t trace_record[NUM_RECORDS]; // This buffer must be in internal RAM


using namespace std;

extern const char *TAG;

class ConsumerTask {
public:
	enum Keys {
		SOCKET = 1,
		RINGBUFFER = 2
	};

	/**
	  * @brief extract packet from Ringbuffer     
	  *
	  * @param     fd 		socket where sending data
	  */
	static void consume(void *args);	
};

#endif //PDSPROJECT_CONSUMERTASK_H
