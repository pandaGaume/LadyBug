#include "lb_parser.hpp"

using namespace BlueSteelLadyBug;

template <typename T>
bool PBReader::_readPacked(T *v, WireType wt)
{
    switch (_status.wireType)
    {
    case PB_LEN:
    {
        lb_uint64_t size;
        if (!readLength(&size))
        {
            return false;
        }
        _status.lengthReaded = false;
        size_t end = _status.position + size;
        bool r = true;
        if (_status.position < end)
        {
            do
            {
                if (!_readValue(v, wt))
                {
                    r = false;
                    break;
                }
                v++;
            } while (_status.position < end);
        }
        return r;
    }
    default:
    {
        return false;
    }
    }
}

bool PBReader::readLength(lb_uint64_t *v)
{
    switch (_status.wireType)
    {
    case PB_LEN:
    {
        if (_status.lengthReaded)
        {
            *v = _status.length;
        }
        else
        {
            if (!_readVarint(v))
            {
                return false;
            }
            _status.length = *v;
            _status.lengthReaded = true;
        }
        return true;
    }
    default:
    {
        return false;
    }
    }
}

bool PBReader::readTag()
{
    // read a tag.
    lb_uint64_t tag;
    if (_readVarint(&tag))
    {
        _status.fieldNumber = tag >> 3;
        _status.wireType = (WireType)(tag & 0x03);
        return true;
    }
    return false;
}

void PBReader::save()
{
    if (_input->canSeek())
    {
    }
}

void PBReader::restore()
{
    if (_input->canSeek())
    {
    }
}

bool PBReader::_readVarint(lb_uint64_t *dest)
{
    lb_byte_t byte;

    if (_input->read(&byte) < 0)
    {
        return false;
    }

    _status.position++;
    if (byte & 0x80)
    {
        lb_uint64_t res = byte & 0x7F;
        lb_fastbyte_t shift = 7;
        do
        {
            if (_input->read(&byte) < 0)
            {
                return false;
            }
            _status.position++;
            res |= (lb_uint64_t)(byte & 0x07) << shift;
            if ((byte & 0x80) == 0)
            {
                break;
            }
            shift += 7;
        } while (true);

        if (dest)
        {
            *dest = res;
        }
        return true;
    }

    // short track for 1 byte value
    if (dest)
    {
        *dest = byte;
    }
    return true;
}

bool PBReader::_readSVarint(lb_int64_t *dest)
{

    lb_uint64_t value;
    bool r = _readVarint(&value);
    if (r)
    {
        *dest = (value & 1) ? (lb_int64_t)(~(value >> 1)) : (lb_int64_t)(value >> 1);
    }
    return r;
}

bool PBReader::_readFixed32(void *dest)
{
    union
    {
        lb_uint32_t fixed32;
        lb_byte_t bytes[4];
    } u;

    bool r = _input->read(u.bytes, 4);
    if (r)
    {
#if defined(LB_LITTLE_ENDIAN) && LB_LITTLE_ENDIAN == 1
        // fast track
        *(lb_uint32_t *)dest = u.fixed32;
#else
        *(lb_uint32_t *)dest = ((lb_uint32_t)u.bytes[0] << 0) |
                               ((lb_uint32_t)u.bytes[1] << 8) |
                               ((lb_uint32_t)u.bytes[2] << 16) |
                               ((lb_uint32_t)u.bytes[3] << 24);
#endif
    }
    return r;
}

bool PBReader::_readFixed64(void *dest)
{
    union
    {
        lb_uint64_t fixed64;
        lb_byte_t bytes[8];
    } u;

    bool r = _input->read(u.bytes, 8);
    if (r)
    {
#if defined(LB_LITTLE_ENDIAN) && LB_LITTLE_ENDIAN == 1
        // fast track
        *(lb_uint64_t *)dest = u.fixed64;
#else
        *(lb_uint64_t *)dest = ((lb_uint64_t)u.bytes[0] << 0) |
                               ((lb_uint64_t)u.bytes[1] << 8) |
                               ((lb_uint64_t)u.bytes[2] << 16) |
                               ((lb_uint64_t)u.bytes[3] << 24) |
                               ((lb_uint64_t)u.bytes[4] << 32) |
                               ((lb_uint64_t)u.bytes[5] << 40) |
                               ((lb_uint64_t)u.bytes[6] << 48) |
                               ((lb_uint64_t)u.bytes[7] << 56);
#endif
    }
    return r;
}

bool PBReader::_readValue(lb_int32_t *v, WireType wt)
{
    switch (wt)
    {
    case PB_VARINT:
    {
        lb_int64_t value;
        if (!_readSVarint(&value))
        {
            return false;
        }
        *v = (lb_int32_t)value;
        return true;
    }
    case PB_32BIT:
    {
        return _readFixed32(v);
    }
    default:
    {
        return false;
    }
    }
}

inline bool PBReader::_readValue(lb_int64_t *v, WireType wt)
{
    switch (wt)
    {
    case PB_VARINT:
    {
        return _readSVarint(v);
    }
    case PB_64BIT:
    {
        return _readFixed64(v);
    }
    default:
    {
        return false;
    }
    }
    return false;
}

bool PBReader::_readValue(lb_uint32_t *v, WireType wt)
{
    switch (wt)
    {
    case PB_VARINT:
    {
        lb_uint64_t value;
        if (!_readVarint(&value))
        {
            return false;
        }
        *v = (lb_uint32_t)value;

        return true;
    }
    case PB_32BIT:
    {
        return _readFixed32(v);
    }
    default:
    {
        return false;
    }
    }
    return false;
}

bool PBReader::_readValue(lb_uint64_t *v, WireType wt)
{
    switch (wt)
    {
    case PB_VARINT:
    {
        return _readVarint(v);
    }
    case PB_64BIT:
    {
        return _readFixed64(v);
    }
    default:
    {
        return false;
    }
    }
}

bool PBReader::_readValue(lb_float_t *v, WireType wt)
{
    switch (wt)
    {
    case (PB_32BIT):
    {
        lb_uint32_t value;
        if (!_readFixed32(&value))
        {
            return false;
        }
        *v = (lb_float_t)value;
        return true;
    }

    default:
    {
        return false;
    }
    }
}

bool PBReader::_readValue(lb_double_t *v, WireType wt)
{
    switch (wt)
    {
    case (PB_64BIT):
    {
        lb_uint64_t value;
        if (!_readFixed64(&value))
        {
            return false;
        }
        *v = (lb_double_t)value;
        return true;
    }

    default:
    {
        return false;
    }
    }
}

inline bool PBReader::readValue(lb_int32_t *v)
{
    return _readValue(v, _status.wireType);
}

inline bool PBReader::readValue(lb_int64_t *v)
{
    return _readValue(v, _status.wireType);
}

inline bool PBReader::readValue(lb_uint32_t *v)
{
    return _readValue(v, _status.wireType);
}

inline bool PBReader::readValue(lb_uint64_t *v)
{
    return _readValue(v, _status.wireType);
}

inline bool PBReader::readValue(lb_float_t *v)
{
    return _readValue(v, _status.wireType);
}

inline bool PBReader::readValue(lb_double_t *v)
{
    return _readValue(v, _status.wireType);
}

bool PBReader::readValue(lb_bool_t *v)
{
    lb_uint64_t value;
    if (!_readVarint(&value))
    {
        return false;
    }
    *v = (value != 0);
    return true;
}

bool PBReader::readValue(char *v)
{
    lb_uint64_t size;
    if (!readLength(&size))
    {
        return false;
    }
    _status.lengthReaded = false;
    lb_byte_t *t = (lb_byte_t *)v;
    if (_input->read(t, (int)size) != (int)size)
    {
        return false;
    };
    t[size] = 0; // null terminated
    return true;
}

bool PBReader::readValue(lb_byte_t *v)
{
    lb_uint64_t size;
    if (!readLength(&size))
    {
        return false;
    }
    _status.lengthReaded = false;
    return _input->read(v, (int)size) == (int)size;
}

bool PBReader::skip()
{
    switch (_status.wireType)
    {
    case PB_LEN:
    {
        lb_uint64_t size;
        if (!readLength(&size))
        {
            return false;
        }
        _status.lengthReaded = false;
        return _input->seek((int)size, CURRENT);
    }
    case PB_VARINT:
    {
        return _readVarint(nullptr);
    }
    case PB_32BIT:
    {
        return _input->seek(4, CURRENT);
    }
    case PB_64BIT:
    {
        return _input->seek(8, CURRENT);
    }
    }
    return false;
}

bool PBReader::readValue_s(char *v, int s)
{
    lb_uint64_t size;
    if (!readLength(&size))
    {
        return false;
    }
    lb_byte_t *t = (lb_byte_t *)v;
    int max_size = min((int)size, s);
    if (_input->read(t, (int)max_size) != max_size)
    {
        return false;
    }
    if (max_size != (int)size && !_input->seek(size - max_size, CURRENT))
    {
        return false;
    }
    t[max_size] = 0; // null terminated
    return true;
}

bool PBReader::readValue_s(lb_byte_t *v, int s)
{
    lb_uint64_t size;
    if (!readLength(&size))
    {
        return false;
    }
    int max_size = min((int)size, s);
    if (_input->read(v, max_size) != max_size)
    {
        return false;
    }
    if (max_size != (int)size)
    {
        if (!_input->seek(size - max_size, CURRENT))
        {
            return false;
        }
    }
    return true;
}

inline bool PBReader::readPacked(lb_int32_t *v, WireType wt)
{
    return _readPacked<lb_int32_t>(v, wt);
}

inline bool PBReader::readPacked(lb_int64_t *v, WireType wt)
{
    return _readPacked<lb_int64_t>(v, wt);
}

inline bool PBReader::readPacked(lb_uint32_t *v, WireType wt)
{
    return _readPacked<lb_uint32_t>(v, wt);
}

inline bool PBReader::readPacked(lb_uint64_t *v, WireType wt)
{
    return _readPacked<lb_uint64_t>(v, wt);
}

inline bool PBReader::readPacked(lb_float_t *v)
{
    return _readPacked<lb_float_t>(v, PB_32BIT);
}

inline bool PBReader::readPacked(lb_double_t *v)
{
    return _readPacked<lb_double_t>(v, PB_64BIT);
}
