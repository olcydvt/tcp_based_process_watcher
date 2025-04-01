#pragma once

#include <arpa/inet.h>
#include <atomic>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

#include "session.hpp"

namespace task_tcp_server {

    class server {
    public:
        static constexpr int MAX_EVENTS = 10;
        static constexpr int MAX_BUFFER = 1024;

    private:
        std::unordered_map<int, session> _clients;
        std::atomic<bool> _running{true};

    public:
        void accept_and_listen(const int32_t port) {
            const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (server_fd == -1) {
                std::cerr << "Socket creation failed\n";
                return;
            }
            set_nonblocking(server_fd);

            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(port);
            if (bind(server_fd, (sockaddr*) &addr, sizeof(addr)) == -1 || listen(server_fd, SOMAXCONN) == -1) {
                std::cerr << "Bind/Listen failed\n";
                close(server_fd);
                return;
            }

            const int epoll_fd = epoll_create1(0);
            if (epoll_fd == -1) {
                std::cerr << "Epoll creation failed\n";
                close(server_fd);
                return;
            }

            epoll_event ev{};
            ev.events = EPOLLIN;
            ev.data.fd = server_fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

            int next_client_id = 1;

            while (_running) {
                epoll_event events[MAX_EVENTS];
                int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
                for (int i = 0; i < nfds; ++i) {
                    if (int fd = events[i].data.fd; fd == server_fd) {
                        sockaddr_in client_addr{};
                        socklen_t len = sizeof(client_addr);
                        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &len);
                        if (client_fd == -1)
                            continue;

                        set_nonblocking(client_fd);

                        char client_ip[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
                        _clients.emplace(client_fd, session(client_fd, std::string(client_ip), next_client_id++));

                        ev.events = EPOLLIN | EPOLLET;
                        ev.data.fd = client_fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                        std::cout << "Client " << _clients.at(client_fd).get_id() << " connected from " << client_ip << "\n";
                    } else {
                        char buffer[MAX_BUFFER];
                        if (const ssize_t count = read(fd, buffer, MAX_BUFFER); count <= 0) {
                            std::cout << "Client " << _clients.at(fd).get_id() << " disconnected\n";
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                            close(fd);
                            _clients.erase(fd);
                        } else {
                            _clients.at(fd).on_incoming(buffer, count, [self = this](const int _fd, char* buff, const ssize_t cnt) {
                                self->send_response(_fd, buff, cnt);
                            });
                        }
                    }
                }
            }
        }

        void send_response(int fd, char* buffer, ssize_t count) {
            buffer[count] = '\0';
            write(fd, buffer, count);
        }

    private:
        void set_nonblocking(int fd) {
            const int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
    };

}
