#ifndef _BLUESTEEL_LADYBUG_DECL__
#define _BLUESTEEL_LADYBUG_DECL__

#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
/* UNIX-style OS. ------------------------------------------- */
#include <unistd.h>
#if defined(_POSIX_VERSION)
/* POSIX compliant */
#endif

#include <sys/param.h>
#if defined(BSD)
/* BSD (DragonFly BSD, FreeBSD, OpenBSD, NetBSD). ----------- */
#endif
#endif

#if defined(__linux__)
/* Linux  */
#endif

#if defined(__APPLE__) && defined(__MACH__)
/* Apple OSX and iOS (Darwin) */
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
/* iOS in Xcode simulator */
#elif TARGET_OS_IPHONE == 1
/* iOS on iPhone, iPad, etc. */
#elif TARGET_OS_MAC == 1
/* OS X */
#endif
#endif

#if defined(__CYGWIN__) && !defined(_WIN32)
/* Cygwin POSIX under Microsoft Windows. */
#endif

#if defined(_WIN64)
/* Microsoft Windows (64-bit) */
#include "lb_win64.h"
#elif defined(_WIN32)
/* Microsoft Windows (32-bit) */
#endif

#endif