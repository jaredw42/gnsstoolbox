#include "piksi_multi_interface.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "geodetic2ned.h"

PiksiMultiInterface::PiksiMultiInterface(const PositionLLH& position) : truth_position_(position) {}

void PiksiMultiInterface::processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) {
    // std::cout << "Processing data in PiksiMultiInterface with messageType: " << messageType <<
    // "\n";

    switch (messageType) {
        case SbpMsgType::MsgPosLlh: {
            // if (sizeof(payload) < sizeof(SbpMsgPosLlh)) {
            //     std::cerr << "Payload size is too small for message type: " << messageType << std::endl;
            //     std::cerr << "Payload size: " << sizeof(payload.data()) <<  " posllh size: " << sizeof(SbpMsgPosLlh)
            //     <<std::endl; return;
            // }
            const auto& msg = *reinterpret_cast<const SbpMsgPosLlh*>(payload.data());
            processMsgPosLlh(msg);
            break;
        }
        case SbpMsgType::MsgHeartbeat: {
            if (sizeof(payload) < sizeof(SbpMsgHeartbeat)) {
                std::cerr << "Payload size is too small for message type: " << messageType << std::endl;
                return;
            }
            const auto& msg = *reinterpret_cast<const SbpMsgHeartbeat*>(payload.data());
            processMsgHeartbeat(msg);
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

    // std::cout << "Piksi time: " << msg.tow * 1e-3
    // << " err 2d: " << posNed.horizontal
    // << " est err 2d: " << current_fix_.estimated_horizontal_error
    // << " svs used: " << static_cast<int>(msg.n_sats)
    // << std::endl;
}

void PiksiMultiInterface::processMsgHeartbeat(const SbpMsgHeartbeat& msg) {
    using namespace std::chrono;

    const auto now = std::chrono::system_clock::now();
    const auto now_ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
    // std::cout << "Piksi heartbeat: " << msg.flags.word << std::endl;
    heartbeat_data_.heartbeat_duration_ms = now_ms - heartbeat_data_.system_time_ms;
    heartbeat_data_.system_time_ms = now_ms;
    heartbeat_data_.pvt_tow_ms = current_fix_.tow_ms;
    heartbeat_data_.system_error = msg.flags.system_error;
    heartbeat_data_.io_error = msg.flags.io_error;
    heartbeat_data_.swift_nap_error = msg.flags.swift_nap_error;
    heartbeat_data_.sbp_minor_version = msg.flags.sbp_minor_version;
    heartbeat_data_.sbp_major_version = msg.flags.sbp_major_version;
    heartbeat_data_.antenna_short = msg.flags.antenna_short;
    heartbeat_data_.antenna_connected = msg.flags.antenna_connected;

    std::cout << "Starling PVT time: " << current_fix_.tow_ms * 1e-3
              << " error 2D: " << current_fix_.position_error.horizontal
              << " est error 2D: " << current_fix_.estimated_horizontal_error * 1e-3
              << " svs used_for_nav: " << static_cast<int>(current_fix_.num_svs)
              << " heartbeat duration: " << heartbeat_data_.heartbeat_duration_ms
              << " system error: " << heartbeat_data_.system_error << " io_error: "
              << heartbeat_data_.io_error
              // << " antenna_short: " << heartbeat_data_.antenna_short
              << " antenna_connected: " << heartbeat_data_.antenna_connected << std::endl;
    logMessage("pen15");
}

void PiksiMultiInterface::logMessage(const std::string& message) {
    using namespace std;
    std::ofstream logFile("/home/jared/jared/logs/piksi_multi.log", std::ios_base::app);  // Open file in append mode
    string logmsg = "receiver: piksi_multi, gpstow_ms: " + to_string(current_fix_.tow_ms) + ", " +
                    "error_horiz: " + to_string(current_fix_.position_error.horizontal) + ", " +
                    "est_error_horiz: " + to_string(current_fix_.estimated_horizontal_error) + ", " +
                    "svs_used: " + to_string(current_fix_.num_svs) + ", " +
                    "heartbeat_duration: " + to_string(heartbeat_data_.heartbeat_duration_ms) + ", " +
                    "system_error: " + to_string(heartbeat_data_.system_error) + ", " +
                    "io_error: " + to_string(heartbeat_data_.io_error) + ", " +
                    "antenna_connected: " + to_string(heartbeat_data_.antenna_connected);
    if (logFile.is_open()) {
        logFile << logmsg << std::endl;
        logFile.close();  // Close the file
    } else {
        std::cerr << "Unable to open log file." << std::endl;
    }
}