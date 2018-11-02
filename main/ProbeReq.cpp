#include "ProbeReq.h"

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

	os << endl;

    return os;
}
