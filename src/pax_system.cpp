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

File_Error file_create(File_Handle* self, String_8 filename, Mem_Arena* arena)
{
    File_Impl impl = {};

    File_Error error = file_create_impl(&impl, filename, arena);

    if (error != FILE_ERROR_NONE) return error;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    Mem_Block block = arena_push(arena, bytes, align);

    if (block.memory != 0) {
        for (isize i = 0; i < bytes; i += 1)
            block.memory[i] = PAX_BYTE_PTR(&impl)[i];

        *self = block.memory;

        return FILE_ERROR_NONE;
    }

    file_close_impl(&impl);

    return FILE_ERROR_ARENA_IS_FULL;
}

File_Error file_create_always(File_Handle* self, String_8 filename, Mem_Arena* arena)
{
    File_Impl impl = {};

    File_Error error = file_create_always_impl(&impl, filename, arena);

    if (error != FILE_ERROR_NONE) return error;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    Mem_Block block = arena_push(arena, bytes, align);

    if (block.memory != 0) {
        for (isize i = 0; i < bytes; i += 1)
            block.memory[i] = PAX_BYTE_PTR(&impl)[i];

        *self = block.memory;

        return FILE_ERROR_NONE;
    }

    file_close_impl(&impl);

    return FILE_ERROR_ARENA_IS_FULL;
}

File_Error file_open(File_Handle* self, String_8 filename, Mem_Arena* arena)
{
    File_Impl impl = {};

    File_Error error = file_open_impl(&impl, filename, arena);

    if (error != FILE_ERROR_NONE) return error;

    isize bytes = PAX_SIZE_OF(File_Impl);
    isize align = PAX_ALIGN_OF(File_Impl);

    Mem_Block block = arena_push(arena, bytes, align);

    if (block.memory != 0) {
        for (isize i = 0; i < bytes; i += 1)
            block.memory[i] = PAX_BYTE_PTR(&impl)[i];

        *self = block.memory;

        return FILE_ERROR_NONE;
    }

    file_close_impl(&impl);

    return FILE_ERROR_ARENA_IS_FULL;
}

void file_close(File_Handle* self)
{
    file_close_impl(*(File_Impl**)(self));
}

File_Result file_read(File_Handle* self, Mem_Block* block)
{
    return file_read_impl(*(File_Impl**)(self), block);
}

} // namespace pax
