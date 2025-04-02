#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace task_tcp_server {

    class file_manager {
        std::string _filename;

    public:
        explicit file_manager(const std::string& filename)
                : _filename(filename + ".csv") {}

        void write_line(const std::vector<std::string>& tokens) const {
            std::ofstream file(_filename, std::ios::app);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << _filename << "\n";
                return;
            }

            for (size_t i = 0; i < tokens.size(); ++i) {
                file << tokens[i];
                if (i < tokens.size() - 1)
                    file << ",";
            }
            file << "\n"; // New line for the next row
        }
    };
}
