#include "ProbeReq.h"
#include <sstream>  

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

std::string ProbeReq::toString() {
	std::string resp{"{"};
	resp += "SUBTYPE: ";
	resp += packetSubtype2Str(subtype);
	resp += ", SSID: ";
	resp += ssid;
	resp += ", CHAN: ";
	std::stringstream sstm1;
	sstm1 << static_cast<unsigned int>(channel);
	resp += sstm1.str();
	resp += ", RSSI: ";
	std::stringstream sstm2;
	sstm2 << static_cast<int>(rssi);
	resp += sstm1.str();
	resp += ", SOURCE: ";
	for (auto iter = sourceAddress.begin(); iter != sourceAddress.end(); iter++) {
		/*const unsigned char& byte = *iter;
		os << setfill('0') << setw(2) << hex << static_cast<unsigned int>(byte);
		if (iter != probeReq.sourceAddress.end() - 1)
			os << ":";*/
		resp += "*";  //<------ SISTEMARE
	}

	resp += ", DEST: ";
	for (auto iter = destAddress.begin(); iter != destAddress.end(); iter++) {
		/*const unsigned char& byte = *iter;
		os << setfill('0') << setw(2) << hex << static_cast<unsigned int>(byte);
		if (iter != probeReq.destAddress.end() - 1)
			os << ":";*/
		resp += "*";  //<------ SISTEMARE
	}

	resp += ", BSSID: ";
	for (auto iter = bssid.begin(); iter != bssid.end(); iter++) {
		/*const unsigned char& byte = *iter;
		os << setfill('0') << setw(2) << hex << static_cast<unsigned int>(byte);
		if (iter != probeReq.bssid.end() - 1)
			os << ":";*/
		resp += "*";
	}

	resp += "}";

	return resp;
}
