#include <stdio.h>

#include "pax_defs.hpp"
#include "pax_base.hpp"
#include "pax_system.hpp"

using namespace pax;

int main(int argc, const char* argv[])
{
    if (argc != 2) return 1;

    Mem_Arena   arena = {};
    File_Handle file  = {};

    arena_init(&arena, system_reserve(16));

    String_8 filename;

    str8_init(&filename, PAX_BYTE_PTR(argv[1]), 40);

    File_Error error = file_open(&file, filename, &arena);

    if (error != FILE_ERROR_NONE) {
        printf("Error: %s (%i)\n", FILE_ERROR_NAMES[error], error);
        printf("Unable to open file...\n");

        return 1;
    }

    Mem_Block block = arena_push(&arena, 2048, 1);

    block.length -= 1;

    File_Result result = file_read(&file, &block);

    block.length += 1;

    if (result.error != FILE_ERROR_NONE) {
        printf("Error: %s (%i)\n", FILE_ERROR_NAMES[error], error);
        printf("Unable to read from file...\n");

        return 1;
    }

    printf("%s", block.memory);
}
