#pragma once

#include "common.h"

/// @brief Create patch based on old and new content.
/// @param from_buf       [in]  Old content buffer.
/// @param from_buf_size  [in]  Old content buffer size.
/// @param to_buf         [in]  New content buffer.
/// @param to_buf_size    [in]  New content buffer size.
/// @param patch_buf      [out] Patch buffer.
/// @param patch_capacity [in]  Patch buffer size.
/// @return               Error code.
ZP_ERR patch_create(const void* from_buf, const size_t from_buf_size, const void* to_buf,
                    const size_t to_buf_size, void* patch_buf, const size_t patch_capacity);

/// @brief Apply patch on old content.
/// @param from_buf        [in]  Old content buffer.
/// @param from_buf_size   [in]  Old content buffer size.
/// @param patch_buf       [in]  Patch buffer.
/// @param patch_buf_size  [in]  Patch buffer size.
/// @param to_buf          [out] New content buffer.
/// @param to_buf_capacity [in]  New content buffer size.
/// @param to_size         [out] Actual new content size.
/// @return                Error code.
ZP_ERR patch_apply(const void* from_buf, const size_t from_buf_size, const void* patch_buf,
                   const size_t patch_buf_size, void* to_buf, const size_t to_buf_capacity,
                   size_t* to_size);