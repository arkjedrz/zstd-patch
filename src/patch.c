#include "patch.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <zstd.h>

ZP_ERR patch_create(const void* from_buf, const size_t from_buf_size, const void* to_buf,
                    const size_t to_buf_size, void** patch_buf, size_t* patch_buf_size) {
    CHECK_NULL(from_buf);
    CHECK_NULL(patch_buf);
    CHECK_NULL(to_buf);
    CHECK_NONNULL_OUT(*patch_buf);
    CHECK_NULL(patch_buf_size);

    // Create compression context.
    ZSTD_CCtx* cctx = ZSTD_createCCtx();

    // Determine maximum patch size.
    size_t max_patch_size = ZSTD_compressBound(from_buf_size);
    if (ZSTD_isError(max_patch_size) != 0) {
        msg_err("%s", ZSTD_getErrorString(max_patch_size));
        return ZP_ERR_OUTPUT_SIZE_ERROR;
    }

    // Allocate patch buffer.
    void* new_buf = malloc(max_patch_size);

    // Run compression.
    size_t patch_size = ZSTD_compress_usingDict(cctx, new_buf, max_patch_size, to_buf, to_buf_size,
                                                from_buf, from_buf_size, ZSTD_btultra2);
    if (ZSTD_isError(patch_size) != 0) {
        msg_err("%s", ZSTD_getErrorString(patch_size));
        return ZP_ERR_OTHER;
    }
    if (patch_size > max_patch_size) {
        msg_err("Output (%ld) is larger than capacity (%ld)", patch_size, max_patch_size);
        return ZP_ERR_OUTPUT_TOO_LARGE;
    }

    // Clean-up.
    ZSTD_freeCCtx(cctx);

    // Set output parameters.
    *patch_buf = new_buf;
    *patch_buf_size = patch_size;

    return ZP_ERR_OK;
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
