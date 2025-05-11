#include "patch.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <zstd.h>

ZP_ERR patch_create(UNUSED const void* from_buf, UNUSED const size_t from_buf_size,
                    UNUSED const void* to_buf, UNUSED const size_t to_buf_size,
                    UNUSED void* patch_buf, UNUSED const size_t patch_capacity) {
    // TODO: recreate patch creation.
    return ZP_ERR_NOT_IMPLEMENTED;
}

ZP_ERR patch_apply(const void* from_buf, const size_t from_size, const void* patch_buf,
                   const size_t patch_size, void* to_buf, const size_t to_buf_capacity,
                   size_t* to_size) {
    CHECK_NULL(from_buf);
    CHECK_NULL(patch_buf);
    CHECK_NULL(to_buf);
    CHECK_NULL(to_size);

    // Create decompression context.
    ZSTD_DCtx* dctx = ZSTD_createDCtx();

    // Run decompression.
    // Patch is considered a source and source is used as dictionary.
    // TODO: apply max memory parameter from zstd.
    size_t decomp_size = ZSTD_decompress_usingDict(dctx, to_buf, to_buf_capacity, patch_buf,
                                                   patch_size, from_buf, from_size);
    if (decomp_size > to_buf_capacity) {
        msg_err("Output (%ld) is larger than capacity (%ld)", decomp_size, to_buf_capacity);
        return ZP_ERR_OTHER;
    }

    // Clean-up.
    ZSTD_freeDCtx(dctx);

    // Set output parameter.
    *to_size = decomp_size;

    return ZP_ERR_OK;
}
