#pragma once
#include "gnss_device_interface.h"
#include "position.h"
#include <vector>
#include <cstdint>

class PiksiMultiInterface : public GnssDeviceInterface {
public:
    PiksiMultiInterface(const PositionLLH& position);

protected:
    void processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) override;

private:
    PositionLLH position_;
    void processMsgPosLlh(const SbpMsgPosLlh& msg);
};