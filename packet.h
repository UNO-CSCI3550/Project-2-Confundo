#ifndef PACKET_H
#define PACKET_H

//Packet Header
#include <string>
#include <cstdlib>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <climits>
#include <unistd.h>
#include <cstring>

// Header size, Payload size and Packet size
const int HEADER_SIZE = 12;
const int MAX_PAYLOAD_SIZE = 512;
const int MAX_PACKET_SIZE = HEADER_SIZE + MAX_PAYLOAD_SIZE;
//flags
const uint16_t ACK = 4;
const uint16_t SYN = 2;
const uint16_t FIN = 1;

typedef struct {
	uint32_t seq_num, ack_num;
	uint16_t conn_id, flags;
	uint8_t payload[MAX_PAYLOAD_SIZE] = {};
	int size = HEADER_SIZE;
} PacketArgs;
//Constructors to create the packet from the buffer
class Packet {
public:
	Packet(const uint8_t buffer[MAX_PACKET_SIZE], int packet_size);
	Packet(const PacketArgs& args);

    uint32_t seq_num, ack_num;
    uint16_t conn_id, flags;
    uint8_t payload[MAX_PAYLOAD_SIZE];
    
    int size() const {return size_;}
    int payload_size() const {return payload_size_;}
    void to_uint32_string(uint8_t (&buf)[MAX_PACKET_SIZE]) const;
    bool is_valid() const {return valid_;}

private:
	bool valid_ = true;
	int size_ = 0;
    int payload_size_ = 0;

};
#endif