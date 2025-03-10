#pragma once

#include <array>
#include <boost/endian/arithmetic.hpp>
namespace ubx {
static constexpr uint8_t kUbxHeaderSize = 6;
static constexpr uint8_t kUbxChecksumSize = 2;

class UbxMsgClass {
    public:
    static constexpr  uint8_t Nav = 0x01;
};

struct UbxNavPvtMessage {
    uint32_t itow;
    uint16_t year; 
    uint8_t month;
    uint8_t day;
    uint8_t hour; 
    uint8_t min;
    uint8_t sec;
    uint8_t valid;
    uint32_t time_accuracy;
    int32_t nano;
    uint8_t fix_type; 

    union Flags {
        struct {
            uint8_t gnss_fix_ok : 1;
            uint8_t diff_soln : 1 ; 
            uint8_t reserved : 3 ;
            uint8_t head_veh_valid : 1;
            uint8_t carr_soln: 2;

        };
        uint8_t word; 
    };
    Flags flags ;

    union Flags2{
        struct {
            uint8_t reserved : 5;
            uint8_t confirmed_avai : 1;
            uint8_t confirmed_date : 1; 
            uint8_t confirmed_time : 1;
        };
        uint8_t word; 
    };
    Flags2 flags2; 
    uint8_t num_svs;
    boost::endian::little_int32_t lon;
    boost::endian::little_int32_t lat; 
    boost::endian::little_int32_t height; 
    int32_t height_msl; 
    uint32_t horizontal_acc;
    uint32_t vertical_acc;
    int32_t velocity_n;
    int32_t velocity_e;
    int32_t velocity_d; 
    int32_t ground_speed;
    int32_t heading_motion; 
    uint32_t speed_accuracy;
    uint32_t heading_accuracy; 
    uint32_t pdop;
    std::array<uint8_t, 6> reserved{};
    int32_t heading_vehicle; 
    int16_t magnetic_declination; 
    uint32_t magnetic_declination_acc; 

}__attribute__((packed));
} // namespace ubx