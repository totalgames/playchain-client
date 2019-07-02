#ifndef _PLAYCHAIN_DEFINES_
#define _PLAYCHAIN_DEFINES_

#include "platform_config.h"

#include <stdexcept>

#define PLAYCHAIN_ADDRESS_PREFIX "PLC"

// Workaround for varying preprocessing behavior between MSVC and gcc
#define PLAYCHAIN_EXPAND_MACRO(x) x

// suppress warning "conditional expression is constant" in the while(0) for visual c++
// http://cnicholson.net/2009/03/stupid-c-tricks-dowhile0-and-c4127/
#define PLAYCHAIN_MULTILINE_MACRO_BEGIN \
    do                                  \
    {
#ifdef _MSC_VER
#define PLAYCHAIN_MULTILINE_MACRO_END     \
    __pragma(warning(push))               \
        __pragma(warning(disable : 4127)) \
    }                                     \
    while (0)                             \
    __pragma(warning(pop))
#else
#define PLAYCHAIN_MULTILINE_MACRO_END \
    }                                 \
    while (0)
#endif

#define PLAYCHAIN_THROW_EXCEPTION(EXCEPTION, ...) \
    PLAYCHAIN_MULTILINE_MACRO_BEGIN               \
    throw EXCEPTION(__VA_ARGS__);                 \
    PLAYCHAIN_MULTILINE_MACRO_END

#define PLAYCHAIN_ASSERT(TEST, ...)                                      \
    PLAYCHAIN_EXPAND_MACRO(                                              \
        PLAYCHAIN_MULTILINE_MACRO_BEGIN if (!(TEST)) {                   \
            std::string s_what { #TEST ": " };                           \
            s_what += std::string { __VA_ARGS__ };                       \
            PLAYCHAIN_THROW_EXCEPTION(std::logic_error, s_what.c_str()); \
        } PLAYCHAIN_MULTILINE_MACRO_END)

#define PLAYCHAIN_ERROR(...) \
    PLAYCHAIN_ASSERT(false, __VA_ARGS__)

#define PLAYCHAIN_ASSERT_JSON(TEST) \
    PLAYCHAIN_ASSERT(TEST, "Invalid Json")

#endif //_PLAYCHAIN_DEFINES_
