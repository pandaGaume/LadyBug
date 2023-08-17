#include "lb_stream.hpp"

using namespace BlueSteelLadyBug;

int StreamView::read(lb_byte_t *target, int count)
{
    if (_pos < _size)
    {
        size_t l = min((size_t)count, remainBytes);
        l = _delegate->read(target, l);
        _pos += l;
        remainBytes -= l;
        return l;
    }
    return LB_EOF;
}

bool StreamView::seek(int value, SeekOrigin origin)
{
    size_t tmp = origin == BEGIN ? value : origin == END ? _size - value
                                                         : _pos + value;
    _pos = min(max(tmp, 0), _size);
    _delegate->seek(_pos + _offset, BEGIN);
    remainBytes = _size - _pos;
    return true;
}
