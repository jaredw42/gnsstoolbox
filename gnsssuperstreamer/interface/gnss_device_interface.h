#pragma once
#include <vector>
#include <cstdint>
#include <variant>

#include "position.h"

class GnssDeviceInterface {
public:

class PvtFix {
    public: 
    PositionLLH position;
    NorthEastDown position_error;
    NorthEastDown velocity;
    NorthEastDown velocity_error;
    NorthEastDown position_covariance;
    NorthEastDown velocity_covariance;
    uint16_t estimated_horizontal_error;
    uint16_t estimated_vertical_error;
    uint32_t tow_ms; 
    uint8_t num_svs;
    uint8_t fix_type;


};

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