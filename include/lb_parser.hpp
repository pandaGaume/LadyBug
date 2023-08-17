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
        lb_uint16_t depth;
        size_t position;
        WireType wireType;
        lb_uint32_t fieldNumber;

        lb_uint64_t length;
        bool lengthReaded;
    };

    class PBReader
    {
    public:
        PBReader()
        {
            _status.position = 0;
            _status.wireType = PB_VARINT;
            _status.fieldNumber = 0;
            _status.fieldNumber = 0;
            _status.depth = 0;
            _status.lengthReaded = false;
        }

        PBReader(IInputStream *input) : PBReader()
        {
            _input = input;
        }

        ~PBReader()
        {
        }

        /// @brief reads the next protobuf tag from the input source.
        /// @return true if the token was read successfully; otherwise, false.
        bool readTag();

        bool readLength(lb_uint64_t *);

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

        PBReader *getSubReader();

        bool skip();

        /// @brief Save the current status of the parser. To do so, the underlying stream MUST support Seek operation.
        void save();

        /// @brief Restore the status previously saved. To do so, the underlying stream MUST support Seek operation
        void restore();

        lb_uint32_t getFieldNumber() { return _status.fieldNumber; }
        WireType getWireType() { return _status.wireType; }
        size_t getPosition() { return _status.position; }
        lb_byte_t getDepth() { return _status.depth; }
        size_t getRemainBytes() { return _input->remainBytes; }
        IInputStream *getInput() { return _input; }

    protected:
        ReaderStatus _status;
        IInputStream *_input;

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

        void _invalidateLengthReaded() { _status.lengthReaded = false; }
    };

    typedef PBReader *PBReaderPtr;

    class PBSubReader : public PBReader
    {
    public:
        PBSubReader(PBReader *r, lb_uint16_t depth, size_t from, size_t length) : _sv(r->getInput(), from, length)
        {
            _status.depth = depth;
        }

    private:
        StreamView _sv;
    };

}

#endif