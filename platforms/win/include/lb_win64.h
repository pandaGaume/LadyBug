#ifndef __BLUESTEEL_LADYBUG_WIN64__
#define __BLUESTEEL_LADYBUG_WIN64__

#include <stdlib.h>
#include <windows.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LB_LITTLE_ENDIAN 1

    typedef int lb_int32_t;
    typedef int lb_sint32_t;
    typedef long long lb_int64_t;
    typedef unsigned int lb_uint32_t;
    typedef unsigned short lb_uint16_t;
    typedef unsigned long long lb_uint64_t;

    typedef float lb_float_t;
    typedef double lb_double_t;

    typedef bool lb_bool_t;

    typedef unsigned char lb_byte_t;
    typedef unsigned char lb_fastbyte_t;

#define lb_memset memset
#define lb_memcpy(copy, ptr, size) memcpy((copy), (ptr), (size))

#define lb_malloc(size) malloc((size))
#define lb_realloc(ptr, size) realloc((ptr), (size))
#define lb_free(ptr) free((ptr))

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifdef __cplusplus
}
#endif

#endif