#pragma once
#include "binary_message_parser.h"
#include "ubx.h"
#include <vector>
#include <functional>
#include <cstdint>

namespace ubx {

class UbloxParser : public BinaryMessageParser {
public:
    using HandlerType = std::function<void(const std::variant<uint8_t, uint16_t>&, const std::vector<uint8_t>&)>;

    UbloxParser(HandlerType handler) : BinaryMessageParser(handler) {}

    void feed(const char* data, std::size_t length) override;
    void parse_buffer() override;
    void compute_checksum(const std::vector<uint8_t>& buf, std::size_t start, std::size_t end,
                          uint8_t& ck_a, uint8_t& ck_b);

private:
    static constexpr std::size_t kUbxHeaderSize = 6;
    static constexpr std::size_t kUbxChecksumSize = 2;
};

}  // namespace ubx