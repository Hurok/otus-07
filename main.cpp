#include <iostream>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <sstream>
#include "Bulk.h"

static constexpr auto kArgSize = 1;

void writeToFile(std::time_t started, const std::vector<std::string> &data)
{
    auto tempPath = std::filesystem::temp_directory_path();
    std::stringstream ssfname;
    ssfname << "bulk" << started << ".log";
    const auto fname = ssfname.str();


    std::ofstream f;
    f.open(tempPath/"bulk"/fname);
    f << "bulk: " << std::accumulate(std::next(data.cbegin(), 1), data.cend(), *data.cbegin(), [](std::string res, std::string value){
        return std::move(res) + std::string(", ") + value;
    });
    f.flush();
    f.close();
}

void writeToStdout(std::time_t started, const std::vector<std::string> &data)
{
    std::cout << "bulk: " << std::accumulate(std::next(data.cbegin(), 1), data.cend(), *data.cbegin(), [](std::string res, std::string value){
        return std::move(res) + std::string(", ") + value;
    }) << std::endl;
}

int main(int argc, char **argv)
{
    try {
        // создание директории хранения данных {temp}/bulk/
        auto tempPath = std::filesystem::temp_directory_path();
        if (std::filesystem::exists(tempPath/"bulk"))
            std::filesystem::remove_all(tempPath/"bulk");

        if (!std::filesystem::exists(tempPath/"bulk"))
            std::filesystem::create_directory(tempPath/"bulk");

        // чтение размера блока
        if (argc != 2)
            throw std::logic_error{"invalid command line parameters, expected bulk size"};

        const std::size_t blockSize = std::stoi(argv[1]);
        if (blockSize <= 0)
            throw std::logic_error{"block size cannot be less than 1"};

        // подготовка
        Bulk bulk{blockSize};
        bulk.signReadyRead(std::bind(&writeToFile, std::placeholders::_1, std::placeholders::_2));
        bulk.signReadyRead(std::bind(&writeToStdout, std::placeholders::_1, std::placeholders::_2));

        // чтение блоков
        std::string line;
        std::vector<std::string> commands{blockSize};
        while (std::getline(std::cin, line)) {
            bulk.readNext(line);
        }

        if (std::cin.eof())
            bulk.readNext(std::string{});
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
