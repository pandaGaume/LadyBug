#ifndef _BLUESTEEL_LADYBUG_PULLPARSER__
#define _BLUESTEEL_LADYBUG_PULLPARSER__

#include "lb.h"
#include "lb_stream.hpp"

namespace BlueSteelLadyBug
{
    enum WireType : lb_byte_t
    {
        PB_VARINT = 0,
        PB_64BIT = 1,
        PB_LEN = 2,
        PB_32BIT = 5
    };

    struct ReaderStatus
    {
        lb_byte_t currentDepth;
        size_t position;
        WireType wireType;
        WireType packedWireType;
        lb_uint32_t fieldNumber;
    };

    class PBReader
    {
    public:
        PBReader(IInputStream *input)
        {
            lb_memset(&_status, 0, sizeof(ReaderStatus));
            _input = input;
        }

        PBReader(PBReader &other)
        {
            _status = other._status;
            _input = other._input;
        }
        ~PBReader() {}

        /// @brief reads the next protobuf tag from the input source.
        /// @return true if the token was read successfully; otherwise, false.
        bool readTag();

        bool readValue(lb_int32_t *);
        bool readValue(lb_int64_t *);
        bool readValue(lb_uint32_t *);
        bool readValue(lb_uint64_t *);
        bool readValue(lb_float_t *);
        bool readValue(lb_double_t *);
        bool readValue(lb_bool_t *);
        bool readValue(char *);
        bool readValue(lb_byte_t *);
        bool readValue_s(char *, int);
        bool readValue_s(lb_byte_t *, int);

        bool readPacked(lb_int32_t *, WireType);
        bool readPacked(lb_int64_t *, WireType);
        bool readPacked(lb_uint32_t *, WireType);
        bool readPacked(lb_uint64_t *, WireType);
        bool readPacked(lb_float_t *);
        bool readPacked(lb_double_t *);

        bool skip();

        /// @brief Save the current status of the parser. To do so, the underlying stream MUST support Seek operation.
        void save();

        /// @brief Restore the status previously saved. To do so, the underlying stream MUST support Seek operation
        void restore();

    private:
        ReaderStatus _status;
        IInputStream *_input;

        /// @brief Read raw uint data of varint format. Do not include sign process such ZigZag or Google's signed varint
        /// we use 64 bit as target because it's the largest type it can be saved in protobuf with the VARINT wire type.
        // possible types are : int32, int64, uint32, uint64, sint32, sint64, bool, enum
        /// @param dest
        /// @return
        bool _readVarint(lb_uint64_t *dest);
        bool _readSVarint(lb_int64_t *dest);
        bool _readFixed32(void *dest);
        bool _readFixed64(void *dest);

        bool _readValue(lb_int32_t *, WireType);
        bool _readValue(lb_int64_t *, WireType);
        bool _readValue(lb_uint32_t *, WireType);
        bool _readValue(lb_uint64_t *, WireType);
        bool _readValue(lb_float_t *, WireType);
        bool _readValue(lb_double_t *, WireType);

        template <typename T>
        bool _readPacked(T *, WireType);
    };

    typedef PBReader *PBReaderPtr;
}

#endif