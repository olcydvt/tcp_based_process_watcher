#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace task_tcp_client {
    class tcp_client {
    public:
        static constexpr int32_t BUFFER_SIZE = 1024;

    private:
        int _socket;
        struct sockaddr_in _serv_addr;
        char buffer[BUFFER_SIZE] = {0};

    public:
        bool initialize_socket() {
            // Create socket
            if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                std::cerr << "Socket creation error" << std::endl;
                return false;
            }
            return true;
        }

        bool client_connect(const int32_t port) {
            // Set up server address structure
            _serv_addr.sin_family = AF_INET;
            _serv_addr.sin_port = htons(port);

            // Convert IPv4 address from text to binary form
            if (inet_pton(AF_INET, "127.0.0.1", &_serv_addr.sin_addr) <= 0) {
                std::cerr << "Invalid address / Address not supported" << std::endl;
                close(_socket);
                return false;
            }

            // Connect to server
            if (connect(_socket, (struct sockaddr*) &_serv_addr, sizeof(_serv_addr)) < 0) {
                std::cerr << "Connection failed" << std::endl;
                close(_socket);
                return false;
            }
            std::cout << "Connected to server at 127.0.0.1:" << port << std::endl;
            return true;
        }

        void send_message(std::string_view message) const {
            send(_socket, message.data(), message.length(), 0);
        }

        void disconnect() const {
            std::string disconnect_msg = "Client disconnecting";
            send(_socket, disconnect_msg.c_str(), disconnect_msg.length(), 0);
            usleep(100000);               // 100ms
            shutdown(_socket, SHUT_RDWR); // Gracefully shutdown the connection
            close(_socket);
            std::cout << "Disconnected from server cleanly" << std::endl;
        }
    };
}
