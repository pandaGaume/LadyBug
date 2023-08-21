#ifndef _BLUESTEEL_LADYBUG_STREAM__
#define _BLUESTEEL_LADYBUG_STREAM__
#include "lb.h"

namespace BlueSteelLadyBug
{
    enum SeekOrigin
    {
        BEGIN = 0,   // Specifies the beginning of a stream.
        CURRENT = 1, // Specifies the current position within a stream.
        END = 2      // Specifies the end of a stream.
    };

#define LB_EOF -1;

    class IInputStream
    {
    public:
        virtual ~IInputStream(){};
        virtual int read(lb_byte_t *buf, int count = 1) = 0;
        virtual bool canSeek() = 0;
        virtual bool seek(int value, SeekOrigin origin = BEGIN) = 0;
        virtual size_t getSize() = 0;
        virtual size_t getPosition() = 0;
        virtual size_t getRemainingBytes() = 0;
    };

    class StreamView : public IInputStream
    {
    public:
        StreamView(IInputStream *delegate, size_t offset, size_t size)
        {
            _delegate = delegate;
            _offset = offset;
            _size = size;
            _pos = 0;
        }

        ~StreamView() override {}

        int read(lb_byte_t *target, int count = 1) override;
        bool seek(int value, SeekOrigin origin = BEGIN) override;
        bool canSeek() override { return _delegate->canSeek(); };
        size_t getSize() override { return _size; }
        size_t getPosition() override { return _pos; }
        size_t getRemainingBytes() override { return _size - _pos; }

    private:
        IInputStream *_delegate;
        size_t _offset;
        size_t _size;
        size_t _pos;
    };
}

#endif