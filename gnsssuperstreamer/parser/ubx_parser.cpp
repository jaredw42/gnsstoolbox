#include "ubx_parser.h"
#include <iostream>

namespace ubx {

void UbloxParser::feed(const char* data, std::size_t length) {
    buffer_.insert(buffer_.end(), data, data + length);
    parse_buffer();
}

void UbloxParser::compute_checksum(const std::vector<uint8_t>& buf, std::size_t start, std::size_t end, uint8_t& ck_a, uint8_t& ck_b) {
    ck_a = 0;
    ck_b = 0;
    for (std::size_t i = start; i < end; ++i) {
        ck_a = static_cast<uint8_t>(ck_a + buf[i]);
        ck_b = static_cast<uint8_t>(ck_b + ck_a);
    }
}

void UbloxParser::parse_buffer() {
    while (true) {
        if (buffer_.size() < 2)
            return;

        if (buffer_[0] != 0xB5) {
            buffer_.erase(buffer_.begin());
            continue;
        }
        if (buffer_.size() >= 2 && buffer_[1] != 0x62) {
            buffer_.erase(buffer_.begin());
            continue;
        }

        if (buffer_.size() < kUbxHeaderSize)
            return;

        uint8_t msgClass = buffer_[2];
        uint8_t msgId    = buffer_[3];
        uint16_t payloadLength = buffer_[4] | (buffer_[5] << 8);

        std::size_t totalMessageSize = kUbxHeaderSize + payloadLength + kUbxChecksumSize;
        if (buffer_.size() < totalMessageSize)
            return;

        uint8_t computed_ck_a = 0;
        uint8_t computed_ck_b = 0;
        compute_checksum(buffer_, 2, kUbxHeaderSize + payloadLength, computed_ck_a, computed_ck_b);

        uint8_t recv_ck_a = buffer_[kUbxHeaderSize + payloadLength];
        uint8_t recv_ck_b = buffer_[kUbxHeaderSize + payloadLength + 1];

        if (computed_ck_a != recv_ck_a || computed_ck_b != recv_ck_b) {
            std::cerr << "Checksum error: expected (" 
                      << static_cast<int>(computed_ck_a) << ", " 
                      << static_cast<int>(computed_ck_b) << ") but got ("
                      << static_cast<int>(recv_ck_a) << ", " 
                      << static_cast<int>(recv_ck_b) << "). Discarding sync byte." 
                      << std::endl;
            buffer_.erase(buffer_.begin());
            continue;
        }

        std::vector<uint8_t> payload;
        if (payloadLength > 0) {
            payload.insert(payload.end(),
                           buffer_.begin() + kUbxHeaderSize,
                           buffer_.begin() + kUbxHeaderSize + payloadLength);
        }

        // Combine msgClass and msgId into a single uint16_t
        uint16_t combinedType = (static_cast<uint16_t>(msgClass) << 8) | msgId;
        handler_(combinedType, payload);
        buffer_.erase(buffer_.begin(), buffer_.begin() + totalMessageSize);
    }
}

}  // namespace ubx