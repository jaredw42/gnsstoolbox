#pragma once
#include <vector>
#include <cstdint>
#include <variant>

class GnssDeviceInterface {
public:
    virtual ~GnssDeviceInterface() = default;

    using MessageType = std::variant<uint8_t, uint16_t>;

    void processData(MessageType messageType, const std::vector<uint8_t>& payload) {
        std::visit([this, &payload](auto&& arg) {
            processDataImpl(static_cast<uint16_t>(arg), payload);
        }, messageType);
    }

protected:
    virtual void processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) = 0;
};