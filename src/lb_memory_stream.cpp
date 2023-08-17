#include "lb_memory_stream.hpp"

using namespace BlueSteelLadyBug;

int MemoryStream::read(lb_byte_t *target, int count)
{
    if (_pos < _size)
    {
        size_t c = count;
        if (c == 1)
        {
            *target = *(_buffer + _pos);
            _pos++;
            remainBytes--;
            return 1;
        }
        size_t l = min(c, remainBytes);
        memcpy(target, _buffer, l);
        _pos += l;
        remainBytes -= l;
        return l;
    }
    return LB_EOF;
}

bool MemoryStream::seek(int value, SeekOrigin origin)
{
    size_t tmp = origin == BEGIN ? value : origin == END ? _size - value
                                                         : _pos + value;
    _pos = min(max(tmp, 0), _size);
    remainBytes = _size - _pos;
    return true;
}
