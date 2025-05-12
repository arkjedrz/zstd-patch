#include "patch.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <zstd.h>

ZP_ERR patch_create(UNUSED const void* from_buf, UNUSED const size_t from_buf_size,
                    UNUSED const void* to_buf, UNUSED const size_t to_buf_size,
                    UNUSED void** patch_buf, UNUSED size_t* patch_buf_size) {
    // TODO: recreate patch creation.
    return ZP_ERR_NOT_IMPLEMENTED;
}

ZP_ERR patch_apply(const void* from_buf, const size_t from_size, const void* patch_buf,
                   const size_t patch_size, void** to_buf, size_t* to_buf_size) {
    CHECK_NULL(from_buf);
    CHECK_NULL(patch_buf);
    CHECK_NULL(to_buf);
    CHECK_NONNULL_OUT(*to_buf);
    CHECK_NULL(to_buf_size);

    // Create decompression context.
    ZSTD_DCtx* dctx = ZSTD_createDCtx();

    // Determine expected output size.
    unsigned long long expected_to_size_ull = ZSTD_getFrameContentSize(patch_buf, patch_size);
    size_t expected_to_size_ul = (size_t)expected_to_size_ull;
    if (expected_to_size_ull == ZSTD_CONTENTSIZE_ERROR) {
        msg_err("Failed to get patched content size");
        return ZP_ERR_OUTPUT_SIZE_ERROR;
    } else if (expected_to_size_ull == ZSTD_CONTENTSIZE_UNKNOWN) {
        msg_err("Unknown size of patched content");
        return ZP_ERR_OUTPUT_SIZE_UNKNOWN;
    } else if (expected_to_size_ull > (unsigned long long)expected_to_size_ul) {
        msg_err("Patched content is too large: %llu", expected_to_size_ull);
        return ZP_ERR_OUTPUT_TOO_LARGE;
    }

    // Allocate output buffer.
    void* new_buf = malloc(expected_to_size_ul);

    // Run decompression.
    // Patch is considered a source and source is used as dictionary.
    // TODO: apply max memory parameter from zstd.
    size_t decomp_size = ZSTD_decompress_usingDict(dctx, new_buf, expected_to_size_ul, patch_buf,
                                                   patch_size, from_buf, from_size);
    if (decomp_size > expected_to_size_ul) {
        msg_err("Output (%ld) is larger than capacity (%ld)", decomp_size, expected_to_size_ul);
        return ZP_ERR_OUTPUT_TOO_LARGE;
    }

    // Clean-up.
    ZSTD_freeDCtx(dctx);

    // Set output parameter.
    *to_buf = new_buf;
    *to_buf_size = decomp_size;

    return ZP_ERR_OK;
}
