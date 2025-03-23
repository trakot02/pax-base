#include <stdio.h>

#include "pax_defs.hpp"
#include "pax_base.hpp"
#include "pax_system.hpp"

using namespace pax;

int main()
{
    Mem_Arena   arena = {};
    File_Handle file  = {};

    arena_init(&arena, system_reserve(1));

    String_8 filename = PAX_STR8("./😀.txt");

    File_Error error = file_create_if_new(&file, &filename, &arena);

    if (error != FILE_ERROR_NONE)
        printf("Unable to create file %s '%s'...\n", FILE_ERROR_NAMES[error], filename.memory);
    else
        printf("Created file %p '%s'...\n", file, filename.memory);
}
