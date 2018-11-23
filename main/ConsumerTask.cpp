#include "ConsumerTask.h"

void ConsumerTask::consume(void *args) {
    ESP_LOGI(TAG, "Consumer task created");
    map<int, void*> *argsMap = static_cast<map<int, void*>*> (args);
    auto it = argsMap->find(ConsumerTask::Keys::SOCKET);
    assert(it != argsMap->end() && "error missing socket argument");
    int socket = (int) it->second;
    ESP_LOGD(TAG, "socket: %d", socket);
    it = argsMap->find(ConsumerTask::Keys::RINGBUFFER);
    assert(it != argsMap->end() && "error missing ringbuffer argument");
    RingbufHandle_t packetRingBuffer = (RingbufHandle_t) it->second;
    delete argsMap;

    // ESP_ERROR_CHECK( heap_trace_init_standalone(trace_record, NUM_RECORDS) );

    while (1) {
        char buf[32 + 1];    
        uint8_t subtype;
        int8_t rssi;
        uint8_t channel;
        uint8_t ssidLen;
        // string ssid;
        // array<uint8_t,6> sAddr;
        // array<uint8_t,6> dAddr;
        // array<uint8_t,6> bssid;
        unsigned char md5digest[16];

        size_t packetSize;  

        // ESP_ERROR_CHECK( heap_trace_start(HEAP_TRACE_LEAKS) );

        void *probePacket = xRingbufferReceive(packetRingBuffer, &packetSize, portMAX_DELAY);
        if (probePacket == nullptr) {
            ESP_LOGE(TAG, "Error retrieving element from queue\n");
            continue;
        }
        // ESP_LOGD(TAG, "(Core %d) Extracted element from queue, size: %d",xPortGetCoreID(), packetSize);        

        const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)probePacket;
        const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
        const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
        /* filter only PROBE REQ packets */
        subtype = hdr->frame_ctrl;
        subtype = subtype & 0xF0;
        if (subtype != 0x40) {
            // ESP_LOGD(TAG, "remaining size %d", xRingbufferGetCurFreeSize(packetRingBuffer));
            vRingbufferReturnItem(packetRingBuffer, probePacket);
            continue;
        }
        // i don't know why but last 4 bytes (of FCS i think) are different for any ESP receiver
        int payloadSize = ppkt->rx_ctrl.sig_len - 4;
        // debug
        // dumpPacket(ppkt, payloadSize);
        uint8_t *payloadHash = new uint8_t[payloadSize];
        bzero(payloadHash, payloadSize);
        memcpy(payloadHash, ppkt->payload, payloadSize);
        // extracting packet info from the payload
        rssi = ppkt->rx_ctrl.rssi;
        // take channel byte and filter useful bits
        channel = ppkt->rx_ctrl.channel;
        channel &= 0xF0;
        // ***ToDo correct the timestamp***
        
        // i don't know why but last 4 bytes (of CRC i think) are different for any ESP receiver
        // calculating md5 packet digest
        mbedtls_md5((const unsigned char *) ppkt->payload, payloadSize, md5digest);
        
        // delete[] payloadHash;
        // ESP_LOGD(TAG, "****ssidLen %d %d %d %d %d %d", ppkt->payload[0], ppkt->payload[1], ppkt->payload[2], ppkt->payload[3], ppkt->payload[4], ppkt->payload[5]);
        memcpy(buf, &(ppkt->payload[26]), (size_t) ppkt->payload[25]);
        buf[ppkt->payload[25]] = '\0';
        ESP_LOGD(TAG, "%s", buf);
        string ssid(buf);
        // unique_ptr<ProbeReq::Builder> builderPtr(ProbeReq::Builder::create());
        ProbeReq::Builder *builderPtr = ProbeReq::Builder::create();
        ProbeReq probe = builderPtr->withType(WIFI_PKT_MGMT)
                .withSubtype(subtype)
                .withChannel(ppkt->rx_ctrl.channel)
                .withRssi(rssi)
                // .withSsid((char *) &(ppkt->payload[26]), ppkt->payload[25])
                .withSsid2(ssid)
                .withSourceAddress(hdr->addr2)
                .withDestAddress(hdr->addr1)
                .withBssid(hdr->addr3)
                .withMd5digest(md5digest)
                .withSequenceNumber(hdr->sequence_number)
                .withTimestamp(ppkt->rx_ctrl.timestamp)
                .build();

        delete builderPtr;
        // vRingbufferReturnItem(packetRingBuffer, probePacket);

        // ESP_LOGI(TAG, "packet len: %d ssidLen: %d", ppkt->rx_ctrl.sig_len, ipkt->ssid_prmtr.tag_len);
        // cout << probe;
        stringstream ss;
        ss << probe;
        string str = ss.str();
        char *abc = new char[str.length() + 1];
        strcpy(abc, str.c_str());
        ESP_LOGD(TAG, "%s", abc);
        // xSemaphoreTake(chanMutex, portMAX_DELAY);
        // xEventGroupSetBits(send_event_group, SEND_BIT);
        // esp_wifi_set_promiscuous_rx_cb(nullptr);
        // vTaskDelay(10); 
        // ESP_ERROR_CHECK(esp_wifi_set_channel(wifi_connection_chan, wifi_connection_second_chan));
        // ESP_LOGD(TAG, "d1");// esp_wifi_set_promiscuous(false);
        // uint8_t pchan; wifi_second_chan_t schan; 
        // esp_wifi_get_channel(&pchan, &schan);
        // ESP_LOGD(TAG, "channel send: %d schan: %d", pchan, schan);
        vTaskDelay(10);
        if (sendn(socket, abc, strlen(abc), 0) == -1) {
            ESP_LOGE(TAG, "Error sending sniffed packet info to server");
            close(socket);
            ESP_ERROR_CHECK(connect_to_server(&socket));
        }
        // ESP_LOGD(TAG, "d2");
        // xEventGroupSetBits(send_event_group, SEND_BIT);
        // esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
        // vTaskDelay(10);
        // ESP_ERROR_CHECK(esp_wifi_set_channel(FIXED_CHANNEL, WIFI_SECOND_CHAN_NONE));
        // ESP_LOGD(TAG, "d3");
        // esp_wifi_set_promiscuous(true);
        // xSemaphoreGive(chanMutex);
                
        // esp_wifi_get_channel(&pchan, &schan);
        // ESP_LOGD(TAG, "channel send: %d schan: %d", pchan, schan);       
        delete[] abc;
        vRingbufferReturnItem(packetRingBuffer, probePacket);
        // ESP_LOGD(TAG, "remaining size %d", xRingbufferGetCurFreeSize(packetRingBuffer));

        // builderPtr.reset();
        // probe.~ProbeReq();
        // ESP_ERROR_CHECK( heap_trace_stop() );
        // heap_trace_dump();
    }
}