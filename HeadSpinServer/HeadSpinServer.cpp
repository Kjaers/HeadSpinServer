#pragma once
#define _CRT_SECURE_NO_WARNINGS
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <iostream>
#include <asio.hpp>
#include <../Libraries/ini.h>
#include "headspin_globals.h"

using asio::ip::udp;

class UDPServer {
public:
    UDPServer(asio::io_context& io_context, short port)
        : socket_(io_context, udp::endpoint(udp::v4(), port)) {
        start_receive();
    }

private:
    void start_receive() {
        socket_.async_receive_from(
            asio::buffer(recv_buffer_), remote_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                handle_receive(bytes_recvd);
            }
            else {
                start_receive();
            }
        });
    }

    void handle_receive(std::size_t length) {
        std::cout << "Received: " << std::string(recv_buffer_.data(), length) << std::endl;
        start_receive();
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
};

int main(int argc, char* argv[]) {
    try {

        // Find and read config file
        mINI::INIFile file("config.ini");
        mINI::INIStructure ini;

        // If no config file is found, create one
        if (!file.read(ini)) {
            std::cout << "Can't load 'config.ini', generating a new one...\n";

            // [server]
            ini["Server"]["port"] = "60000";

            // generate an INI file (overwrites any previous file)
            file.generate(ini, true);

            std::cout << "Generated config.ini\nUpdate the config file with your settings\nClosing server...\nServer Closed!\n";

            system("pause");
            return 0;
        }

        // Reading content from config file
        SERVER_PORT = ini.stringToUint16_t(ini["Server"]["port"]);


        // Starting server
        asio::io_context io_context;
        UDPServer server(io_context, SERVER_PORT);
        
        std::cout << "Started UDP server on port " << ini["Server"]["port"] << "\r\n";
        
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}