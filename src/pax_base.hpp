#ifndef PAX_BASE_HPP
#define PAX_BASE_HPP

#include "pax_defs.hpp"

#define __PAX_TO_STRING__(x) #x
#define __PAX_CONCAT__(x, y) x##y

#define PAX_TO_STRING(x) __PAX_TO_STRING__(x)
#define PAX_CONCAT(x, y) __PAX_CONCAT__(x, y)

#define PAX_SIZE_OF(x)  sizeof(x)
#define PAX_ALIGN_OF(x) alignof(x)

#define PAX_ARRAY_LENGTH(x) PAX_SIZE_OF((x))
#define PAX_ARRAY_STRIDE(x) PAX_SIZE_OF((x)[0])

#define PAX_ARRAY_ITEMS(x) (PAX_ARRAY_LENGTH(x) / PAX_ARRAY_STRIDE(x))

#define PAX_MAX(x, y) ((x) < (y) ? (y) : (x))
#define PAX_MIN(x, y) ((x) < (y) ? (x) : (y))

#define PAX_CLAMP_TOP(x, y)    PAX_MIN((x), (y))
#define PAX_CLAMP_BOTTOM(x, y) PAX_MAX((x), (y))
#define PAX_CLAMP(x, y, z)     PAX_MAX(x, PAX_MIN(y, z))

#define PAX_BYTE_PTR(x) ((pax::u8*)(x))

#define PAX_STR_8(x) \
    String_8 {(pax::u8*)(x), PAX_ARRAY_ITEMS(x) - 1}

#define PAX_BLOCK(x) \
    Mem_Block {(pax::u8*)(x), PAX_ARRAY_LENGTH(x)}

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

enum UTF_Error {
    UTF_ERROR_NONE,
    UTF_ERROR_UNREACHABLE,
    UTF_ERROR_INVALID,
    UTF_ERROR_OVERLONG,
    UTF_ERROR_SURROGATE,
    UTF_ERROR_OUT_OF_BOUNDS,
};

struct UTF_Result {
    u32       value;
    isize     units;
    UTF_Error error;
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

/* Unicode */

bool unicode_is_valid(u32 value);

bool unicode_is_invalid(u32 value);

bool unicode_is_surrogate(u32 value);

bool unicode_is_surr_low(u32 value);

bool unicode_is_surr_high(u32 value);

/* UTF-8 */

isize utf8_get_units(u32 value);

isize utf8_get_units_ahead(u8 value);

bool utf8_is_trailing(u8 value);

bool utf8_is_overlong(u32 value, isize units);

bool str8_init(String_8* self, u8* value, isize limit);

UTF_Result str8_encode(String_8 self, isize index, u32 value);

UTF_Result str8_decode(String_8 self, isize index);

isize str8_count_as_utf16(String_8 self);

isize str8_count_as_utf32(String_8 self);

bool str8_to_utf16(String_8 self, String_16* string, Mem_Arena* arena);

bool str8_to_utf32(String_8 self, String_32* string, Mem_Arena* arena);

/* UTF-16 */

isize utf16_get_units(u32 value);

isize utf16_get_units_ahead(u16 value);

bool str16_init(String_16* self, u16* value, isize limit);

UTF_Result str16_encode(String_16 self, isize index, u32 value);

UTF_Result str16_decode(String_16 self, isize index);

isize str16_count_as_utf8(String_16 self);

isize str16_count_as_utf32(String_16 self);

bool str16_to_utf8(String_16 self, String_8* string, Mem_Arena* arena);

bool str16_to_utf32(String_16 self, String_32* string, Mem_Arena* arena);

/* UTF-32 */

bool str32_init(String_32* self, u32* value, isize limit);

UTF_Result str32_encode(String_32 self, isize index, u32 value);

UTF_Result str32_decode(String_32 self, isize index);

isize str32_count_as_utf8(String_32 self);

isize str32_count_as_utf16(String_32 self);

bool str32_to_utf8(String_32 self, String_8* string, Mem_Arena* arena);

bool str32_to_utf16(String_32 self, String_16* string, Mem_Arena* arena);

/* Arena */

isize align_by(isize value, isize align);

void arena_init(Mem_Arena* self, Mem_Block block);

void arena_clear(Mem_Arena* arena);

Mem_Block arena_push(Mem_Arena* arena, isize bytes, isize align);

Mem_Block arena_push_array(Mem_Arena* arena, isize items, isize stride, isize align);

bool arena_pop(Mem_Arena* arena, isize marker);

} // namespace pax

#endif // PAX_BASE_HPP
