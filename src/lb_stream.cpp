#include "lb_stream.hpp"

using namespace BlueSteelLadyBug;

int StreamView::read(lb_byte_t *target, int count)
{
    if (_pos < _size)
    {
        size_t l = min((size_t)count, _size - _pos);
        l = _delegate->read(target, l);
        _pos += l;
        return l;
    }
    return LB_EOF;
}

bool StreamView::seek(int value, SeekOrigin origin)
{

    size_t tmp = origin == BEGIN ? value : origin == END ? _size - value
                                                         : _pos + value;
    tmp = min(max(tmp, 0), _size);
    if (!_delegate->seek(tmp + _offset, BEGIN))
    {
        return false;
    }
    _pos = tmp;
    return true;
}
