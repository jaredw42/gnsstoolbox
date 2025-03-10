#include "tcp.h"
#include "sbp_parser.h"
#include "ubx_parser.h"

TcpBinaryClient::TcpBinaryClient(boost::asio::io_context& io_context,
                                 const std::string& host,
                                 const std::string& port,
                                 std::shared_ptr<GnssDeviceInterface> deviceInterface)
    : io_context_(io_context),
      socket_(io_context),
      deviceInterface_(std::move(deviceInterface)),
      sbp_parser_([this](const std::variant<uint8_t, uint16_t>& messageType, const std::vector<uint8_t>& payload) {
          deviceInterface_->processData(messageType, payload);
      }),
      ubx_parser_([this](const std::variant<uint8_t, uint16_t>& messageType, const std::vector<uint8_t>& payload) {
          deviceInterface_->processData(messageType, payload);
      })
{
    tcp::resolver resolver(io_context_);
    auto endpoints = resolver.resolve(host, port);
    boost::asio::connect(socket_, endpoints);
}

void TcpBinaryClient::start() {
    do_read();
}

void TcpBinaryClient::do_read() {
    socket_.async_read_some(boost::asio::buffer(data_),
        [this](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                // Feed data to both parsers
                sbp_parser_.feed(data_.data(), length);
                ubx_parser_.feed(data_.data(), length);
                do_read();
            } else {
                if (ec == boost::asio::error::eof) {
                    std::cout << "Connection closed by peer.\n";
                } else {
                    std::cerr << "Error on receive: " << ec.message() << "\n";
                }
            }
        });
}