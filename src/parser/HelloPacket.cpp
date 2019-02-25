//
// Created by rewbycraft on 2/25/19.
//

#include "HelloPacket.h"
#include "internal.h"
#include "MalformedPacketException.h"

parser::HelloPacket::HelloPacket() : Packet() {

}

parser::HelloPacket::HelloPacket(const parser::bytevector &data) : Packet(data) {
	const parser::bytevector rawNeighbors = parser::deserializeObject(header, data);
	if (rawNeighbors.size() % 4 != 0)
		throw parser::MalformedPacketException("Invalid neighbours.");

	for (unsigned long i = 0; i < rawNeighbors.size(); i += 4) {
		uint32_t neighbor = (rawNeighbors[i] << 24) + (rawNeighbors[i+1] << 16) + (rawNeighbors[i+2] << 8) + rawNeighbors[i+3];
		neighbors.push_back(neighbor);
	}
}

const parser::bytevector parser::HelloPacket::serialize() const {
	parser::bytevector result;
	serializeObject(result, header);
	
	for (const uint32_t& neighbor : neighbors)
	{
		result.push_back(static_cast<uint8_t>((neighbor >> 24) & 0xFF));
		result.push_back(static_cast<uint8_t>((neighbor >> 16) & 0xFF));
		result.push_back(static_cast<uint8_t>((neighbor >> 8) & 0xFF));
		result.push_back(static_cast<uint8_t>(neighbor & 0xFF));
	}
	
	return result;
}

const std::vector<uint32_t> &parser::HelloPacket::getNeighbors() const {
	return neighbors;
}

void parser::HelloPacket::setNeighbors(const std::vector<uint32_t> &neighbors) {
	HelloPacket::neighbors = neighbors;
}

const parser::HelloPacket::Header &parser::HelloPacket::getHeader() const {
	return header;
}

void parser::HelloPacket::setHeader(const parser::HelloPacket::Header &header) {
	HelloPacket::header = header;
}
