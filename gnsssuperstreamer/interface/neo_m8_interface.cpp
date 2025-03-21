#include "neo_m8_interface.h"

#include <fstream>
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

    current_fix_.position = llh;
    current_fix_.position_error = posNed;
    current_fix_.num_svs = msg.num_svs;
    current_fix_.tow_ms = msg.itow;
    current_fix_.fix_type = msg.fix_type;
    current_fix_.estimated_horizontal_error = msg.horizontal_acc;
    current_fix_.estimated_vertical_error = msg.vertical_acc;

    logMessage("Received NavPvt message");
};

void NeoM8Interface::logMessage(const std::string& message) {
    using namespace std;
    std::ofstream logFile("/home/jared/jared/logs/neo_m8.log", std::ios_base::app);  // Open file in append mode

    string logmsg = "receiver: neo_m8, gpstow_ms: " + to_string(current_fix_.tow_ms) +
                    ", error_horizontal: " + to_string(current_fix_.position_error.horizontal) +
                    ", error_vertical: " + to_string(current_fix_.position_error.down) +
                    ", num_svs: " + to_string(current_fix_.num_svs) + ", fix_type: " + to_string(current_fix_.fix_type);
    if (logFile.is_open()) {
        logFile << logmsg << std::endl;
        logFile.close();  // Close the file
    } else {
        std::cerr << "Unable to open log file." << std::endl;
    }
}
}  // namespace ubx