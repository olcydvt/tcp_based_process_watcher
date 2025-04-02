#pragma once

#include <chrono>
#include <functional>
#include <thread>

namespace task_tcp_client {
    template <typename Self>
    class timer {
        std::jthread _thread;
        Self& _self;

    public:
        explicit timer(Self& self) :_self(self) {}
        void start_periodic(const std::chrono::milliseconds& interval) {
            stop();
            _thread = std::jthread([=, this](const std::stop_token& stoken) {
                while (!stoken.stop_requested()) {
                    std::this_thread::sleep_for(interval);
                    if (!stoken.stop_requested()) {
                        this->_self.on_timeout();
                    }
                }
            });
        }

        void stop() {
            if (_thread.joinable()) {
                _thread.request_stop();
                _thread.join();
            }
        }

        ~timer() {
            stop();
        }
    };
}
