#pragma once
#include <cstdint>
#include <vector>

#include "gnss_device_interface.h"
#include "position.h"
#include "sbp.h"

class PiksiMultiInterface : public GnssDeviceInterface {
   public:
    PiksiMultiInterface(const PositionLLH& position);

   protected:
    void processDataImpl(uint16_t messageType, const std::vector<uint8_t>& payload) override;

   private:
    class HeartbeatData {
       public:
        bool system_error;
        bool io_error;
        bool swift_nap_error;
        uint8_t sbp_minor_version;
        uint8_t sbp_major_version;
        bool antenna_short;
        bool antenna_connected;

        uint32_t heartbeat_duration_ms;
        uint32_t pvt_tow_ms;
        uint32_t system_time_ms;
    };

    PositionLLH truth_position_;
    PositionLLH current_position_;
    PvtFix current_fix_;
    HeartbeatData heartbeat_data_;

    void processMsgPosLlh(const SbpMsgPosLlh& msg);
    void processMsgHeartbeat(const SbpMsgHeartbeat& msg);
};