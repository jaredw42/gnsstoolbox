#include "sbp_parser.h"

#include <iostream>
#include <vector>

void SwiftBinaryMessageParser::feed(const char* data, std::size_t length) {
    buffer_.insert(buffer_.end(), data, data + length);
    parse_buffer();
}

uint16_t SwiftBinaryMessageParser::compute_crc(const std::vector<char>& buf, std::size_t start, std::size_t end) {
    uint16_t crc = 0xFFFF;
    for (std::size_t i = start; i < end; ++i) {
        crc ^= static_cast<uint8_t>(buf[i]);
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0x8408;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void SwiftBinaryMessageParser::parse_buffer() {
    while (true) {
        if (buffer_.size() < 1) return;

        if (static_cast<uint8_t>(buffer_[0]) != 0x55) {
            buffer_.erase(buffer_.begin());
            continue;
        }

        if (buffer_.size() < kSbpHeaderSize) return;

        uint16_t msgType = static_cast<uint8_t>(buffer_[1]) | (static_cast<uint8_t>(buffer_[2]) << 8);
        uint16_t sender = static_cast<uint8_t>(buffer_[3]) | (static_cast<uint8_t>(buffer_[4]) << 8);
        uint8_t payloadLength = static_cast<uint8_t>(buffer_[5]);

        std::size_t totalMessageSize = kSbpHeaderSize + payloadLength + kSbpChecksumSize;
        if (buffer_.size() < totalMessageSize) return;

        // std::cout<< "bufLen: "<< buffer_.size()<< " msg_type: " << msgType << " sender: "<<
        // sender << " payLen: " << static_cast<int>(payloadLength) << std::endl;
        uint16_t computed_crc = compute_crc(buffer_, 1, kSbpHeaderSize + payloadLength);
        uint16_t recv_crc = static_cast<uint8_t>(buffer_[kSbpHeaderSize + payloadLength]) |
                            (static_cast<uint8_t>(buffer_[kSbpHeaderSize + payloadLength + 1]) << 8);
        if (computed_crc != recv_crc) {
            // std::cerr << "CRC error: expected ("
            //           << computed_crc << ") but got ("
            //           << recv_crc << "). Discarding message."
            //           << std::endl;
            // buffer_.erase(buffer_.begin(), buffer_.begin() + 1);
            // continue;
        }

        std::vector<uint8_t> payload;
        if (payloadLength > 0) {
            payload.insert(payload.end(), buffer_.begin() + kSbpHeaderSize,
                           buffer_.begin() + kSbpHeaderSize + payloadLength);
        }

        // std::cout << "Parsed message: msgType=" << msgType
        //           << ", sender=" << sender
        //           << ", payloadLength=" << static_cast<int>(payloadLength)
        //           << ", computed_crc=" << computed_crc
        //           << ", recv_crc=" << recv_crc << std::endl;

        handler_(msgType, payload);
        buffer_.erase(buffer_.begin(), buffer_.begin() + totalMessageSize);
    }
}