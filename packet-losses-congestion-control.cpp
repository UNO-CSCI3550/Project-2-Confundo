// Creating a structure for a connection
struct Connection {

	FILE* fd;
	uint16_t id;
	uint32_t client_seq_num;
	uint32_t server_seq_num;
	uint32_t cwnd;
	uint32_t ssthresh;
};c

// Handling Slow Start and Congestion Avoidance
void congestion_mode() {
	if (c.cwnd < c.ssthresh) {
		// slow start
		c.cwnd += INIT_CWND;
	}
	else {
		// congestion avoidance
		c.cwnd += INIT_CWND * INIT_CWND / c.cwnd;
	}
	if (c.cwnd > MAX_CWND) {
		c.cwnd = MAX_CWND;
	}
}

// sending the file and pushing the packets - handling packet losses
	while(1) {
		uint8_t read_buff[MAX_PAYLOAD_SIZE];

		c_win_size = 0;

		// utilize the entire congestion window
		while(c_win_size < c.cwnd) {
			int read_bytes = fread(read_buff, sizeof(char), MAX_PAYLOAD_SIZE, c.fd);
			if (read_bytes < 0) {
				report_error("reading payload file", true, 1);
			}
			if (read_bytes == 0) {
				sending_data = false;
				break; // No payload to send
			}
			if (read_bytes > 0) {
				args.seq_num = c.client_seq_num;
				args.ack_num = 0;
				args.flags = 0;
				memcpy(&args.payload, read_buff, read_bytes);
				args.size = HEADER_SIZE + read_bytes;
				p = Packet(args); // see packet.cpp and packet.h
				c_window.push(p); // pushing those packets

				c_win_size += read_bytes;
				p.to_uint32_string(send_pack);
				print_output(sent, p);
				if (sendto(sockfd, send_pack, p.size(), 0, (struct sockaddr *) &(*servaddr), sizeof(*servaddr)) < 0) {
					report_error("sending payload packet to server", true, 1);
				}
				c.client_seq_num += read_bytes;
				c.client_seq_num %= (MAX_NUM+1);
			}
		}

		// Wait for ACK
		while(!c_window.empty()) {

			do { // Check for ACK received, retransmit
				recv_bytes = recvfrom(sockfd, recv_buff, MAX_PACKET_SIZE, 0, (struct sockaddr*) &(*servaddr), &len);
				if (recv_bytes > 0) {
					gettimeofday(&receive_time, NULL);
					break;
				}
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
				 	// std::cerr << "DEBUG: ACK timeout" << std::endl;
				 	retransmit(p);
				}
			} while(errno == EAGAIN || errno == EWOULDBLOCK);
			p = Packet(recv_buff, recv_bytes);

			if (check_header_data(p, c_window.front(), ACK) == 1) {
				// report_error("receiving ACK from server", true, 1);
				c_window.pop();
				print_output(rcvd, p);
				congestion_mode(); // figure out the congestion control mode
			}
			else {
				// std::cerr << "DEBUG: header incorrect" << std::endl;
				retransmit(c_window.front());
			}
			c.server_seq_num = p.seq_num;
			c.client_seq_num %= (MAX_NUM+1);
		}

		if (!sending_data) {break; }
	}
