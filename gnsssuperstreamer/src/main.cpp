#include <boost/asio.hpp>
#include <iostream>

#include "neo_m8_interface.h"
#include "piksi_multi_interface.h"
#include "tcp.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <ublox_host> <ublox_port> <swift_host> <swift_port>\n";
        return 1;
    }

    try {
        std::cout << "Starting program..." << std::endl;
        boost::asio::io_context io_context;
        static constexpr PositionLLH kPosition1430California = {37.79131855, -122.41806216, 45.583};

        std::cout << "Creating Ublox TCP client..." << std::endl;
        auto neoM8Interface = std::make_shared<ubx::NeoM8Interface>(kPosition1430California);
        TcpBinaryClient ubloxClient(io_context, argv[1], argv[2], neoM8Interface);
        ubloxClient.start();

        std::cout << "Creating Swift TCP client..." << std::endl;
        auto piksiMultiInterface = std::make_shared<PiksiMultiInterface>(kPosition1430California);
        TcpBinaryClient swiftClient(io_context, argv[3], argv[4], piksiMultiInterface);
        swiftClient.start();

        std::cout << "Running I/O service..." << std::endl;
        io_context.run();
        std::cout << "I/O service finished." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}