#pragma once
#include <array>
#include <boost/asio.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "gnss_device_interface.h"
#include "sbp_parser.h"
#include "ubx_parser.h"

using boost::asio::ip::tcp;

class TcpBinaryClient {
   public:
    TcpBinaryClient(boost::asio::io_context& io_context, const std::string& host, const std::string& port,
                    std::shared_ptr<GnssDeviceInterface> deviceInterface);

    void start();

   private:
    void do_read();

    boost::asio::io_context& io_context_;
    tcp::socket socket_;
    std::array<char, 1024> data_;
    std::shared_ptr<GnssDeviceInterface> deviceInterface_;
    SwiftBinaryMessageParser sbp_parser_;
    ubx::UbloxParser ubx_parser_;
};