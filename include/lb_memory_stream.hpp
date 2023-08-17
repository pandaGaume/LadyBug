#ifndef _BLUESTEEL_LADYBUG_MEMORY_STREAM__
#define _BLUESTEEL_LADYBUG_MEMORY_STREAM__

#include "lb.h"
#include "lb_stream.hpp"

namespace BlueSteelLadyBug
{
    class MemoryStream : public IInputStream
    {
    public:
        MemoryStream(lb_byte_t *buffer, size_t size)
        {
            _buffer = buffer;
            _size = size;
            _pos = 0;
            remainBytes = size;
        }
        ~MemoryStream() override {}
        int read(lb_byte_t *target, int count = 1) override;
        bool seek(int value, SeekOrigin origin = BEGIN) override;
        bool canSeek() override { return true; };

    private:
        lb_byte_t *_buffer;
        size_t _size;
        size_t _pos;
    };
}

#endif