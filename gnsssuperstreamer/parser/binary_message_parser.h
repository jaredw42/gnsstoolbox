#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <variant>

class BinaryMessageParser {
public:
    using HandlerType = std::function<void(const std::variant<uint8_t, uint16_t>&, const std::vector<uint8_t>&)>;

    BinaryMessageParser(HandlerType handler) : handler_(handler) {}
    virtual ~BinaryMessageParser() = default; // Ensure virtual destructor

    virtual void feed(const char* data, std::size_t length) = 0;
    virtual void parse_buffer() = 0;

protected:
    HandlerType handler_;
    std::vector<uint8_t> buffer_;
};