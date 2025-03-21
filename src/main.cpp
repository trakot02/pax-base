#include <stdio.h>

#include "pax_base.hpp"
#include "pax_system.hpp"

using namespace pax;

u32 points[4]   = {0x1f600, 0x1f600, 0x1f600, 0x1f600};
u8  memory[256] = {};

int main()
{
    Mem_Arena arena;

    if (arena_init(&arena, system_reserve(1)) == false)
        return 1;

    String_UTF8 filename;

    if (str8_init(&filename, (u8*)("src/😀😁.txt"), 50) == false)
        return 1;

    File_Result result = file_create_always(&filename, &arena);

    if (result.error == FILE_ERROR_NONE) {
        printf("File (%u) created...\n", result.value);
    } else
        printf("Unable to create file...\n");
}
