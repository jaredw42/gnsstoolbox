# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
# Set the project name and version
project(gnsssuperstreamer VERSION 1.0 LANGUAGES CXX)

# Add the include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/message
    ${CMAKE_SOURCE_DIR}/parser
    ${CMAKE_SOURCE_DIR}/connection
    ${CMAKE_SOURCE_DIR}/conversions
    ${CMAKE_SOURCE_DIR}/interface
    /usr/local/boost_1_87_0
)

# Find Boost libraries
set(BOOST_ROOT "/usr/local/boost_1_87_0")
set(Boost_INCLUDE_DIR "/usr/local/boost_1_87_0")
find_package(Boost REQUIRED)

# Add an executable (specify source files)
add_executable(gnsssuperstreamer
    src/main.cpp
    parser/ubx_parser.cpp
    parser/sbp_parser.cpp
    interface/piksi_multi_interface.cpp
    interface/neo_m8_interface.cpp
    connection/tcp.cpp
)

# Specify C++ standard
target_compile_features(gnsssuperstreamer PUBLIC cxx_std_20)

# Link Boost libraries
# target_link_libraries(gnsssuperstreamer Boost::system Boost::thread)