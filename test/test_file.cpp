/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This template is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

static constexpr std::size_t BUFFER_SIZE = 8190;

int main() {
    char buffer_src[BUFFER_SIZE], buffer_dst[BUFFER_SIZE];
    memset(buffer_src, 0xAA, BUFFER_SIZE);

    // write file
    std::ofstream src_file("binv_test_src", std::ios::binary);
    if (!src_file.is_open()) {
        std::cerr << "failed to open file binv_test_src" << std::endl;
        return EXIT_FAILURE;
    }
    src_file.write(buffer_src, BUFFER_SIZE);
    src_file.close();

    // invert
    int  result      = std::system("../binv -i binv_test_src -o binv_test_dst");
    auto exit_status = WEXITSTATUS(result);

    if (exit_status != 0) { return exit_status; }

    // read file
    std::ifstream dst_file("binv_test_dst", std::ios::binary);
    if (!dst_file.is_open()) {
        std::cerr << "failed to open file binv_test_src" << std::endl;
        return EXIT_FAILURE;
    }
    dst_file.read(buffer_dst, BUFFER_SIZE);
    dst_file.close();

    // check data
    for (std::size_t i = 0; i < BUFFER_SIZE; ++i) {
        if (buffer_src[i] != ~buffer_dst[i]) {
            std::cerr << "Failed @" << i << std::endl;
            return EXIT_FAILURE;
        }
    }

    auto tmp = std::system("rm -f binv_test_src binv_test_dst");
    static_cast<void>(tmp);
}
