
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

void readNode(BlueSteelLadyBug ::PBReader *reader)
{
    char spaces[32];
    memset((void *)spaces, ' ', reader->getDepth());
    spaces[reader->getDepth()] = '\0';

    std::cout << spaces << "+ Read Node\r\n";
    if (reader->readTag())
    {
        do
        {
            std::cout << spaces << "  field:" << reader->getFieldNumber() << ", wire type:" << (int)reader->getWireType() << "\r\n";
            switch (reader->getFieldNumber())
            {
            default:
            {
                reader->skip();
                break;
            }
            }
        } while (reader->readTag());
    }
};

void readInput(BlueSteelLadyBug ::PBReader *reader)
{
    char spaces[32];
    memset((void *)spaces, ' ', reader->getDepth());
    spaces[reader->getDepth()] = '\0';

    std::cout << "+ Read Input infos\r\n";
    if (reader->readTag())
    {
        do
        {
            std::cout << spaces << "  field:" << reader->getFieldNumber() << ", wire type:" << (int)reader->getWireType() << "\r\n";
            switch (reader->getFieldNumber())
            {
            default:
            {
                reader->skip();
                break;
            }
            }
        } while (reader->readTag());
    }
};

void readOutput(BlueSteelLadyBug ::PBReader *reader)
{
    char spaces[32];
    memset((void *)spaces, ' ', reader->getDepth());
    spaces[reader->getDepth()] = '\0';

    std::cout << "+ Read Output Infos\r\n";
    if (reader->readTag())
    {
        do
        {
            std::cout << spaces << "  field:" << reader->getFieldNumber() << ", wire type:" << (int)reader->getWireType() << "\r\n";
            switch (reader->getFieldNumber())
            {
            default:
            {
                reader->skip();
                break;
            }
            }
        } while (reader->readTag());
    }
};

void readGraph(BlueSteelLadyBug ::PBReader *reader)
{
    char spaces[32];
    memset((void *)spaces, ' ', reader->getDepth());
    spaces[reader->getDepth()] = '\0';
    std::cout << spaces << "+ Read Graph\r\n";
    if (reader->readTag())
    {
        do
        {
            std::cout << spaces << "  field:" << reader->getFieldNumber() << ", wire type:" << (int)reader->getWireType() << "\r\n";
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                BlueSteelLadyBug ::PBReader *subReader = reader->getSubMessageReader();
                readNode(subReader);
                delete subReader;
                break;
            }
            case 11:
            {
                BlueSteelLadyBug ::PBReader *subReader = reader->getSubMessageReader();
                readInput(subReader);
                delete subReader;
                break;
            }
            case 12:
            {
                BlueSteelLadyBug ::PBReader *subReader = reader->getSubMessageReader();
                readOutput(subReader);
                delete subReader;
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            }
        } while (reader->readTag());
    }
};

void readModel(BlueSteelLadyBug ::PBReader *reader)
{
    std::cout << "+ Read Model\r\n";

    if (reader->readTag())
    {
        do
        {
            std::cout << "  field:" << reader->getFieldNumber() << ", wire type:" << (int)reader->getWireType() << "\r\n";
            switch (reader->getFieldNumber())
            {
            case 7:
            {
                BlueSteelLadyBug ::PBReader *subReader = reader->getSubMessageReader();
                readGraph(subReader);
                delete subReader;
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            }
        } while (reader->readTag());
    }
};
int main()
{
    const char *filename = "C:/Users/guill/Documents/sources/LadyBug/models/Abs/abs.onnx";
    size_t fileSize;
    char *fileContents = ReadFileIntoMemory(filename, &fileSize);

    if (fileContents)
    {
        BlueSteelLadyBug ::MemoryStream *input = new BlueSteelLadyBug ::MemoryStream((lb_byte_t *)fileContents, fileSize);
        BlueSteelLadyBug ::PBReader *reader = new BlueSteelLadyBug ::PBReader(input);

        readModel(reader);

        delete input;
        delete reader;
    }
    return 0;
}