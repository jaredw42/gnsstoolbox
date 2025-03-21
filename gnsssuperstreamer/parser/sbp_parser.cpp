#include "sbp_parser.h"

#include <iomanip>
#include <iostream>
#include <vector>

void SwiftBinaryMessageParser::feed(const char* data, std::size_t length) {
    buffer_.insert(buffer_.end(), data, data + length);
    parse_buffer();
}

uint16_t SwiftBinaryMessageParser::compute_crc(const std::vector<uint8_t>& buffer, std::size_t start_index,
                                               std::size_t length, uint16_t initial_crc) {
    uint16_t crc = initial_crc;

    for (std::size_t i = start_index; i < start_index + length; ++i) {
        crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ buffer[i]) & 0x00FF];
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
        uint16_t recv_crc = static_cast<uint8_t>(buffer_[6]) | (static_cast<uint8_t>(buffer_[7]) << 8);
        std::size_t totalMessageSize = kSbpHeaderSize + payloadLength + kSbpChecksumSize;
        if (buffer_.size() < totalMessageSize) return;

        // Debug output: print buffer contents and extracted fields
        // std::cerr << "Buffer contents: ";
        // for (size_t i = 0; i < totalMessageSize; ++i) {
        //     std::cerr << "0x" << std::hex << std::setw(2) << std::setfill('0') <<
        //     (int)static_cast<uint8_t>(buffer_[i]) << " ";
        // }
        // std::cerr << std::dec << std::endl;

        // std::cerr << "Extracted fields: msgType=" << msgType
        //           << ", sender=" << sender
        //           << ", payloadLength=" << static_cast<int>(payloadLength)
        //           << std::endl;

        uint16_t computed_crc = compute_crc(buffer_, 1, kSbpHeaderSize - 1 + payloadLength, 0xFFFF);

        if (computed_crc != recv_crc) {
            // std::cerr << "CRC error: expected ("
            //           << computed_crc << ") but got ("
            //           << recv_crc << "). Discarding message."
            //           << std::endl;
            // buffer_.erase(buffer_.begin(), buffer_.begin() + 1);
            // continue;
        } else {
            std::cerr << "CRC OK " << (int)msgType << std::endl;
        }

        std::vector<uint8_t> payload;
        if (payloadLength > 0) {
            payload.insert(payload.end(), buffer_.begin() + kSbpHeaderSize,
                           buffer_.begin() + kSbpHeaderSize + payloadLength);
        }

        handler_(msgType, payload);
        buffer_.erase(buffer_.begin(), buffer_.begin() + totalMessageSize);
    }
}