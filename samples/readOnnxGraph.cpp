
#include <iostream>
#include <fstream>

#include "lb_parser.hpp"
#include "lb_memory_stream.hpp"

char *ReadFileIntoMemory(const char *filename, size_t *fileSize)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return nullptr;
    }

    *fileSize = file.tellg();
    char *buffer = new char[*fileSize];

    file.seekg(0, std::ios::beg);
    file.read(buffer, *fileSize);
    file.close();

    return buffer;
}

int main()
{
    const char *filename = "C:/Users/guill/Documents/sources/LadyBug/models/Abs/abs.onnx";
    size_t fileSize;
    char *fileContents = ReadFileIntoMemory(filename, &fileSize);

    if (fileContents)
    {
        BlueSteelLadyBug ::MemoryStream *input = new BlueSteelLadyBug ::MemoryStream((lb_byte_t *)fileContents, fileSize);
        BlueSteelLadyBug ::PBReader *reader = new BlueSteelLadyBug ::PBReader(input);
        if (reader->readTag())
        {
            do
            {
                std::cout << "field:" << reader->getFieldNumber() << ", wire type:" << (int)reader->getWireType() << "\r\n";
                reader->skip();
            } while (reader->readTag());
        }

        delete input;
        delete reader;
    }
    return 0;
}