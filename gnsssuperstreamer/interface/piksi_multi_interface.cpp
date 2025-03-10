#include "piksi_multi_interface.h"
#include "geodetic2ned.h"
#include "sbp.h"
#include <iostream>

PiksiMultiInterface::PiksiMultiInterface(const PositionLLH& position) : position_(position) {}

void PiksiMultiInterface::processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) {
    // std::cout << "Processing data in PiksiMultiInterface with messageType: " << messageType << "\n";

    switch (messageType) {
        case 0x20A: {
        const auto& msg = *reinterpret_cast<const SbpMsgPosLlh*>(payload.data());
            processMsgPosLlh(msg);
            break;
        }
        default:
            // std::cerr << "Unhandled message type: " << messageType << std::endl;
            break;      
    }

}

void PiksiMultiInterface::processMsgPosLlh(const SbpMsgPosLlh& msg) {
    PositionLLH llh;
    llh.latitude = msg.latitude;
    llh.longitude = msg.longitude;
    llh.ellipsoidHeight = msg.height;
    const auto posNed = LLHToNED(llh, position_);
    std::cout << "Piksi time: " << msg.tow * 1e-3 
              << " err north: " << posNed.north 
              << " east: " << posNed.east 
              << " svs used: " << static_cast<int>(msg.n_sats) 
              << std::endl;
}