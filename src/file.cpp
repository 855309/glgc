#include "file.hpp"

std::vector<std::string> file_readlines(std::string path)
{
    std::fstream fileStream;
    fileStream.open(path, std::ios::in);

    std::vector<std::string> lines;

    if (fileStream.is_open())
    {
        std::string lc;

        while (getline(fileStream, lc))
        {
            lines.push_back(lc);
        }

        fileStream.close();
    }

    return lines;
}