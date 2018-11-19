#include "ProbeReq.h"

using namespace std;

ProbeReq::Builder* ProbeReq::Builder::create() {
	return new Builder();
}

ProbeReq::Builder& ProbeReq::Builder::withType(wifi_promiscuous_pkt_type_t type) {
	this->type = type;
	return *this;	
}

ProbeReq::Builder& ProbeReq::Builder::withSubtype(uint8_t subtype) {
	this->subtype = subtype;
	// cout << this << endl;
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withChannel(uint8_t channel) {
	this->channel = channel;
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withRssi(int8_t rssi) {
	this->rssi = rssi;
    cout << this << endl;
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withSsid(const char *ssid, uint8_t ssidLen) {
	this->ssid = move(ssid);
	this->ssidLen = ssidLen;
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withDestAddress(const uint8_t destAddress[6]) {
	// this->destAddress = destAddress;
    copy(destAddress, destAddress+6, begin(this->destAddress));
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withSourceAddress(const uint8_t sourceAddress[6]) {
	copy(sourceAddress, sourceAddress+6, begin(this->sourceAddress));
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withBssid(const uint8_t bssid[6]) {
    copy(bssid, bssid+6, begin(this->bssid));
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withMd5digest(const unsigned char md5digest[16]) {
    copy(md5digest, md5digest+16, begin(this->md5digest));
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withTimestamp(uint32_t timestamp) {
	this->timestamp = timestamp;
	return *this;
}

ProbeReq::Builder& ProbeReq::Builder::withSequenceNumber(uint16_t sequence_number) {
	this->sequence_number = sequence_number;
	return *this;
}

ProbeReq ProbeReq::Builder::build() {
	return ProbeReq(type, subtype, channel, rssi, ssid, ssidLen,
			move(destAddress), move(sourceAddress), move(bssid),
			sequence_number, move(md5digest), timestamp);
}



std::ostream& operator<<(ostream& os, const ProbeReq& probeReq){
    os << "SUBTYPE=" << packetSubtype2Str(probeReq.subtype) << ", ";
	os << "SSID=" << probeReq.ssid << ", ";
	os << "CHAN=" << setfill('0') << setw(2) << dec
		<< static_cast<unsigned int>(probeReq.channel) << ", ";
	os << "RSSI=" << setfill('0') << setw(2) << dec << static_cast<int>(probeReq.rssi);
	os << ", SOURCE=";
	for (auto iter = probeReq.sourceAddress.begin(); iter != probeReq.sourceAddress.end(); iter++) {
		const unsigned char& byte = *iter;
		os << setfill('0') << setw(2) << hex <<  static_cast<unsigned int>(byte);
		if (iter != probeReq.sourceAddress.end() - 1)
			os << ":";
	}
	os << ", DEST=";
	for (auto iter = probeReq.destAddress.begin(); iter != probeReq.destAddress.end(); iter++) {
		const unsigned char& byte = *iter;
		os << setfill('0') << setw(2) << hex << static_cast<unsigned int>(byte);
		if (iter != probeReq.destAddress.end() - 1)
			os << ":";
	}
	os << ", BSSID=";
	for (auto iter = probeReq.bssid.begin(); iter != probeReq.bssid.end(); iter++) {
		const unsigned char& byte = *iter;
		os << setfill('0') << setw(2) << hex << static_cast<unsigned int>(byte);
		if (iter != probeReq.bssid.end() - 1)
			os << ":";
	}
	os << ", timestamp=" << dec << probeReq.timestamp;
	os << ", seq num: " << dec << probeReq.sequence_number;
	os << ", md5hash: ";
	for (int i = 0; i < 16; i++) {
		os << setfill('0') << setw(2) << hex << static_cast<unsigned int>(probeReq.md5digest[i]);
    }
	os << endl;

    return os;
}
