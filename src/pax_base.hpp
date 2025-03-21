#ifndef PAX_BASE
#define PAX_BASE

#define PAX_SYSTEM_UNKNOWN 1
#define PAX_COMP_UNKNOWN 1
#define PAX_ARCH_UNKNOWN 1

#if _WIN32

    #define PAX_SYSTEM_WINDOWS 1
    #undef  PAX_SYSTEM_UNKNOWN

#endif

#if _MSC_VER

    #define PAX_COMP_MSVC 1
    #undef  PAX_COMP_UNKNOWN

#endif

#if _WIN64 || __LP64__ || __x86_64__ || __amd64__ || __aarch64__

    #define PAX_ARCH_64 1
    #undef  PAX_ARCH_UNKNOWN

#elif _WIN32 || __ILP32__ || _i386_

    #define PAX_ARCH_32 1
    #undef  PAX_ARCH_UNKNOWN

#endif

namespace pax {

typedef float  f32;
typedef double f64;

typedef char           i8;
typedef unsigned char  u8;
typedef short          i16;
typedef unsigned short u16;

#if PAX_ARCH_64

    typedef int                i32;
    typedef unsigned int       u32;
    typedef long long          i64;
    typedef unsigned long long u64;

    typedef long long          isize;
    typedef unsigned long long usize;

#elif PAX_ARCH_32

    typedef long               i32;
    typedef unsigned long      u32;
    typedef long long          i64;
    typedef unsigned long long u64;

    typedef long          isize;
    typedef unsigned long usize;

#endif

typedef void* pntr;

}

#define PAX_SIZE_OF(x)  ((pax::isize)(sizeof  (x)))
#define PAX_ALIGN_OF(x) ((pax::isize)(alignof (x)))

#define PAX_ARRAY_LENGTH(array) (PAX_SIZE_OF(array))
#define PAX_ARRAY_STRIDE(array) (PAX_SIZE_OF((array)[0]))
#define PAX_ARRAY_ITEMS(array)  (PAX_ARRAY_LENGTH(array) / PAX_ARRAY_STRIDE(array))

#define PAX_TEST(cond) ((cond) ? 1 : -1)

namespace pax_impl
{
    using namespace pax;

    static u8 test_size_of_f32[PAX_TEST(PAX_SIZE_OF(f32) == 4)];
    static u8 test_size_of_f64[PAX_TEST(PAX_SIZE_OF(f64) == 8)];

    static u8 test_size_of_i8[PAX_TEST(PAX_SIZE_OF(i8) == 1)];
    static u8 test_size_of_u8[PAX_TEST(PAX_SIZE_OF(u8) == 1)];

    static u8 test_size_of_i16[PAX_TEST(PAX_SIZE_OF(i16) == 2)];
    static u8 test_size_of_u16[PAX_TEST(PAX_SIZE_OF(u16) == 2)];

    static u8 test_size_of_i32[PAX_TEST(PAX_SIZE_OF(i32) == 4)];
    static u8 test_size_of_u32[PAX_TEST(PAX_SIZE_OF(u32) == 4)];

    static u8 test_size_of_i64[PAX_TEST(PAX_SIZE_OF(i64) == 8)];
    static u8 test_size_of_u64[PAX_TEST(PAX_SIZE_OF(u64) == 8)];

    static u8 test_size_of_isize[
        PAX_TEST(PAX_SIZE_OF(isize) == PAX_SIZE_OF(pntr))];

    static u8 test_size_of_usize[
        PAX_TEST(PAX_SIZE_OF(usize) == PAX_SIZE_OF(pntr))];

}

#define PAX_MAX(x, y) ((x) < (y) ? (y) : (x))
#define PAX_MIN(x, y) ((x) < (y) ? (x) : (y))

#define PAX_LIMIT_TOP(x, y)    PAX_MIN((x), (y))
#define PAX_LIMIT_BOTTOM(x, y) PAX_MAX((x), (y))

#endif
