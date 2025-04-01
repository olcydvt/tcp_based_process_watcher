
#include "tcp_server.hpp"

using namespace task_tcp_server;

int main() {
    server server;
    server.accept_and_listen(8080);
    return 0;
}
