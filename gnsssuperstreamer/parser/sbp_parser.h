#pragma once
#include <cstdint>
#include <functional>
#include <vector>

#include "binary_message_parser.h"
#include "sbp.h"

class SwiftBinaryMessageParser {
   public:
    using HandlerType = std::function<void(uint16_t, const std::vector<uint8_t>&)>;

    SwiftBinaryMessageParser(HandlerType handler) : handler_(handler) {}

    void feed(const char* data, std::size_t length);

   private:
    void parse_buffer();
    uint16_t compute_crc(const std::vector<char>& buf, std::size_t start, std::size_t end);

    std::vector<char> buffer_;
    HandlerType handler_;
};