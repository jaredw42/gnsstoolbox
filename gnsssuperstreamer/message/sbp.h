#pragma once
#include <cstdint>
#include <vector>

static constexpr uint8_t kSbpHeaderSize = 6;
static constexpr uint8_t kSbpChecksumSize = 2;

struct SbpMsgType {
    static constexpr uint16_t MsgPosLlh = 0x020A;
    static constexpr uint16_t MsgHeartbeat = 0xFFFF;
};

struct SbpMessage {
    uint16_t msgType;
    uint16_t sender;
    uint8_t length;
    std::vector<uint8_t> payload;
    uint16_t crc;
} __attribute__((packed));

struct SbpMsgPosLlh {
    uint32_t tow;
    double latitude;
    double longitude;
    double height;
    uint16_t horizontal_accuracy;
    uint16_t vertical_accuracy;
    uint8_t n_sats;

    union Flags {
        struct {
            uint8_t gnss_fix : 2;
            uint8_t inertial_mode : 2;
            uint8_t tow_type : 1;
            uint8_t reserved : 2;
        };
        uint8_t word;
    };
    Flags flags;
} __attribute__((packed));

struct SbpMsgHeartbeat {
    union Flags {
        struct {
            uint32_t system_error : 1;
            uint32_t io_error : 1;
            uint32_t swift_nap_error : 1;
            uint32_t reserved : 5;
            uint32_t sbp_minor_version : 8;
            uint32_t sbp_major_version : 8;
            uint32_t reserved2 : 6;
            uint32_t antenna_short : 1;
            uint32_t antenna_connected : 1;
        };
        uint32_t word;
    };
    Flags flags;
} __attribute__((packed));