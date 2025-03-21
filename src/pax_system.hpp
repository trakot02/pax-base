#ifndef PAX_SYSTEM_HPP
#define PAX_SYSTEM_HPP

#include "pax_base.hpp"

namespace pax {

//
// Types
//

struct String_UTF8 {
    u8*   memory = 0;
    isize length = 0;
};

struct String_UTF16 {
    u16*  memory = 0;
    isize length = 0;
};

typedef String_UTF8 String;

enum UTF_Error {
    UTF_ERROR_NONE,
    UTF_ERROR_UNREACHABLE,
    UTF_ERROR_INVALID,
    UTF_ERROR_OVERLONG,
    UTF_ERROR_SURROGATE,
    UTF_ERROR_OUT_OF_BOUNDS,
};

struct UTF_Result {
    u32       value = 0;
    isize     units = 0;
    UTF_Error error = UTF_ERROR_NONE;
};

struct Mem_Arena {
    u8*   memory = 0;
    isize length = 0;
    isize offset = 0;
};

enum Mem_Error {
    MEM_ERROR_NONE,
    MEM_ERROR_SYSTEM_OUT_OF_MEMORY,
    MEM_ERROR_ARENA_OUT_OF_MEMORY,
};

struct Mem_Result {
    u8*       memory = 0;
    isize     length = 0;
    Mem_Error error  = MEM_ERROR_NONE;
};

typedef pntr File_Handle;

enum File_Error {
    FILE_ERROR_NONE,
    FILE_ERROR_UNKNOWN,
    FILE_ERROR_NOT_FOUND,
    FILE_ERROR_ACCESS_DENIED,
    FILE_ERROR_ALREADY_EXISTS,
    FILE_ERROR_INVALID_PATH,
    FILE_ERROR_PATH_ENCODING_FAILED,
    FILE_ERROR_SYSTEM_OUT_OF_MEMORY,
    FILE_ERROR_ARENA_OUT_OF_MEMORY,
};

struct File_Result {
    File_Handle value = 0;
    File_Error  error = FILE_ERROR_NONE;
};

//
// Procs
//

/**
 *
 */
bool unicode_is_valid(u32 value);

/**
 *
 */
bool unicode_is_surrogate(u32 value);

/**
 *
 */
bool unicode_is_surrogate_low(u32 value);

/**
 *
 */
bool unicode_is_surrogate_high(u32 value);

/**
 *
 */
bool utf8_is_continuation(u8 value);

/**
 *
 */
bool utf8_is_overlong(u32 value, isize units);

/**
 *
 */
UTF_Result utf8_get_units(u32 value);

/**
 *
 */
isize utf8_get_units_ahead(u8 value);

/**
 *
 */
bool str8_init(String_UTF8* self, const u8* value, isize limit);

/**
 *
 */
UTF_Result str8_encode(String_UTF8* self, isize index, u32 value);

/**
 *
 */
UTF_Result str8_decode(String_UTF8* self, isize index);

/**
 *
 */
isize str8_get_units_utf16(String_UTF8* self);

/**
 *
 */
String_UTF16 str8_to_utf16_le(String_UTF8* self, Mem_Arena* arena);

/**
 *
 */
UTF_Result utf16_get_units(u32 value);

/**
 *
 */
isize utf16_get_units_ahead(u16 value);

/**
 *
 */
bool str16_init(String_UTF16* self, const u16* value, isize limit);

/**
 *
 */
UTF_Result str16_encode_le(String_UTF16* self, isize index, u32 value);

/**
 *
 */
UTF_Result str16_decode_le(String_UTF16* self, isize index);

/**
 *
 */
isize str16_get_units_utf8(String_UTF16* self);

/**
 *
 */
String_UTF8 str16_to_utf8(String_UTF16* self, Mem_Arena* arena);

/**
 *
 */
isize align_to(isize value, isize align);

/**
 *
 */
bool arena_init(Mem_Arena* self, Mem_Result memory);

/**
 *
 */
void arena_clear(Mem_Arena* self);

/**
 *
 */
Mem_Result arena_push(Mem_Arena* self, isize bytes, isize align);

/**
 *
 */
Mem_Result arena_push_array(Mem_Arena* self, isize items, isize stride, isize align);

/**
 *
 */
bool arena_pop(Mem_Arena* self, isize offset);

/**
 *
 */
isize system_get_page_size(isize pages);

/**
 *
 */
Mem_Result system_reserve(isize pages);

/**
 *
 */
void system_release(pntr value);

/**
 *
 */
File_Result file_create_if_new(String_UTF8* filename, Mem_Arena* arena);

/**
 *
 */
File_Result file_create_always(String_UTF8* filename, Mem_Arena* arena);

/**
 *
 */
File_Result file_open(String_UTF8* filename, Mem_Arena* arena);

/**
 *
 */
void file_close(File_Handle* self);

}

#endif
