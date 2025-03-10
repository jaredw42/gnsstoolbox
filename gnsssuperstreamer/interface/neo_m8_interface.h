#pragma once
#include <cstdint>
#include <vector>

#include "gnss_device_interface.h"
#include "position.h"
#include "ubx.h"

namespace ubx {
    class NeoM8Interface : public GnssDeviceInterface {
    public:
        NeoM8Interface(const PositionLLH& position);

    protected:
        void processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) override;

    private:
        PositionLLH truth_position_;

        void parseNavPvtMsg(const UbxNavPvtMessage& msg);
    };
}