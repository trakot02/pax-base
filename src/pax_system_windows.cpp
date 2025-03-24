#include "pax_system.hpp"

#define NOMINMAX
#define NOGDI
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace pax {

//
// Types
//

struct File_Impl { HANDLE handle; };

//
// Procs
//

isize system_get_page_size_impl()
{
    SYSTEM_INFO info = {};

    GetSystemInfo(&info);

    return info.dwPageSize;
}

Mem_Block system_reserve_impl(isize pages)
{
    Mem_Block result = {};

    DWORD length = pages * system_get_page_size();

    if (pages <= 0) return result;

    LPVOID memory = VirtualAlloc(0, length,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (memory == 0) return result;

    result.memory = (u8*)(memory);
    result.length = length;

    return result;
}

void system_release_impl(Mem_Block block)
{
    VirtualFree(block.memory, 0, MEM_RELEASE);
}

File_Error file_create_impl(File_Impl* self, String_8 filename, Mem_Arena* arena)
{
    String_16 result = {};

    DWORD access = GENERIC_READ | GENERIC_WRITE;
    DWORD action = CREATE_NEW;
    DWORD share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str8_to_utf16(filename, &result, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    HANDLE handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (handle != INVALID_HANDLE_VALUE) {
        self->handle = handle;

        return FILE_ERROR_NONE;
    }

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_PATH_EXISTS;
    }

    return FILE_ERROR_UNKNOWN;
}

File_Error file_create_always_impl(File_Impl* self, String_8 filename, Mem_Arena* arena)
{
    String_16 result = {};

    DWORD access = GENERIC_READ | GENERIC_WRITE;
    DWORD action = CREATE_ALWAYS;
    DWORD share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str8_to_utf16(filename, &result, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    HANDLE handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (handle != INVALID_HANDLE_VALUE) {
        self->handle = handle;

        return FILE_ERROR_NONE;
    }

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_NONE;
    }

    return FILE_ERROR_UNKNOWN;
}

File_Error file_open_to_read_impl(File_Impl* self, String_8 filename, Mem_Arena* arena)
{
    String_16 result = {};

    DWORD access = GENERIC_READ;
    DWORD action = OPEN_EXISTING;
    DWORD share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str8_to_utf16(filename, &result, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    HANDLE handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (handle != INVALID_HANDLE_VALUE) {
        self->handle = handle;

        return FILE_ERROR_NONE;
    }

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_NONE;
    }

    return FILE_ERROR_UNKNOWN;
}

File_Error file_open_to_write_impl(File_Impl* self, String_8 filename, Mem_Arena* arena)
{
    String_16 result = {};

    DWORD access = GENERIC_WRITE;
    DWORD action = OPEN_EXISTING;
    DWORD share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str8_to_utf16(filename, &result, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    HANDLE handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (handle != INVALID_HANDLE_VALUE) {
        self->handle = handle;

        return FILE_ERROR_NONE;
    }

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_NONE;
    }

    return FILE_ERROR_UNKNOWN;
}

void file_close_impl(File_Impl* self)
{
    if (self->handle != INVALID_HANDLE_VALUE)
        CloseHandle(self->handle);

    self->handle = 0;
}

File_Result file_read_impl(File_Impl* self, Mem_Block* block)
{
    File_Result result = {};

    DWORD bytes = 0;

    if (block->memory == 0 || block->length <= 0)
        return result;

    BOOL state = ReadFile(self->handle, block->memory,
        block->length, &bytes, 0);

    if (bytes <= block->length && state != 0) {
        result.bytes = bytes;

        return result;
    }

    switch (GetLastError()) {
        case ERROR_HANDLE_EOF: { result.bytes = bytes; } break;

        default: { result.error = FILE_ERROR_UNKNOWN; } break;
    }

    return result;
}

File_Result file_seek_impl(File_Impl* self, isize offset, File_Origin origin)
{
    File_Result result = {};

    DWORD method = 0;

    switch (origin) {
        case FILE_ORIGIN_BEGIN:  { method = FILE_BEGIN;   } break;
        case FILE_ORIGIN_CURSOR: { method = FILE_CURRENT; } break;
        case FILE_ORIGIN_END:    { method = FILE_END;     } break;

        default: { result.error = FILE_ERROR_ORIGIN_INVALID; } break;
    }

    if (result.error != FILE_ERROR_NONE) return result;

    LARGE_INTEGER distance = {};
    LARGE_INTEGER position = {};

    distance.QuadPart = offset;

    i32 state = SetFilePointerEx(self->handle, distance,
        &position, method);

    if (state != 0) result.bytes = position.QuadPart;
    else            result.error = FILE_ERROR_UNKNOWN;

    return result;
}

} // namespace pax
