
#include "process_watcher.hpp"
#include "tcp_client.hpp"
#include <latch>

using namespace task_tcp_client;

int main() {
    std::latch running(1);
    process_watcher process_watcher;
    process_watcher.start();
    running.wait();
    return 0;
}
