#ifndef PDSPROJECT_PROBEREQ_H
#define PDSPROJECT_PROBEREQ_H

#include "esp_wifi_types.h"
#include "80211Packet.h"
#include <string>
#include <array>
#include <memory>
#include <iostream>
#include <iomanip>

using namespace std;

class ProbeReq {

public:

	explicit ProbeReq(wifi_promiscuous_pkt_type_t type, uint8_t subtype,
		uint8_t channel, int8_t rssi, string ssid, uint8_t ssidLen,
		array<uint8_t,6>& destAddress, array<uint8_t,6>& sourceAddress,
		array<uint8_t,6>& bssid):
		type(type), subtype(subtype), channel(channel), rssi(rssi),
		ssid(ssid), ssidLen(ssidLen), destAddress(destAddress),
		sourceAddress(sourceAddress), bssid(bssid) {};

	void setType(wifi_promiscuous_pkt_type_t type){ this->type = type; }

	wifi_promiscuous_pkt_type_t getType() {	return type; }

	void setSubtype(uint8_t subtype) { this->subtype = subtype; }

	uint8_t getSubtype(uint8_t subtype) { return subtype; }

	void setChannel(uint8_t channel) { this->channel = channel; }

	uint8_t getChannel() {return this->channel; }

	void setRssi(int8_t rssi) { this->rssi = rssi; }

	int8_t getRssi() {return this->rssi; }

	void setDestAddress(array<uint8_t,6>& destAddress) { this->destAddress = destAddress; }

	array<uint8_t,6> getDestAddress() { return destAddress; }

	void setSourceAddress(array<uint8_t,6>& sourceAddress) { this->sourceAddress = sourceAddress; }

	array<uint8_t,6> getSourceAddress() { return sourceAddress; }

	void setBssid(array<uint8_t,6>& bssid) { this->bssid = bssid; }

	array<uint8_t,6> getBssid() { return bssid; }

	void setSsidLen(uint8_t ssidLen) { this->ssidLen = ssidLen; }

	uint8_t getSsidLen() { return ssidLen; }

	void setSsid(string& ssid) { this->ssid = ssid; }

	string getSsid() { return this->ssid; }

	string to_string();

private:
	wifi_promiscuous_pkt_type_t type;
	uint8_t subtype;
	uint8_t channel;
	int8_t rssi;
	string ssid;
	uint8_t ssidLen;
	array <uint8_t,6> destAddress; /* receiver address */
	array <uint8_t,6> sourceAddress; /* sender address */
	array <uint8_t,6> bssid; /* filtering address */

	friend std::ostream& operator<<(ostream& os, const ProbeReq& probeReq);
};

#endif //PDSPROJECT_PROBEREQ_H
