#include "neo_m8_interface.h"

#include <iostream>
#include <typeinfo>

#include "geodetic2ned.h"

namespace ubx {
NeoM8Interface::NeoM8Interface(const PositionLLH& position) : truth_position_(position) {}

void NeoM8Interface::processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) {
    uint8_t msgClass = static_cast<uint8_t>(messageType >> 8);
    uint8_t msgId = static_cast<uint8_t>(messageType & 0xFF);

    // std::cout << "Processing data in NeoM8Interface with msgClass: " <<
    // static_cast<int>(msgClass)
    //           << ", msgId: " << static_cast<int>(msgId) << "\n";
    switch (msgClass) {
        case UbxMsgClass::Nav:
            switch (msgId) {
                case 0x07:
                    const auto msg = *reinterpret_cast<const UbxNavPvtMessage*>(payload.data());
                    parseNavPvtMsg(msg);
            }
            break;
        default:
            break;
            // do nothing
    };
}

void NeoM8Interface::parseNavPvtMsg(const UbxNavPvtMessage& msg) {
    PositionLLH llh;
    llh.latitude = msg.lat * 1e-7;
    llh.longitude = msg.lon * 1e-7;
    llh.ellipsoidHeight = msg.height * 1e-3;
    const auto posNed = LLHToNED(llh, truth_position_);
    std::cout << "Ublox time: " << msg.itow * 1e-3 << " err 2d: " << posNed.horizontal
              << " est err 2d: " << msg.horizontal_acc * 1e-3 << " svs used: " << static_cast<int>(msg.num_svs)
              << std::endl;
};
}  // namespace ubx