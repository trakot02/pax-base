#include "pax_system.hpp"

#if defined(PAX_SYSTEM_WINDOWS)

#define NOMINMAX
#define NOGDI
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace pax {

struct File_Impl {
    HANDLE handle;
};

isize system_get_page_size_impl(isize pages)
{
    SYSTEM_INFO info;

    GetSystemInfo(&info);

    return pages * info.dwPageSize;
}

Mem_Result system_reserve_impl(isize pages)
{
    Mem_Result result;

    isize length = system_get_page_size(pages);

    if (length <= 0) return result;

    pntr memory = VirtualAlloc(0, length,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
    );

    if (memory == 0) {
        result.error = MEM_ERROR_SYSTEM_OUT_OF_MEMORY;

        return result;
    }

    result.memory = (u8*)(memory);
    result.length = length;

    return result;
}

void system_release_impl(pntr value)
{
    VirtualFree(value, 0, MEM_RELEASE);
}

File_Error file_create_if_new_impl(File_Impl* self, String_UTF8* filename, Mem_Arena* arena)
{
    self->handle = 0;

    i32 access = GENERIC_READ | GENERIC_WRITE;
    i32 action = CREATE_NEW;
    i32 share  = 0;

    isize        offset = arena->offset;
    String_UTF16 result = str8_to_utf16_le(filename, arena);

    if (result.length == 0) return FILE_ERROR_PATH_ENCODING_FAILED;

    auto handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, offset);

    if (handle == INVALID_HANDLE_VALUE) {
        switch (GetLastError()) {
            case ERROR_FILE_NOT_FOUND:    return FILE_ERROR_NOT_FOUND;
            case ERROR_ACCESS_DENIED:     return FILE_ERROR_ACCESS_DENIED;
            case ERROR_FILE_EXISTS:       return FILE_ERROR_ALREADY_EXISTS;
            case ERROR_PATH_NOT_FOUND:    return FILE_ERROR_INVALID_PATH;
            case ERROR_NOT_ENOUGH_MEMORY: return FILE_ERROR_SYSTEM_OUT_OF_MEMORY;
        }

        return FILE_ERROR_UNKNOWN;
    }

    self->handle = handle;

    return FILE_ERROR_NONE;
}

File_Error file_create_always_impl(File_Impl* self, String_UTF8* filename, Mem_Arena* arena)
{
    self->handle = 0;

    i32 access = GENERIC_READ | GENERIC_WRITE;
    i32 action = CREATE_ALWAYS;
    i32 share  = 0;

    isize        offset = arena->offset;
    String_UTF16 result = str8_to_utf16_le(filename, arena);

    if (result.length == 0) return FILE_ERROR_PATH_ENCODING_FAILED;

    auto handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, offset);

    if (handle == INVALID_HANDLE_VALUE) {
        switch (GetLastError()) {
            case ERROR_FILE_NOT_FOUND:    return FILE_ERROR_NOT_FOUND;
            case ERROR_ACCESS_DENIED:     return FILE_ERROR_ACCESS_DENIED;
            case ERROR_FILE_EXISTS:       return FILE_ERROR_ALREADY_EXISTS;
            case ERROR_PATH_NOT_FOUND:    return FILE_ERROR_INVALID_PATH;
            case ERROR_NOT_ENOUGH_MEMORY: return FILE_ERROR_SYSTEM_OUT_OF_MEMORY;
        }

        return FILE_ERROR_UNKNOWN;
    }

    self->handle = handle;

    return FILE_ERROR_NONE;
}

File_Error file_open_impl(File_Impl* self, String_UTF8* filename, Mem_Arena* arena)
{
    self->handle = 0;

    i32 access = GENERIC_READ;
    i32 action = OPEN_EXISTING;
    i32 share  = FILE_SHARE_READ;

    isize        offset = arena->offset;
    String_UTF16 result = str8_to_utf16_le(filename, arena);

    if (result.length == 0) return FILE_ERROR_PATH_ENCODING_FAILED;

    auto handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, offset);

    if (handle == INVALID_HANDLE_VALUE) {
        switch (GetLastError()) {
            case ERROR_FILE_NOT_FOUND:    return FILE_ERROR_NOT_FOUND;
            case ERROR_ACCESS_DENIED:     return FILE_ERROR_ACCESS_DENIED;
            case ERROR_FILE_EXISTS:       return FILE_ERROR_ALREADY_EXISTS;
            case ERROR_PATH_NOT_FOUND:    return FILE_ERROR_INVALID_PATH;
            case ERROR_NOT_ENOUGH_MEMORY: return FILE_ERROR_SYSTEM_OUT_OF_MEMORY;
        }

        return FILE_ERROR_UNKNOWN;
    }

    self->handle = handle;

    return FILE_ERROR_NONE;
}

void file_close_impl(File_Impl* self)
{
    if (self->handle != INVALID_HANDLE_VALUE)
        CloseHandle(self->handle);

    self->handle = 0;
}

}

#endif

namespace pax {

bool unicode_is_valid(u32 value)
{
    return (value >= 0x0    && value < 0xd800) ||
           (value >= 0xe000 && value < 0x110000);
}

bool unicode_is_surrogate(u32 value)
{
    return (value >= 0xd800 && value < 0xe000);
}

bool unicode_is_surrogate_low(u32 value)
{
    return (value >= 0xdc00 && value < 0xe000);
}

bool unicode_is_surrogate_high(u32 value)
{
    return (value >= 0xd800 && value < 0xdc00);
}

bool utf8_is_continuation(u8 value)
{
    return (value & 0xc0) == 0x80;
}

bool utf8_is_overlong(u32 value, isize units)
{
    return ((value >= 0xc080     && value < 0xc200)     && units == 2) ||
           ((value >= 0xe08080   && value < 0xe0a000)   && units == 3) ||
           ((value >= 0xf0808080 && value < 0xf0c00000) && units == 4);
}

UTF_Result utf8_get_units(u32 value)
{
    UTF_Result result;

    isize units = 0;

    if (unicode_is_valid(value) == false)
        result.error = UTF_ERROR_INVALID;

    if (unicode_is_surrogate(value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (result.error != UTF_ERROR_NONE) return result;

    if (value >= 0x0     && value < 0x80)     units = 1;
    if (value >= 0x80    && value < 0x800)    units = 2;
    if (value >= 0x800   && value < 0x10000)  units = 3;
    if (value >= 0x10000 && value < 0x110000) units = 4;

    if (units >= 1 && units <= 4) {
        result.value = value;
        result.units = units;
    } else
        result.error = UTF_ERROR_UNREACHABLE;

    return result;
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

bool str8_init(String_UTF8* self, const u8* value, isize limit)
{
    isize length = 0;

    self->memory = 0;
    self->length = length;

    if (value == 0) return true;

    while (value[length] != 0) {
        if (length >= limit)
            return false;

        length += 1;
    }

    self->memory = (u8*)(value);
    self->length = length;

    return true;
}

UTF_Result str8_encode(String_UTF8* self, isize index, u32 value)
{
    UTF_Result result = utf8_get_units(value);

    if (result.error != UTF_ERROR_NONE) return result;

    isize stop   = index + result.units;
    u8*   memory = self->memory + index;

    if (index < 0 || stop > self->length) {
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

        return result;
    }

    switch (result.units) {
        case 1: { memory[0] = (u8)(value); } break;

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

        default: { result.error = UTF_ERROR_UNREACHABLE; } break;
    }

    return result;
}

UTF_Result str8_decode(String_UTF8* self, isize index)
{
    UTF_Result result;

    isize limit  = PAX_LIMIT_TOP(self->length - index, 4);
    u8*   memory = self->memory + index;

    if (index < 0 || index >= self->length) {
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

        return result;
    }

    isize units = utf8_get_units_ahead(memory[0]);

    if (units < 1 || units > limit) {
        if (units < 1)     result.error = UTF_ERROR_INVALID;
        if (units > limit) result.error = UTF_ERROR_OUT_OF_BOUNDS;

        return result;
    }

    for (isize i = 1; i < units; i += 1) {
        bool state = utf8_is_continuation(memory[i]);

        if (state == false)
            result.error = UTF_ERROR_INVALID;
    }

    result.units = units;

    switch (result.units) {
        case 1: { result.value = memory[0]; } break;

        case 2: {
            result.value  = (memory[0] & 0x1f) << 6;
            result.value += (memory[1] & 0x3f);
        } break;

        case 3: {
            result.value  = (memory[0] & 0x0f) << 12;
            result.value += (memory[1] & 0x3f) <<  6;
            result.value += (memory[2] & 0x3f);
        } break;

        case 4: {
            result.value  = (memory[0] & 0x07) << 18;
            result.value += (memory[1] & 0x3f) << 12;
            result.value += (memory[2] & 0x3f) <<  6;
            result.value += (memory[3] & 0x3f);
        } break;

        default: { result.error = UTF_ERROR_UNREACHABLE; } break;
    }

    if (unicode_is_surrogate(result.value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (unicode_is_valid(result.value) == false)
        result.error = UTF_ERROR_INVALID;

    if (utf8_is_overlong(result.value, result.units) == true)
        result.error = UTF_ERROR_OVERLONG;

    return result;
}

isize str8_get_units_utf16(String_UTF8* self)
{
    isize index  = 0;
    isize result = 0;

    while (index < self->length) {
        UTF_Result decode = str8_decode(self, index);
        UTF_Result encode = utf16_get_units(decode.value);

        if (decode.error != UTF_ERROR_NONE) return -1;
        if (encode.error != UTF_ERROR_NONE) return -1;

        index  += decode.units;
        result += encode.units;
    }

    return result;
}

String_UTF16 str8_to_utf16_le(String_UTF8* self, Mem_Arena* arena)
{
    String_UTF16 string;

    isize offset = arena->offset;
    isize items  = str8_get_units_utf16(self);

    Mem_Result result = arena_push_array(arena, items + 1,
        PAX_SIZE_OF(u16), PAX_ALIGN_OF(u16));

    if (result.error != MEM_ERROR_NONE) return string;

    string.memory = (u16*)(result.memory);
    string.length = items;

    isize index = 0;
    isize other = 0;

    while (index < self->length) {
        UTF_Result decode = str8_decode(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, offset);

            string.memory = 0;
            string.length = 0;

            return string;
        }

        UTF_Result encode = str16_encode_le(&string, other, decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, offset);

            string.memory = 0;
            string.length = 0;

            return string;
        }

        index += decode.units;
        other += encode.units;
    }

    return string;
}

UTF_Result utf16_get_units(u32 value)
{
    UTF_Result result;

    isize units = 0;

    if (unicode_is_valid(value) == false)
        result.error = UTF_ERROR_INVALID;

    if (unicode_is_surrogate(value) == true)
        result.error = UTF_ERROR_SURROGATE;

    if (result.error != UTF_ERROR_NONE) return result;

    if (value >= 0x0     && value < 0x10000)  units = 1;
    if (value >= 0x10000 && value < 0x110000) units = 2;

    if (units >= 1 && units <= 2) {
        result.value = value;
        result.units = units;
    } else
        result.error = UTF_ERROR_UNREACHABLE;

    return result;
}

isize utf16_get_units_ahead(u16 value)
{
    isize units = 0;

    if      (value  < 0xd800 || value >= 0xe000) units = 1;
    else if (value >= 0xd800 && value  < 0xdc00) units = 2;

    return units;
}

bool str16_init(String_UTF16* self, const u16* value, isize limit)
{
    isize length = 0;

    self->memory = 0;
    self->length = 0;

    if (value == 0) return true;

    while (value[length] != 0) {
        if (length > limit)
            return false;

        length += 1;
    }

    self->memory = (u16*)(value);
    self->length = length;

    return true;
}

UTF_Result str16_encode_le(String_UTF16* self, isize index, u32 value)
{
    UTF_Result result = utf16_get_units(value);

    if (result.error != UTF_ERROR_NONE) return result;

    isize stop   = index + result.units;
    u16*  memory = self->memory + index;

    if (index < 0 || stop > self->length) {
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

        return result;
    }

    switch (result.units) {
        case 0: { result.error = UTF_ERROR_INVALID; } break;

        case 1: { memory[0] = (u16)(value); } break;

        case 2: {
            u32 temp = value - 0x10000;

            memory[0] = (u16)((temp >>    10) + 0xd800);
            memory[1] = (u16)((temp  & 0x3ff) + 0xdc00);
        } break;

        default: { result.error = UTF_ERROR_UNREACHABLE; } break;
    }

    return result;
}

UTF_Result str16_decode_le(String_UTF16* self, isize index)
{
    UTF_Result result;

    isize limit  = PAX_LIMIT_TOP(self->length - index, 2);
    u16*  memory = self->memory + index;

    if (index < 0 || index >= self->length) {
        result.error = UTF_ERROR_OUT_OF_BOUNDS;

        return result;
    }

    isize units = utf16_get_units_ahead(memory[0]);

    if (units < 1 || units > limit) {
        if (units < 1)     result.error = UTF_ERROR_INVALID;
        if (units > limit) result.error = UTF_ERROR_OUT_OF_BOUNDS;

        return result;
    }

    result.units = units;

    switch (result.units) {
        case 1: { result.value = memory[0]; } break;

        case 2: {
            bool state = unicode_is_surrogate_low(memory[1]);

            if (state == true) {
                result.value  = 0x10000;
                result.value += (memory[0] - 0xd800) << 10;
                result.value += (memory[1] - 0xdc00);
            } else
                result.error = UTF_ERROR_INVALID;
        } break;

        default: { result.error = UTF_ERROR_UNREACHABLE; } break;
    }

    return result;
}

isize str16_get_units_utf8(String_UTF16* self)
{
    isize index  = 0;
    isize result = 0;

    while (index < self->length) {
        UTF_Result decode = str16_decode_le(self, index);
        UTF_Result encode = utf8_get_units(decode.value);

        if (decode.error != UTF_ERROR_NONE) return -1;
        if (encode.error != UTF_ERROR_NONE) return -1;

        index  += decode.units;
        result += encode.units;
    }

    return result;
}

String_UTF8 str16_to_utf8(String_UTF16* self, Mem_Arena* arena)
{
    String_UTF8 string;

    isize offset = arena->offset;
    isize items  = str16_get_units_utf8(self);

    Mem_Result result = arena_push_array(arena, items + 1,
        PAX_SIZE_OF(u8), PAX_ALIGN_OF(u8));

    if (result.error != MEM_ERROR_NONE) return string;

    string.memory = (u8*)(result.memory);
    string.length = items;

    isize index = 0;
    isize other = 0;

    while (index < self->length) {
        UTF_Result decode = str16_decode_le(self, index);

        if (decode.error != UTF_ERROR_NONE) {
            arena_pop(arena, offset);

            string.memory = 0;
            string.length = 0;

            return string;
        }

        UTF_Result encode = str8_encode(&string, other, decode.value);

        if (encode.error != UTF_ERROR_NONE) {
            arena_pop(arena, offset);

            string.memory = 0;
            string.length = 0;

            return string;
        }

        index += decode.units;
        other += encode.units;
    }

    return string;
}

isize align_to(isize value, isize align)
{
    auto error = value % align;
    auto extra = (isize)(0);

    if (error != 0)
        extra = align - error;

    return value + extra;
}

bool arena_init(Mem_Arena* self, Mem_Result memory)
{
    if (memory.error != MEM_ERROR_NONE)
        return false;

    self->memory = memory.memory;
    self->length = memory.length;

    return true;
}

void arena_clear(Mem_Arena* self)
{
    self->offset = 0;
}

Mem_Result arena_push(Mem_Arena* self, isize bytes, isize align)
{
    Mem_Result result;

    isize offset = align_to(self->offset, align);
    u8*   memory = self->memory + offset;

    if (bytes <= 0 || align <= 0) return result;

    if (offset + bytes > self->length) {
        result.error = MEM_ERROR_ARENA_OUT_OF_MEMORY;

        return result;
    }

    result.memory = memory;
    result.length = bytes;

    self->offset = offset + bytes;

    for (isize i = 0; i < bytes; i += 1)
        memory[i] = 0;

    return result;
}

Mem_Result arena_push_array(Mem_Arena* self, isize items, isize stride, isize align)
{
    return arena_push(self, items * stride, align);
}

bool arena_pop(Mem_Arena* self, isize offset)
{
    if (offset < 0 || offset >= self->length)
        return false;

    self->offset = offset;

    return true;
}

isize system_get_page_size(isize pages)
{
    return system_get_page_size_impl(pages);
}

Mem_Result system_reserve(isize pages)
{
    return system_reserve_impl(pages);
}

void system_release(pntr value)
{
    system_release_impl(value);
}

File_Result file_create_if_new(String_UTF8* filename, Mem_Arena* arena)
{
    File_Result result;
    File_Impl   impl;

    result.error = file_create_if_new_impl(&impl, filename, arena);

    if (result.error != FILE_ERROR_NONE) return result;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    Mem_Result mem = arena_push(arena, bytes, align);

    if (mem.error == MEM_ERROR_NONE) {
        for (isize i = 0; i < mem.length; i += 1)
            mem.memory[i] = ((u8*)(&impl))[i];

        result.value = mem.memory;

        return result;
    }

    file_close_impl(&impl);

    result.error = FILE_ERROR_ARENA_OUT_OF_MEMORY;

    return result;
}

File_Result file_create_always(String_UTF8* filename, Mem_Arena* arena)
{
    File_Result result;
    File_Impl   impl;

    result.error = file_create_always_impl(&impl, filename, arena);

    if (result.error != FILE_ERROR_NONE) return result;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    Mem_Result mem = arena_push(arena, bytes, align);

    if (mem.error == MEM_ERROR_NONE) {
        for (isize i = 0; i < mem.length; i += 1)
            mem.memory[i] = ((u8*)(&impl))[i];

        result.value = mem.memory;

        return result;
    }

    file_close_impl(&impl);

    result.error = FILE_ERROR_ARENA_OUT_OF_MEMORY;

    return result;
}

File_Result file_open(String_UTF8* filename, Mem_Arena* arena)
{
    File_Result result;
    File_Impl   impl;

    result.error = file_open_impl(&impl, filename, arena);

    if (result.error != FILE_ERROR_NONE) return result;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    Mem_Result mem = arena_push(arena, bytes, align);

    if (mem.error == MEM_ERROR_NONE) {
        for (isize i = 0; i < mem.length; i += 1)
            mem.memory[i] = ((u8*)(&impl))[i];

        result.value = mem.memory;

        return result;
    }

    file_close_impl(&impl);

    result.error = FILE_ERROR_ARENA_OUT_OF_MEMORY;

    return result;
}

void file_close(File_Handle* self)
{
    file_close_impl((File_Impl*)(self));
}

}
