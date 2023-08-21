
#include <iostream>
#include <fstream>

#include "lb_parser.hpp"
#include "lb_memory_stream.hpp"

#define MAX_DEPTH 32

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

#define GETSPACE(s)                                      \
    char spaces[s];                                      \
    memset((void *)spaces, ' ', reader->getDepth() + 1); \
    spaces[reader->getDepth() + 1] = '\0';

#define NEWLINE "\r\n"

#define JSON_NAME(s) "\"" #s "\""

#define READ_SUB_MESSAGE(n, fn)                                             \
    std::cout << spaces << JSON_NAME(n) << ":{" << NEWLINE;                 \
    BlueSteelLadyBug ::PBReader *subReader = reader->getSubMessageReader(); \
    fn(subReader);                                                          \
    delete subReader;                                                       \
    std::cout << spaces << "}," << NEWLINE;

void readString(BlueSteelLadyBug ::PBReader *reader, const char *fieldName)
{
    GETSPACE(MAX_DEPTH)
    size_t l;
    reader->readLength(&l);
    char *name = (char *)malloc(l + 1);
    reader->readValue(name);
    std::cout << spaces << "\"" << fieldName << "\":\"" << name << "\"," << NEWLINE;
    free(name);
}

template <typename T>
void readNumber(BlueSteelLadyBug ::PBReader *reader, const char *fieldName)
{
    GETSPACE(MAX_DEPTH)
    T v;
    reader->readValue(&v);
    std::cout << spaces << "\"" << fieldName << "\":" << v << "," << NEWLINE;
}

void readAttribute(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case (1):
            {
                readString(reader, "name");
                break;
            }
            case (2):
            {
                readNumber<lb_float_t>(reader, "f");
                break;
            }
            case (3):
            {
                readNumber<lb_int64_t>(reader, "i");
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

void readNode(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case (1):
            {
                readString(reader, "input");
                break;
            }
            case (2):
            {
                readString(reader, "output");
                break;
            }
            case (3):
            {
                readString(reader, "name");
                break;
            }
            case (4):
            {
                readString(reader, "op_type");
                break;
            }
            case (5):
            {
                READ_SUB_MESSAGE(attribute, readAttribute);
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

void readDimension(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                readNumber<lb_uint64_t>(reader, "dim_value");
                break;
            }
            case 2:
            {
                readString(reader, "dim_param");
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            };
        } while (reader->readTag());
    }
};

void readShape(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                READ_SUB_MESSAGE(dim, readDimension);
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            };
        } while (reader->readTag());
    }
};

void readTensorType(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                readNumber<lb_uint32_t>(reader, "elem_type");
                break;
            }
            case 2:
            {
                READ_SUB_MESSAGE(shape, readShape);
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            };
        } while (reader->readTag());
    }
};

void readType(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                READ_SUB_MESSAGE(tensor_type, readTensorType);
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            };
        } while (reader->readTag());
    }
};

void readValueInfo(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                readString(reader, "name");
                break;
            }
            case 2:
            {
                READ_SUB_MESSAGE(type, readType);
                break;
            }
            default:
            {
                reader->skip();
                break;
            }
            };
        } while (reader->readTag());
    }
};

void readGraph(BlueSteelLadyBug ::PBReader *reader)
{
    GETSPACE(MAX_DEPTH)
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                READ_SUB_MESSAGE(node, readNode);
                break;
            }
            case 2:
            {
                readString(reader, "name");
                break;
            }
            case 11:
            {
                READ_SUB_MESSAGE(input, readValueInfo);
                break;
            }
            case 12:
            {
                READ_SUB_MESSAGE(output, readValueInfo);
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
    GETSPACE(MAX_DEPTH)
    std::cout << "{" << NEWLINE;
    if (reader->readTag())
    {
        do
        {
            switch (reader->getFieldNumber())
            {
            case 1:
            {
                readNumber<lb_uint32_t>(reader, "ir_version");
                break;
            }
            case 2:
            {
                readString(reader, "producer_name");
                break;
            }
            case 3:
            {
                readString(reader, "producer_version");
                break;
            }
            case 4:
            {
                readString(reader, "domain");
                break;
            }
            case 5:
            {
                readNumber<lb_uint32_t>(reader, "model_version");
                break;
            }
            case 7:
            {
                READ_SUB_MESSAGE(graph, readGraph);
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
    std::cout << "}" << NEWLINE;
};
int main()
{
    const char *filename = "C:/Users/guill/Documents/sources/LadyBug/models/LSTM/lstm.onnx";
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