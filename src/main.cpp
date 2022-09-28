/*
 * Copyright (C) 2020-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sysexits.h>
#include <vector>

static constexpr std::size_t DEFAULT_BUFFER_SIZE = 4096;

int main(int argc, char **argv) {
    const std::string exe_name = std::filesystem::path(argv[0]).filename().string();
    std::string input_file  = "-";
    std::string output_file = "-";
    std::size_t buffer_size = DEFAULT_BUFFER_SIZE;

    // parse args
    if (argc != 1) {
        std::vector<std::string> args;

        for (int i = 1; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }

        bool read_input_file  = false;
        bool read_output_file = false;
        bool read_buffer_size = false;

        // parse options
        for (const auto &arg : args) {
            if (read_input_file) {
                read_input_file = false;
                input_file      = arg;
                continue;
            }

            if (read_output_file) {
                read_output_file = false;
                output_file      = arg;
                continue;
            }

            if (read_buffer_size) {
                read_buffer_size = false;
                try {
                    buffer_size = std::stoull(arg, nullptr, 0);
                } catch (const std::exception &) {
                    std::cerr << "Failed to parse '" << arg << "' as buffer size" << std::endl;
                    return EX_USAGE;
                }
                continue;
            }

            if (arg == "-h" || arg == "--help") {
                std::cout << "Bitwise invert" << std::endl;
                std::cout << "Usage: " << exe_name << " [OPTION...]" << std::endl;
                std::cout << std::endl;
                std::cout << "  -i  --input arg       input file (read from stdin if not specified)" << std::endl;
                std::cout << "  -o  --output arg      output file (write to stdout if not specified)" << std::endl;
                std::cout << "  -b  --buffer arg      buffer size in bytes (default 4096)" << std::endl;
                std::cout << "      --help            print usage" << std::endl;
                std::cout << "      --license         print license" << std::endl;
                std::cout << "      --version         print version" << std::endl;
                        return EX_OK;
            }

            if (arg == "--version") {
                std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION << " (compiled with " << COMPILER_INFO << " on "
                          << SYSTEM_INFO << ')' << std::endl;
                return EX_OK;
            }

            if (arg == "--license") {
                std::cout << "    MIT License" << std::endl;
                std::cout << std::endl;
                std::cout << "    Copyright (c) 2022 Nikolas Koesling" << std::endl;
                std::cout << std::endl;
                std::cout << "    Permission is hereby granted, free of charge, to any person obtaining a copy";
                std::cout << std::endl;
                std::cout << "    of this software and associated documentation files (the \"Software\"), to deal";
                std::cout << std::endl;
                std::cout << "    in the Software without restriction, including without limitation the rights";
                std::cout << std::endl;
                std::cout << "    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell";
                std::cout << std::endl;
                std::cout << "    copies of the Software, and to permit persons to whom the Software is" << std::endl;
                std::cout << "    furnished to do so, subject to the following conditions:" << std::endl;
                std::cout << std::endl;
                std::cout << "    The above copyright notice and this permission notice shall be included in all";
                std::cout << std::endl;
                std::cout << "    copies or substantial portions of the Software." << std::endl;
                std::cout << std::endl;
                std::cout << "    THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR";
                std::cout << std::endl;
                std::cout << "    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,";
                std::cout << std::endl;
                std::cout << "    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE";
                std::cout << std::endl;
                std::cout << "    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER" << std::endl;
                std::cout << "    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,";
                std::cout << std::endl;
                std::cout << "    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE";
                std::cout << std::endl;
                std::cout << "    SOFTWARE." << std::endl;
                return EX_OK;
            }

            if (arg == "-i" || arg == "--input") {
                read_input_file = true;
                continue;
            }

            if (arg == "-o" || arg == "--output") {
                read_output_file = true;
                continue;
            }

            if (arg == "-b" || arg == "--buffer") {
                read_buffer_size = true;
                continue;
            }

            std::cerr << "unknown argument '" << arg << "': use --help for more details" << std::endl;
            return EX_USAGE;
        }

        if (read_input_file) {
            std::cerr << "no file specified for argument --input." << std::endl;
            return EX_USAGE;
        }

        if (read_output_file) {
            std::cerr << "no file specified for argument --output." << std::endl;
            return EX_USAGE;
        }

        if (read_buffer_size) {
            std::cerr << "no size specified for argument --buffer." << std::endl;
            return EX_USAGE;
        }
    }

    std::ostream *output_stream = &std::cout;
    std::istream *input_stream  = &std::cin;

    std::unique_ptr<std::ifstream> infile;
    std::unique_ptr<std::ofstream> outfile;

    // open file streams
    if (input_file != "-") {
        infile = std::make_unique<std::ifstream>(input_file, std::ios::binary);
        if (!infile->is_open()) {
            std::cerr << "Failed to open input file: " << std::strerror(errno) << std::endl;
            return EX_OSFILE;
        }

        input_stream = infile.get();
    }

    if (output_file != "-") {
        outfile = std::make_unique<std::ofstream>(output_file, std::ios::binary);
        if (!outfile->is_open()) {
            std::cerr << "Failed to open output file: " << std::strerror(errno) << std::endl;
            return EX_OSFILE;
        }

        output_stream = outfile.get();
    }

    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(buffer_size);
    static_assert(sizeof(char) == 1);

    do {
        // read
        input_stream->read(buffer.get(), static_cast<std::streamsize>(buffer_size));
        auto bytes = input_stream->gcount();

        // invert
        std::size_t size_t_it = static_cast<std::size_t>(bytes) / sizeof(std::size_t);
        std::size_t byte_it   = static_cast<std::size_t>(bytes) - size_t_it;

        auto arr = reinterpret_cast<std::size_t *>(buffer.get());
        for (std::size_t i = 0; i < size_t_it; ++i) {
            arr[i] ^= ~(static_cast<std::size_t>(0));
        }

        for (std::size_t i = 0; i < byte_it; ++i) {
            buffer[i + size_t_it * sizeof(std::size_t)] ^= static_cast<char>(0xFF);
        }

        // write
        output_stream->write(buffer.get(), bytes);
    } while (!input_stream->fail());

    if (!input_stream->eof()) {
        std::cerr << "Failed to read input: " << std::strerror(errno) << std::endl;
        return EX_OSFILE;
    }
}
