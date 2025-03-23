#ifndef PAX_BASE_HPP
#define PAX_BASE_HPP

#include "pax_defs.hpp"

namespace pax {

//
// Values
//

static const u8* const UTF_ERROR_NAMES[] = {
    (const u8*) PAX_TO_STRING(UTF_ERROR_NONE),
    (const u8*) PAX_TO_STRING(UTF_ERROR_UNREACHABLE),
    (const u8*) PAX_TO_STRING(UTF_ERROR_INVALID),
    (const u8*) PAX_TO_STRING(UTF_ERROR_OVERLONG),
    (const u8*) PAX_TO_STRING(UTF_ERROR_SURROGATE),
    (const u8*) PAX_TO_STRING(UTF_ERROR_OUT_OF_BOUNDS),
};

//
// Types
//

struct String_8 {
    u8*   memory;
    isize length;
};

struct String_16 {
    u16*  memory;
    isize length;
};

struct String_32 {
    u32*  memory;
    isize length;
};

using String = String_8;

enum Utf_Error {
    UTF_ERROR_NONE,
    UTF_ERROR_UNREACHABLE,
    UTF_ERROR_INVALID,
    UTF_ERROR_OVERLONG,
    UTF_ERROR_SURROGATE,
    UTF_ERROR_OUT_OF_BOUNDS,
};

struct Utf_Result {
    u32       value;
    isize     units;
    Utf_Error error;
};

struct Mem_Block {
    u8*   memory;
    isize length;
};

struct Mem_Arena {
    u8*   memory;
    isize length;
    isize offset;
};

//
// Procs
//

/* Unicode, UTF-8, UTF-16, UTF-32 */

bool unicode_is_valid(u32 value);

bool unicode_is_invalid(u32 value);

bool unicode_is_surrogate(u32 value);

bool unicode_is_surr_low(u32 value);

bool unicode_is_surr_high(u32 value);

isize utf8_get_units(u32 value);

isize utf8_get_units_ahead(u8 value);

bool utf8_is_trailing(u8 value);

bool utf8_is_overlong(u32 value, isize units);

bool str8_init(String_8* self, u8* value, isize limit);

bool str8_from_utf16(String_8* self, String_16* string, Mem_Arena* arena);

Utf_Result str8_encode(String_8* self, isize index, u32 value);

Utf_Result str8_decode(String_8* self, isize index);

isize str8_count_as_utf16(String_8* self);

isize utf16_get_units(u32 value);

isize utf16_get_units_ahead(u16 value);

bool str16_init(String_16* self, u16* value, isize limit);

bool str16_from_utf8(String_16* self, String_8* string, Mem_Arena* arena);

Utf_Result str16_encode(String_16* self, isize index, u32 value);

Utf_Result str16_decode(String_16* self, isize index);

isize str16_count_as_utf8(String_16* self);

/* Arena */

isize align_by(isize value, isize align);

void arena_init(Mem_Arena* self, Mem_Block block);

void arena_clear(Mem_Arena* arena);

ptr arena_push(Mem_Arena* arena, isize bytes, isize align);

ptr arena_push_array(Mem_Arena* arena, isize items, isize stride, isize align);

bool arena_pop(Mem_Arena* arena, isize marker);

} // namespace pax

#define PAX_STR8(x) String_8 {(u8*)(x), PAX_ARRAY_ITEMS(x) - 1}

#endif // PAX_BASE_HPP
