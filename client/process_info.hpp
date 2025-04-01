#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace task_tcp_client {
    class process_snapshot {
    public:
        struct process_info {
            pid_t pid;
            std::string command;
            std::string user;
        };

    private:
        std::vector<process_info> _processes;

    public:
        std::vector<process_info>& get_process_info() {
            _processes.clear();
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("ps -eo pid,comm,user", "r"), pclose);
            if (!pipe) {
                std::cerr << "Failed to run ps command" << std::endl;
                return _processes;
            }

            char buffer[256];
            bool first_line = true;

            fgets(buffer, sizeof(buffer), pipe.get());
            _processes.reserve(64);

            while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
                if (first_line) {
                    // Skip header line
                    first_line = false;
                    continue;
                }

                std::string line(buffer);
                std::istringstream iss(line);

                process_info info;
                std::string pid_str;

                if (iss >> pid_str >> info.command >> info.user) {
                    try {
                        info.pid = std::stoi(pid_str);
                        info.user = info.user.substr(0, info.user.find_last_not_of(" \n\r") + 1);
                        _processes.push_back(info);
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing PID: " << pid_str << std::endl;
                    }
                }
            }

            return _processes;
        }
    };
}
