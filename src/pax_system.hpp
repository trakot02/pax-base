#ifndef PAX_SYSTEM_HPP
#define PAX_SYSTEM_HPP

#include "pax_defs.hpp"
#include "pax_base.hpp"

namespace pax {

//
// Values
//

static const u8* const FILE_ERROR_NAMES[] = {
    (const u8*) PAX_TO_STRING(FILE_ERROR_NONE),
    (const u8*) PAX_TO_STRING(FILE_ERROR_UNKNOWN),
    (const u8*) PAX_TO_STRING(FILE_ERROR_ARENA_IS_FULL),
    (const u8*) PAX_TO_STRING(FILE_ERROR_PATH_ENCODING),
    (const u8*) PAX_TO_STRING(FILE_ERROR_PATH_INVALID),
    (const u8*) PAX_TO_STRING(FILE_ERROR_ALREADY_EXISTS),
};

//
// Types
//

using File_Handle = ptr;

enum File_Error {
    FILE_ERROR_NONE,
    FILE_ERROR_UNKNOWN,
    FILE_ERROR_ARENA_IS_FULL,
    FILE_ERROR_PATH_ENCODING,
    FILE_ERROR_PATH_INVALID,
    FILE_ERROR_ALREADY_EXISTS,
};

//
// Procs
//

isize system_get_page_size();

Mem_Block system_reserve(isize pages);

void system_release(Mem_Block block);

File_Error file_create_if_new(File_Handle* self, String_8* filename, Mem_Arena* arena);

File_Error file_create_always(File_Handle* self, String_8* filename, Mem_Arena* arena);

File_Error file_open_if_exists(File_Handle* self, String_8* filename, Mem_Arena* arena);

void file_close(File_Handle* self);

} // namespace pax

#endif // PAX_SYSTEM_HPP
