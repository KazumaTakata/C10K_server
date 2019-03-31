#include "file.hpp"

std::string read_file(std::string path)
{
    /* open an existing file for reading */
    std::string folder_path = "static/";

    std::ifstream t(folder_path + path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
}