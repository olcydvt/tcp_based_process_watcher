#pragma once

#include "file_manager.hpp"
#include <functional>

namespace task_tcp_server {
    class alignas(64) session {
        // Structure to hold client information
        int32_t fd;          // Client socket file descriptor
        std::string address; // Client IP address
        int32_t id;          // Unique client ID

    public:
        session(const int fd, std::string_view addr, const int id)
                : fd(fd),
                  address(addr),
                  id(id) {}

        const int32_t get_fd() const {
            return fd;
        }

        const std::string& get_address() const {
            return address;
        }

        const int32_t get_id() const {
            return id;
        }

        void on_incoming(char* buffer, ssize_t count, const std::function<void(int, char*, ssize_t)>& on_response) {
            buffer[count] = '\0';
            parse_message(buffer);
            //No need on_response, client does not expect any response
            //on_response(fd, buffer, count);
        }

    private:
        void parse_message(char* buffer) {
            ;
        }
    };
};
