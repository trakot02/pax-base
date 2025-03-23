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

    if (pages <= 0) return result;

    isize page   = system_get_page_size();
    isize length = page * pages;

    u8* memory = (u8*) VirtualAlloc(0, length,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (memory == 0) return result;

    result.memory = memory;
    result.length = length;

    return result;
}

void system_release_impl(Mem_Block block)
{
    VirtualFree(block.memory, 0, MEM_RELEASE);
}

File_Error file_create_if_new_impl(File_Impl* self, String_8* filename, Mem_Arena* arena)
{
    String_16 result = {};

    i32 access = GENERIC_READ | GENERIC_WRITE;
    i32 action = CREATE_NEW;
    i32 share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str16_from_utf8(&result, filename, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    self->handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (self->handle != INVALID_HANDLE_VALUE)
        return FILE_ERROR_NONE;

    self->handle = 0;

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_ALREADY_EXISTS;
    }

    return FILE_ERROR_UNKNOWN;
}

File_Error file_create_always_impl(File_Impl* self, String_8* filename, Mem_Arena* arena)
{
    String_16 result = {};

    i32 access = GENERIC_READ | GENERIC_WRITE;
    i32 action = CREATE_ALWAYS;
    i32 share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str16_from_utf8(&result, filename, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    self->handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (self->handle != INVALID_HANDLE_VALUE)
        return FILE_ERROR_NONE;

    self->handle = 0;

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_ALREADY_EXISTS;
    }

    return FILE_ERROR_UNKNOWN;
}

File_Error file_open_if_exists_impl(File_Impl* self, String_8* filename, Mem_Arena* arena)
{
    String_16 result = {};

    i32 access = GENERIC_READ | GENERIC_WRITE;
    i32 action = OPEN_EXISTING;
    i32 share  = FILE_SHARE_READ;

    isize marker = arena->offset;

    if (str16_from_utf8(&result, filename, arena) == false)
        return FILE_ERROR_PATH_ENCODING;

    self->handle = CreateFileW((wchar_t*)(result.memory),
        access, share, 0, action, FILE_ATTRIBUTE_NORMAL, 0);

    arena_pop(arena, marker);

    if (self->handle != INVALID_HANDLE_VALUE)
        return FILE_ERROR_NONE;

    self->handle = 0;

    switch (GetLastError()) {
        case ERROR_PATH_NOT_FOUND: return FILE_ERROR_PATH_INVALID;
        case ERROR_FILE_EXISTS:    return FILE_ERROR_ALREADY_EXISTS;
    }

    return FILE_ERROR_UNKNOWN;
}

void file_close_impl(File_Impl* self)
{
    if (self->handle != INVALID_HANDLE_VALUE)
        CloseHandle(self->handle);

    self->handle = 0;
}

} // namespace pax
