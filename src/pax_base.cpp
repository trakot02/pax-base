#include "pax_base.hpp"

namespace pax {

bool unicode_is_valid(u32 value)
{
    return (value >= 0x0    && value < 0xd800) ||
           (value >= 0xe000 && value < 0x110000);
}

bool unicode_is_invalid(u32 value)
{
    return (value >= 0xd800 && value < 0xe000) || (value >= 0x110000);
}

bool unicode_is_surrogate(u32 value)
{
    return (value >= 0xdc00 && value < 0xe000);
}

bool unicode_is_surr_low(u32 value)
{
    return (value >= 0xdc00 && value < 0xe000);
}

bool unicode_is_surr_high(u32 value)
{
    return (value >= 0xd800 && value < 0xdc00);
}

isize utf8_get_units(u32 value)
{
    isize units = 0;

    if (value >= 0x0     && value < 0x80)     units = 1;
    if (value >= 0x80    && value < 0x800)    units = 2;
    if (value >= 0x800   && value < 0x10000)  units = 3;
    if (value >= 0x10000 && value < 0x110000) units = 4;

    return units;
}

isize utf8_get_units_ahead(u8 value)
{
    isize units = 0;

    if      ((value & 0x80) == 0x0)  units = 1;
    else if ((value & 0xe0) == 0xc0) units = 2;
    else if ((value & 0xf0) == 0xe0) units = 3;
    else if ((value & 0xf8) == 0xf0) units = 4;

    return units;
}

bool utf8_is_trailing(u8 value)
{
    return (value & 0xc0) == 0x80;
}

bool utf8_is_overlong(u32 value, isize units)
{
    return ((value >= 0xc080     && value < 0xc200)     && units == 2) ||
           ((value >= 0xe08080   && value < 0xe0a000)   && units == 3) ||
           ((value >= 0xf0808080 && value < 0xf0c00000) && units == 4);
}

bool str8_init(String_8* self, u8* value, isize limit)
{
    isize length = 0;

    if (value == 0) {
        self->memory = 0;
        self->length = 0;

        return true;
    }

    while (value[length] != 0) {
        if (length >= limit)
            return false;

        length += 1;
    }

    self->memory = value;
    self->length = length;

    return true;
}

UTF_Result str8_encode(String_8 self, isize index, u32 value)
{
    UTF_Result result = {};

    isize units  = utf8_get_units(value);
    isize stop   = index + units;
    u8*   memory = self.memory + index;

    if (unicode_is_invalid(value) == true)
        result.error = UTF_ERROR_INVALID;

    if (unicode_is_surrogate(value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (index < 0 || stop > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

    if (result.error != UTF_ERROR_NONE) return result;

    switch (units) {
        case 1: {
            memory[0] = (u8)(value);
        } break;

        case 2: {
            memory[0] = (u8)((value >> 6   ) | 0xc0);
            memory[1] = (u8)((value  & 0x3f) | 0x80);
        } break;

        case 3: {
            memory[0] = (u8)(( value >> 12        ) | 0xe0);
            memory[1] = (u8)(((value >>  6) & 0x3f) | 0x80);
            memory[2] = (u8)(( value        & 0x3f) | 0x80);
        } break;

        case 4: {
            memory[0] = (u8)(( value >> 18        ) | 0xf0);
            memory[1] = (u8)(((value >> 12) & 0x3f) | 0x80);
            memory[2] = (u8)(((value >>  6) & 0x3f) | 0x80);
            memory[3] = (u8)(( value        & 0x3f) | 0x80);
        } break;

        default: {
            result.error = UTF_ERROR_UNREACHABLE;
        } break;
    }

    result.value = value;
    result.units = units;

    return result;
}

UTF_Result str8_decode(String_8 self, isize index)
{
    UTF_Result result = {};

    u32   value  = 0;
    isize units  = 0;
    u8*   memory = self.memory + index;

    if (index < 0 || index > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;
    else
        units = utf8_get_units_ahead(memory[0]);

    if (index + units > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

    for (isize i = 1; i < units; i += 1) {
        bool state = utf8_is_trailing(memory[i]);

        if (state == false)
            result.error = UTF_ERROR_INVALID;
    }

    if (result.error != UTF_ERROR_NONE) return result;

    switch (units) {
        case 1: { value = memory[0]; } break;

        case 2: {
            value  = (memory[0] & 0x1f) << 6;
            value += (memory[1] & 0x3f);
        } break;

        case 3: {
            value  = (memory[0] & 0x0f) << 12;
            value += (memory[1] & 0x3f) <<  6;
            value += (memory[2] & 0x3f);
        } break;

        case 4: {
            value  = (memory[0] & 0x07) << 18;
            value += (memory[1] & 0x3f) << 12;
            value += (memory[2] & 0x3f) <<  6;
            value += (memory[3] & 0x3f);
        } break;

        default: { result.error = UTF_ERROR_UNREACHABLE; } break;
    }

    if (utf8_is_overlong(value, units) == true)
        result.error = UTF_ERROR_OVERLONG;

    if (unicode_is_surrogate(value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (unicode_is_invalid(value) == true)
        result.error = UTF_ERROR_INVALID;

    result.value = value;
    result.units = units;

    return result;
}

isize str8_count_as_utf16(String_8 self)
{
    isize index  = 0;
    isize result = 0;

    while (index < self.length) {
        UTF_Result decode = str8_decode(self, index);

        if (decode.error != UTF_ERROR_NONE)
            return -1;

        result += utf16_get_units(decode.value);

        index += decode.units;
    }

    return result;
}

isize str8_count_as_utf32(String_8 self)
{
    isize index  = 0;
    isize result = 0;

    while (index < self.length) {
        UTF_Result decode = str8_decode(self, index);

        if (decode.error != UTF_ERROR_NONE)
            return -1;

        result += 1;

        index += decode.units;
    }

    return result;
}

bool str8_to_utf16(String_8 self, String_16* string, Mem_Arena* arena)
{
    String_16 result = {};

    isize marker = arena->offset;
    isize units  = str8_count_as_utf16(self);

    Mem_Block block = arena_push_array(arena, units + 1,
        PAX_SIZE_OF(u16), PAX_ALIGN_OF(u16));

    if (block.memory == 0) return false;

    result.memory = (u16*)(block.memory);
    result.length = units;

    isize index = 0;
    isize other = 0;

    while (index < self.length) {
        UTF_Result decode = str8_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        UTF_Result encode = str16_encode(result, other,
            decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        index += decode.units;
        other += encode.units;
    }

    *string = result;

    return true;
}

bool str8_to_utf32(String_8 self, String_32* string, Mem_Arena* arena)
{
    String_32 result = {};

    isize marker = arena->offset;
    isize units  = str8_count_as_utf32(self);

    Mem_Block block = arena_push_array(arena, units + 1,
        PAX_SIZE_OF(u32), PAX_ALIGN_OF(u32));

    if (block.memory == 0) return false;

    result.memory = (u32*)(block.memory);
    result.length = units;

    isize index = 0;
    isize other = 0;

    while (index < self.length) {
        UTF_Result decode = str8_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        UTF_Result encode = str32_encode(result, other,
            decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        index += decode.units;
        other += encode.units;
    }

    *string = result;

    return true;
}

isize utf16_get_units(u32 value)
{
    isize units = 0;

    if (value >= 0x0     && value < 0x10000)  units = 1;
    if (value >= 0x10000 && value < 0x110000) units = 2;

    return units;
}

isize utf16_get_units_ahead(u16 value)
{
    isize units = 0;

    if      (value  < 0xd800 || value >= 0xe000) units = 1;
    else if (value >= 0xd800 && value  < 0xdc00) units = 2;

    return units;
}

bool str16_init(String_16* self, u16* value, isize limit)
{
    isize length = 0;

    if (value == 0) {
        self->memory = 0;
        self->length = 0;

        return true;
    }

    while (value[length] != 0) {
        if (length >= limit)
            return false;

        length += 1;
    }

    self->memory = value;
    self->length = length;

    return true;
}

UTF_Result str16_encode(String_16 self, isize index, u32 value)
{
    UTF_Result result = {};

    isize units  = utf16_get_units(value);
    isize stop   = index + units;
    u16*  memory = self.memory + index;

    if (unicode_is_invalid(value) == true)
        result.error = UTF_ERROR_INVALID;

    if (unicode_is_surrogate(value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (index < 0 || stop > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

    if (result.error != UTF_ERROR_NONE) return result;

    switch (units) {
        case 1: {
            memory[0] = (u16)(value);
        } break;

        case 2: {
            u32 temp = value - 0x10000;

            memory[0] = (u16)((temp >>    10) + 0xd800);
            memory[1] = (u16)((temp  & 0x3ff) + 0xdc00);
        } break;

        default: {
            result.error = UTF_ERROR_UNREACHABLE;
        } break;
    }

    result.value = value;
    result.units = units;

    return result;
}

UTF_Result str16_decode(String_16 self, isize index)
{
    UTF_Result result = {};

    u32   value  = 0;
    isize units  = 0;
    u16*  memory = self.memory + index;

    if (index < 0 || index > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;
    else
        units = utf16_get_units_ahead(memory[0]);

    if (index + units > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

    if (result.error != UTF_ERROR_NONE) return result;

    switch (units) {
        case 1: { value = memory[0]; } break;

        case 2: {
            bool state = unicode_is_surr_low(memory[1]);

            if (state == true) {
                value  = 0x10000;
                value += (memory[0] - 0xd800) << 10;
                value += (memory[1] - 0xdc00);
            } else
                result.error = UTF_ERROR_INVALID;
        } break;

        default: {
            result.error = UTF_ERROR_UNREACHABLE;
        } break;
    }

    result.value = value;
    result.units = units;

    return result;
}

isize str16_count_as_utf8(String_16 self)
{
    isize index  = 0;
    isize result = 0;

    while (index < self.length) {
        UTF_Result decode = str16_decode(self, index);

        if (decode.error != UTF_ERROR_NONE)
            return -1;

        result += utf8_get_units(decode.value);

        index += decode.units;
    }

    return result;
}

isize str16_count_as_utf32(String_16 self)
{
    isize index  = 0;
    isize result = 0;

    while (index < self.length) {
        UTF_Result decode = str16_decode(self, index);

        if (decode.error != UTF_ERROR_NONE)
            return -1;

        result += 1;

        index += decode.units;
    }

    return result;
}

bool str16_to_utf8(String_16 self, String_8* string, Mem_Arena* arena)
{
    String_8 result = {};

    isize marker = arena->offset;
    isize units  = str16_count_as_utf8(self);

    Mem_Block block = arena_push_array(arena, units + 1,
        PAX_SIZE_OF(u8), PAX_ALIGN_OF(u8));

    if (block.memory == 0) return false;

    result.memory = (u8*)(block.memory);
    result.length = units;

    isize index = 0;
    isize other = 0;

    while (index < self.length) {
        UTF_Result decode = str16_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        UTF_Result encode = str8_encode(result, other,
            decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        index += decode.units;
        other += encode.units;
    }

    *string = result;

    return true;
}

bool str16_to_utf32(String_16 self, String_32* string, Mem_Arena* arena)
{
    String_32 result = {};

    isize marker = arena->offset;
    isize units  = str16_count_as_utf32(self);

    Mem_Block block = arena_push_array(arena, units + 1,
        PAX_SIZE_OF(u32), PAX_ALIGN_OF(u32));

    if (block.memory == 0) return false;

    result.memory = (u32*)(block.memory);
    result.length = units;

    isize index = 0;
    isize other = 0;

    while (index < self.length) {
        UTF_Result decode = str16_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        UTF_Result encode = str32_encode(result, other,
            decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        index += decode.units;
        other += encode.units;
    }

    *string = result;

    return true;
}

bool str32_init(String_32* self, u32* value, isize limit)
{
    isize length = 0;

    if (value == 0) {
        self->memory = 0;
        self->length = 0;

        return true;
    }

    while (value[length] != 0) {
        if (length >= limit)
            return false;

        length += 1;
    }

    self->memory = value;
    self->length = length;

    return true;
}

UTF_Result str32_encode(String_32 self, isize index, u32 value)
{
    UTF_Result result = {};

    isize units  = 1;
    isize stop   = index + units;
    u32*  memory = self.memory + index;

    if (unicode_is_invalid(value) == true)
        result.error = UTF_ERROR_INVALID;

    if (unicode_is_surrogate(value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (index < 0 || stop > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

    if (result.error == UTF_ERROR_NONE) {
        memory[0] = value;

        result.value = value;
        result.units = units;
    }

    return result;
}

UTF_Result str32_decode(String_32 self, isize index)
{
    UTF_Result result = {};

    u32   value  = 0;
    isize units  = 0;
    u32*  memory = self.memory + index;

    if (index < 0 || index > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;
    else
        units = 1;

    if (index + units > self.length)
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

    if (result.error == UTF_ERROR_NONE) {
        value = memory[0];

        result.value = value;
        result.units = units;
    }

    return result;
}

isize str32_count_as_utf8(String_32 self)
{
    isize result = 0;

    for (isize i = 0; i < self.length; i += 1) {
        UTF_Result decode = str32_decode(self, i);

        if (decode.error != UTF_ERROR_NONE)
            return -1;

        result += utf8_get_units(decode.value);
    }

    return result;
}

isize str32_count_as_utf16(String_32 self)
{
    isize result = 0;

    for (isize i = 0; i < self.length; i += 1) {
        UTF_Result decode = str32_decode(self, i);

        if (decode.error != UTF_ERROR_NONE)
            return -1;

        result += utf16_get_units(decode.value);
    }

    return result;
}

bool str32_to_utf8(String_32 self, String_8* string, Mem_Arena* arena)
{
    String_8 result = {};

    isize marker = arena->offset;
    isize units  = str32_count_as_utf8(self);

    Mem_Block block = arena_push_array(arena, units + 1,
        PAX_SIZE_OF(u8), PAX_ALIGN_OF(u8));

    if (block.memory == 0) return false;

    result.memory = (u8*)(block.memory);
    result.length = units;

    isize index = 0;
    isize other = 0;

    while (index < self.length) {
        UTF_Result decode = str32_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        UTF_Result encode = str8_encode(result, other,
            decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        index += decode.units;
        other += encode.units;
    }

    *string = result;

    return true;
}

bool str32_to_utf16(String_32 self, String_16* string, Mem_Arena* arena)
{
    String_16 result = {};

    isize marker = arena->offset;
    isize units  = str32_count_as_utf16(self);

    Mem_Block block = arena_push_array(arena, units + 1,
        PAX_SIZE_OF(u16), PAX_ALIGN_OF(u16));

    if (block.memory == 0) return false;

    result.memory = (u16*)(block.memory);
    result.length = units;

    isize index = 0;
    isize other = 0;

    while (index < self.length) {
        UTF_Result decode = str32_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        UTF_Result encode = str16_encode(result, other,
            decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, marker);

            return false;
        }

        index += decode.units;
        other += encode.units;
    }

    *string = result;

    return true;
}

isize align_by(isize value, isize align)
{
    isize error = value % align;
    isize extra = 0;

    if (error != 0)
        extra = align - error;

    return value + extra;
}

void arena_init(Mem_Arena* self, Mem_Block block)
{
    if (block.memory == 0)
        block.length = 0;

    self->memory = block.memory;
    self->length = block.length;
}

void arena_clear(Mem_Arena* self)
{
    self->offset = 0;
}

Mem_Block arena_push(Mem_Arena* self, isize bytes, isize align)
{
    Mem_Block result = {};

    isize offset = align_by(self->offset, align);
    u8*   memory = self->memory + offset;

    if (bytes <= 0 || align <= 0) return result;

    if (offset + bytes > self->length)
        return result;

    result.memory = memory;
    result.length = bytes;

    self->offset = offset + bytes;

    for (isize i = 0; i < bytes; i += 1)
        memory[i] = 0;

    return result;
}

Mem_Block arena_push_array(Mem_Arena* self, isize items, isize stride, isize align)
{
    return arena_push(self, items * stride, align);
}

bool arena_pop(Mem_Arena* self, isize marker)
{
    if (marker < 0 || marker > self->offset)
        return false;

    self->offset = marker;

    return true;
}

} // namespace pax
