//Creating packet format
#include "packet.h"
#include <iostream>
//create the packet from the buffer
//set the sequence and acknowledgment number based on the number of bytes
Packet::Packet(const uint8_t buffer[MAX_PACKET_SIZE], int packet_size) : size_(packet_size) {
	if (size_ < HEADER_SIZE) valid_ = false;
	else {
		payload_size_ = size_ - HEADER_SIZE;

		memcpy(&seq_num, buffer, 4);
		memcpy(&ack_num, buffer+4, 4);
		seq_num = ntohl(seq_num);
		ack_num = ntohl(ack_num);

		uint32_t id_flags;
		memcpy(&id_flags, buffer+8, 4);
		id_flags = ntohl(id_flags);
		conn_id = id_flags >> 16;
		flags = id_flags;

		memcpy(&payload, buffer+12, payload_size_);
	}
}
//Parse the packet
Packet::Packet(const PacketArgs& args) : size_(args.size) {
	if (size_ < HEADER_SIZE) valid_ = false;
	else {
		payload_size_ = size_ - HEADER_SIZE;

		seq_num = args.seq_num;
		ack_num = args.ack_num;
		conn_id = args.conn_id;
		flags = args.flags;
		memcpy(&payload, &args.payload, payload_size_);
	}
}

void Packet::to_uint32_string(uint8_t (&buf)[MAX_PACKET_SIZE]) const {
	uint32_t val = htonl(seq_num);
	// std::cout << seq_num << " " << val << "\n";
	memcpy(buf, &val, 4);

	val = htonl(ack_num);
	memcpy(buf+4, &val, 4);

	val = htonl(((uint32_t)conn_id << 16) + flags);
	memcpy(buf+8, &val, 4);

	memcpy(buf+12, payload, payload_size_);
}
