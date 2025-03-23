#ifndef PAX_DEFS_HPP
#define PAX_DEFS_HPP

#define PAX_SYSTEM_UNKNOWN 0
#define PAX_SYSTEM_WINDOWS 1
#define PAX_SYSTEM_MAX     2

#define PAX_COMP_UNKNOWN 0
#define PAX_COMP_MSVC    1
#define PAX_COMP_MAX     2

#define PAX_ARCH_UNKNOWN 0
#define PAX_ARCH_64      1
#define PAX_ARCH_32      2
#define PAX_ARCH_MAX     3

#define PAX_LANG_UNKNOWN 0
#define PAX_LANG_C       1
#define PAX_LANG_CPP     2
#define PAX_LANG_MAX     3

#ifndef PAX_SYSTEM

    #if _WIN32

        #define PAX_SYSTEM PAX_SYSTEM_WINDOWS

    #else
        #define PAX_SYSTEM PAX_SYSTEM_UNKNOWN
    #endif

#endif

#ifndef PAX_COMP

    #if _MSC_VER

        #define PAX_COMP PAX_COMP_MSVC

    #else
        #define PAX_COMP PAX_COMP_UNKNOWN
    #endif

#endif

#ifndef PAX_ARCH

    #if _WIN64 || __PL64__ || __x86_64__ || __amd64__ || __aarch64__

        #define PAX_ARCH PAX_ARCH_64

    #elif _WIN32 || _ILP32__ || _i386_

        #define PAX_ARCH PAX_ARCH_32

    #else
        #define PAX_ARCH PAX_ARCH_UNKNOWN
    #endif

#endif

#if PAX_ARCH == PAX_ARCH_UNKNOWN
    #error "Unable to detect architecture..."
#endif

namespace pax {

#if PAX_ARCH == PAX_ARCH_64

    using i32 = int;
    using u32 = unsigned int;

    using isize = i32;
    using usize = u32;

#elif PAX_ARCH == PAX_ARCH_32

    using i32 = long;
    using u32 = unsigned long;

    using isize = i32;
    using usize = u32;

#endif

using i64 = long long;
using u64 = unsigned long long;

using i16 = short;
using u16 = unsigned short;

using i8 = char;
using u8 = unsigned char;

using f64 = double;
using f32 = float;

using ptr = void*;

} // namespace pax

#endif // PAX_DEFS_HPP
