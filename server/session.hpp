#pragma once

#include "file_manager.hpp"
#include <functional>
#include <sstream>

namespace task_tcp_server {
    class alignas(64) session {
        struct process_info {
            std::string pid;
            std::string command;
            std::string user;
        };

        int32_t _fd;
        std::string _address;
        int32_t _id;

    public:
        session(const int fd, std::string_view addr, const int id)
                : _fd(fd),
                  _address(addr),
                  _id(id) {}

        const int32_t get_fd() const {
            return _fd;
        }

        const std::string& get_address() const {
            return _address;
        }

        const int32_t get_id() const {
            return _id;
        }

        void on_incoming(char* buffer, const ssize_t count, const std::function<void(int, char*, ssize_t)>& on_response) {
            buffer[count] = '\0';
            parse_message(buffer);
            //No need on_response, client does not expect any response
            //on_response(fd, buffer, count);
        }

    private:
        void parse_message(const char* buffer) {
            std::vector<std::string> tokens;
            std::istringstream message_stream(buffer);

            std::string token;
            while (getline(message_stream, token, ';')) {
                process_info info;
                std::string::size_type pid_pos = 0;
                std::string::size_type command_name_pos = 0;
                std::string::size_type user_pos = 0;

                command_name_pos = token.find(',', pid_pos) + 1;
                info.pid = token.substr(0, command_name_pos - 1);
                user_pos = token.find(',', command_name_pos) + 1;
                info.command = token.substr(command_name_pos, user_pos - command_name_pos - 1);
                info.user = token.substr(user_pos);
            }
        }
    };
};
