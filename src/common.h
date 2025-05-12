#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/// @brief Name of current file.
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
/// @brief Print message to selected file.
#define _msg(file, format, ...) \
    fprintf(file, "[%s:%d] " format "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
/// @brief Print message to stderr.
#define msg_err(format, ...) _msg(stderr, format, ##__VA_ARGS__)
/// @brief Print message to stdout.
#define msg_out(format, ...) _msg(stdout, format, ##__VA_ARGS__)

#define UNUSED __attribute__((unused))

/// @brief List of application errors.
typedef enum {
    /// @brief Success.
    ZP_ERR_OK = 0,
    /// @brief Not implemented.
    ZP_ERR_NOT_IMPLEMENTED,
    /// @brief Invalid number of arguments.
    ZP_ERR_INVALID_ARG_NUM,
    /// @brief Invalid argument value.
    ZP_ERR_INVALID_ARG_VALUE,
    /// @brief File not found.
    ZP_ERR_FILE_NOT_FOUND,
    /// @brief File already exists - overwrites are disallowed.
    ZP_ERR_FILE_EXISTS,
    /// @brief File is too large.
    ZP_ERR_FILE_TOO_LARGE,
    /// @brief File failed to close.
    ZP_ERR_FILE_FAILED_TO_CLOSE,
    /// @brief I/O error encountered on file operation.
    ZP_ERR_FILE_IO_ERROR,
    /// @brief Failed to allocate memory.
    ZP_ERR_ALLOC_FAIL,
    /// @brief Null pointer encountered.
    ZP_ERR_NULL_PTR,
    /// @brief Non-null out parameter provided.
    ZP_ERR_NONNULL_OUT_PARAM,
    /// @brief Unknown size of patched content.
    ZP_ERR_OUTPUT_SIZE_UNKNOWN,
    /// @brief Failed to get patched content size.
    ZP_ERR_OUTPUT_SIZE_ERROR,
    /// @brief Patched content is too large.
    ZP_ERR_OUTPUT_TOO_LARGE,
    /// @brief Other error encountered.
    ZP_ERR_OTHER
} ZP_ERR;

/// @brief Return on non-OK error code.
#define CHECK_ERR(code)          \
    do {                         \
        if (code != ZP_ERR_OK) { \
            return code;         \
        }                        \
    } while (0)

/// @brief Return and log on null.
#define CHECK_NULL(name)                            \
    do {                                            \
        if (name == NULL) {                         \
            msg_err("Encountered null: %s", #name); \
            return ZP_ERR_NULL_PTR;                 \
        }                                           \
    } while (0)

/// @brief Return on non-null out parameter.
#define CHECK_NONNULL_OUT(name)                              \
    do {                                                     \
        if (name != NULL) {                                  \
            msg_err("Out parameter is not null: %s", #name); \
            return ZP_ERR_NONNULL_OUT_PARAM;                 \
        }                                                    \
    } while (0)

/// @brief Return on file not found.
#define CHECK_FILE_NOT_FOUND(path)               \
    do {                                         \
        if (!file_exist(path)) {                 \
            msg_err("File not found: %s", path); \
            return ZP_ERR_FILE_NOT_FOUND;        \
        }                                        \
    } while (0)

/// @brief Return on file existing.
#define CHECK_FILE_EXISTS(path)                       \
    do {                                              \
        if (file_exist(path)) {                       \
            msg_err("File already exists: %s", path); \
            return ZP_ERR_FILE_EXISTS;                \
        }                                             \
    } while (0)
