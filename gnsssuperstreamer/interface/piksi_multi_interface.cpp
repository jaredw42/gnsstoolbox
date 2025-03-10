#include "piksi_multi_interface.h"
#include "geodetic2ned.h"

#include <iostream>

PiksiMultiInterface::PiksiMultiInterface(const PositionLLH& position) : truth_position_(position) {}

void PiksiMultiInterface::processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) {
    // std::cout << "Processing data in PiksiMultiInterface with messageType: " << messageType << "\n";

    switch (messageType) {
        case SbpMsgType::MsgPosLlh: {
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

    current_position_.latitude = msg.latitude;
    current_position_.longitude = msg.longitude;
    current_position_.ellipsoidHeight = msg.height;
    const auto posNed = LLHToNED(current_position_, truth_position_);

    current_fix_.position = current_position_;
    current_fix_.position_error = posNed;
    current_fix_.num_svs = msg.n_sats;
    current_fix_.tow_ms = msg.tow;
    current_fix_.fix_type = msg.flags.gnss_fix;
    current_fix_.estimated_horizontal_error = msg.horizontal_accuracy;
    current_fix_.estimated_vertical_error = msg.vertical_accuracy;

    std::cout << "Piksi time: " << msg.tow * 1e-3 
    << " err 2d: " << posNed.horizontal 
    << " est err 2d: " << current_fix_.estimated_horizontal_error
    << " svs used: " << static_cast<int>(msg.n_sats) 
    << std::endl;
}