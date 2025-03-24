#include <stdio.h>

#include "pax_defs.hpp"
#include "pax_base.hpp"
#include "pax_system.hpp"

using namespace pax;

int main()
{
    Mem_Arena arena = {};

    String_8    name   = PAX_STR_8("README.md");
    File_Handle handle = {};

    arena_init(&arena, system_reserve(4));

    File_Error error = file_open_to_read(&handle, name, &arena);

    if (error != FILE_ERROR_NONE) return 1;

    File_Result result = file_seek(&handle, 8, FILE_ORIGIN_BEGIN);

    if (result.error != FILE_ERROR_NONE)
        return 1;

    Mem_Block block = arena_push(&arena, 256, 1);

    block.length -= 1;

    result = file_read(&handle, &block);

    block.length += 1;

    if (result.error != FILE_ERROR_NONE)
        return 1;

    printf("'%s'\n", block.memory);
}
