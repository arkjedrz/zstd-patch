#pragma once

#include <stdio.h>

#include "common.h"

/// @brief Check if file exists.
/// @param file_path [in] Path to file.
/// @return          True if file exists.
bool file_exist(const char* file_path);

/// @brief Get file size.
/// @param file_path [in]  Path to file.
/// @param file_size [out] File size.
/// @return          Error code.
ZP_ERR file_size(const char* file_path, size_t* file_size);

/// @brief Open file.
/// @param file_path [in]  Path to file.
/// @param mode      [in]  Mode.
/// @param fd        [out] File descriptor.
/// @return          Error code.
ZP_ERR file_open(const char* file_path, const char* mode, FILE** fd);

/// @brief Close file.
/// @param fd [in/out] Path to file. Set to NULL.
/// @return   Error code.
ZP_ERR file_close(FILE** fd);

/// @brief Load content of a file to a buffer.
/// @param fd       [in]  File descriptor.
/// @param buf      [out] Buffer to use.
/// @param buf_size [in]  Buffer size.
/// @return         Error code.
ZP_ERR file_load(FILE* fd, void* buf, const size_t buf_size);

/// @brief Save buffer to a file.
/// @param fd       [in] File descriptor.
/// @param buf      [in] Buffer to use.
/// @param buf_size [in] Buffer size.
/// @return         Error code.
ZP_ERR file_save(FILE* fd, void* buf, const size_t buf_size);
