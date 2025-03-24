#ifndef PAX_SYSTEM_HPP
#define PAX_SYSTEM_HPP

#include "pax_defs.hpp"
#include "pax_base.hpp"

namespace pax {

//
// Types
//

typedef enum {
    FILE_ORIGIN_BEGIN,
    FILE_ORIGIN_CURSOR,
    FILE_ORIGIN_END,
} File_Origin;

typedef enum {
    FILE_ERROR_NONE,
    FILE_ERROR_UNKNOWN,
    FILE_ERROR_ORIGIN_INVALID,
    FILE_ERROR_ARENA_IS_FULL,
    FILE_ERROR_PATH_ENCODING,
    FILE_ERROR_PATH_INVALID,
    FILE_ERROR_PATH_EXISTS,
} File_Error;

typedef struct {
    File_Error error;
    isize      bytes;
} File_Result;

typedef ptr File_Handle;

//
// Values
//

static const String_8 FILE_ERROR_NAMES[] = {
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_NONE)),
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_UNKNOWN)),
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_ORIGIN_INVALID)),
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_ARENA_IS_FULL)),
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_PATH_ENCODING)),
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_PATH_INVALID)),
    PAX_STR_8(PAX_TO_STRING(FILE_ERROR_ALREADY_EXISTS)),
};

//
// Procs
//

/* Memory */

isize system_get_page_size();

Mem_Block system_reserve(isize pages);

void system_release(Mem_Block block);

/* File */

File_Error file_create(File_Handle* self, String_8 filename, Mem_Arena* arena);

File_Error file_create_always(File_Handle* self, String_8 filename, Mem_Arena* arena);

File_Error file_open_to_read(File_Handle* self, String_8 filename, Mem_Arena* arena);

File_Error file_open_to_write(File_Handle* self, String_8 filename, Mem_Arena* arena);

void file_close(File_Handle* self);

File_Result file_read(File_Handle* self, Mem_Block* block);

File_Result file_seek(File_Handle* self, isize offset, File_Origin origin);

} // namespace pax

#endif // PAX_SYSTEM_HPP
