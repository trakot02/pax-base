#include <stdio.h>

#include "pax_defs.hpp"
#include "pax_base.hpp"
#include "pax_system.hpp"

using namespace pax;

int main()
{
    Mem_Arena arena = {};

    String_8 source = PAX_STR_8("È.txt");
    String_32 target = {};

    arena_init(&arena, system_reserve(16));

    str8_to_utf32(source, &target, &arena);

    for (isize i = 0; i < target.length; i += 1)
        printf("%u\n", target.memory[i]);
}
