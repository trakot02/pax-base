#include "pax_base.hpp"

#if PAX_SYSTEM == PAX_SYSTEM_WINDOWS

    #include "pax_system_windows.cpp"

#endif

namespace pax {

isize system_get_page_size()
{
    return system_get_page_size_impl();
}

Mem_Block system_reserve(isize pages)
{
    return system_reserve_impl(pages);
}

void system_release(Mem_Block block)
{
    system_release_impl(block);
}

File_Error file_create_if_new(File_Handle* self, String_8* filename, Mem_Arena* arena)
{
    File_Impl impl = {};

    File_Error error = file_create_if_new_impl(&impl, filename, arena);

    if (error != FILE_ERROR_NONE) return error;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    u8* memory = (u8*) arena_push(arena, bytes, align);

    *self = memory;

    if (memory != 0) {
        for (isize i = 0; i < bytes; i += 1)
            memory[i] = ((u8*)(&impl))[i];

        return FILE_ERROR_NONE;
    }

    file_close_impl(&impl);

    return FILE_ERROR_ARENA_IS_FULL;
}

File_Error file_create_always(File_Handle* self, String_8* filename, Mem_Arena* arena)
{
    File_Impl impl = {};

    File_Error error = file_create_always_impl(&impl, filename, arena);

    if (error != FILE_ERROR_NONE) return error;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    u8* memory = (u8*) arena_push(arena, bytes, align);

    *self = memory;

    if (memory != 0) {
        for (isize i = 0; i < bytes; i += 1)
            memory[i] = ((u8*)(&impl))[i];

        return FILE_ERROR_NONE;
    }

    file_close_impl(&impl);

    return FILE_ERROR_ARENA_IS_FULL;
}

File_Error file_open_if_exists(File_Handle* self, String_8* filename, Mem_Arena* arena)
{
    File_Impl impl = {};

    File_Error error = file_open_if_exists_impl(&impl, filename, arena);

    if (error != FILE_ERROR_NONE) return error;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    u8* memory = (u8*) arena_push(arena, bytes, align);

    *self = memory;

    if (memory != 0) {
        for (isize i = 0; i < bytes; i += 1)
            memory[i] = ((u8*)(&impl))[i];

        return FILE_ERROR_NONE;
    }

    file_close_impl(&impl);

    return FILE_ERROR_ARENA_IS_FULL;
}

void file_close(File_Handle* self)
{
    file_close_impl((File_Impl*)(self));
}

} // namespace pax
