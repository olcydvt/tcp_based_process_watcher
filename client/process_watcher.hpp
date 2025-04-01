#pragma once
#include "process_info.hpp"
#include "tcp_client.hpp"
#include "timer.hpp"

namespace task_tcp_client {
    class process_watcher : public timer<process_watcher>,
                            public tcp_client,
                            public process_snapshot {
    public:
        process_watcher()
                : timer<process_watcher>(*this) {}

        void on_timeout() {
            std::cout << "process timeout" << std::endl;
            std::vector<process_snapshot::process_info>& process_list = this->get_process_info();
            if (process_list.empty()) {
                return;
            }
            std::string message;
            for (const auto& process_info: process_list) {
                message.append(std::to_string(process_info.pid) + ',' + process_info.command + ',' + process_info.user + ';');
            }
            this->send_message(message);
        }

        void start() {
            this->initialize_socket();
            this->client_connect(8080);
            this->start_periodic(std::chrono::seconds(30));
        }

        ~process_watcher() {
            this->stop();
            this->disconnect();
        }
    };
}
