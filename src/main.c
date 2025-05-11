#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "patch.h"

/// @brief Max mode string length.
#define MAX_MODE_LEN 10
/// @brief Max path string length.
#define MAX_PATH_LEN 4096

/// @brief List of supported modes.
typedef enum {
    /// @brief Create patch, like '--patch-from'.
    ZP_MODE_CREATE,
    /// @brief Apply patch, like '--patch-apply'.
    ZP_MODE_APPLY
} ZP_MODE;

/// @brief App configuration.
typedef struct {
    ZP_MODE mode;
    char from_path[MAX_PATH_LEN];
    char to_path[MAX_PATH_LEN];
    char patch_path[MAX_PATH_LEN];
} Config;

static ZP_ERR handle_args(int argc, char** argv, Config* cfg) {
    // Check number of args.
    if (argc != 5) {
        msg_err("Invalid number of args");
        return ZP_ERR_INVALID_ARG_NUM;
    }

    // Set mode.
    const char* mode_str = argv[1];
    if (strncmp(mode_str, "create", MAX_MODE_LEN) == 0) {
        cfg->mode = ZP_MODE_CREATE;
    } else if (strncmp(mode_str, "apply", MAX_MODE_LEN) == 0) {
        cfg->mode = ZP_MODE_APPLY;
    } else {
        msg_err("Invalid mode: %s", mode_str);
        return ZP_ERR_INVALID_ARG_VALUE;
    }

    // Set paths in config.
    strncpy(cfg->from_path, argv[2], MAX_PATH_LEN);
    strncpy(cfg->patch_path, argv[3], MAX_PATH_LEN);
    strncpy(cfg->to_path, argv[4], MAX_PATH_LEN);

    return ZP_ERR_OK;
}

static void print_config(Config* cfg) {
    msg_out("%14s %-30s", "App mode:", cfg->mode == 1 ? "apply" : "create");
    msg_out("%14s %-30s", "From path:", cfg->from_path);
    msg_out("%14s %-30s", "To path:", cfg->to_path);
    msg_out("%14s %-30s", "Patch path:", cfg->patch_path);
}

/// @brief Allocate buffer with error check.
/// @param buf_size [in]  Buffer size.
/// @param buf      [out] Buffer to create.
/// @return         Error code.
static ZP_ERR alloc_buf(size_t buf_size, void** buf) {
    CHECK_NULL(buf);
    CHECK_NONNULL_OUT(*buf);

    // Allocate buffer.
    void* new_buf = calloc(buf_size, 1);

    // Return error on NULL.
    if (!new_buf) {
        msg_err("Failed to allocate buffer, size: %ld", buf_size);
        return ZP_ERR_ALLOC_FAIL;
    }

    // Set output parameter.
    *buf = new_buf;

    return ZP_ERR_OK;
}

/// @brief Open file, alloc buffer, load content, then close file.
/// @param file_path [in]  File path.
/// @param buf       [out] Buffer to use.
/// @param buf_size  [out] Buffer size.
/// @return          Error code.
static ZP_ERR load_file_to_buf(const char* file_path, void** buf, size_t* buf_size) {
    CHECK_NULL(file_path);
    CHECK_NULL(buf);
    CHECK_NONNULL_OUT(*buf);
    CHECK_NULL(buf_size);

    // Open file.
    FILE* fd = NULL;
    ZP_ERR res = file_open(file_path, "rb", &fd);
    CHECK_ERR(res);

    // Get file size.
    size_t new_size = 0;
    res = file_size(file_path, &new_size);
    CHECK_ERR(res);

    // Allocate buffer.
    void* new_buf = NULL;
    res = alloc_buf(new_size, &new_buf);
    CHECK_ERR(res);

    // Load file content.
    res = file_load(fd, new_buf, new_size);
    CHECK_ERR(res);

    // Close file.
    res = file_close(&fd);
    CHECK_ERR(res);

    // Set output parameters.
    *buf_size = new_size;
    *buf = new_buf;

    return ZP_ERR_OK;
}

/// @brief Open file, save content, then close file.
/// @param file_path [in] File path.
/// @param buf       [in] Buffer to use.
/// @param buf_size  [in] Buffer size.
/// @return          Error code.
static ZP_ERR save_file_from_buf(const char* file_path, void* buf, const size_t buf_size) {
    CHECK_NULL(file_path);
    CHECK_NULL(buf);

    // Open file.
    FILE* fd = NULL;
    ZP_ERR res = file_open(file_path, "wb", &fd);
    CHECK_ERR(res);

    // Save buffer to file.
    res = file_save(fd, buf, buf_size);
    CHECK_ERR(res);

    // Close file.
    res = file_close(&fd);
    CHECK_ERR(res);

    return ZP_ERR_OK;
}

/// @brief Apply patch based on config.
/// @param cfg [in] App config.
/// @return    Error code.
static ZP_ERR run_patch_apply(const Config* cfg) {
    CHECK_NULL(cfg);

    // Check paths.
    CHECK_FILE_NOT_FOUND(cfg->from_path);
    CHECK_FILE_NOT_FOUND(cfg->patch_path);
    CHECK_FILE_EXISTS(cfg->to_path);

    // Load file contents.
    size_t from_buf_size = 0;
    void* from_buf = NULL;
    ZP_ERR res = load_file_to_buf(cfg->from_path, &from_buf, &from_buf_size);
    CHECK_ERR(res);

    size_t patch_buf_size = 0;
    void* patch_buf = NULL;
    res = load_file_to_buf(cfg->patch_path, &patch_buf, &patch_buf_size);
    CHECK_ERR(res);

    // Allocate output buffer.
    // TODO: can this be guessed?
    const size_t kToBufferSize = 2 * 1024 * 1024;
    void* to_buf = NULL;
    res = alloc_buf(kToBufferSize, &to_buf);
    CHECK_ERR(res);

    // Apply patch.
    size_t to_size = 0;
    res = patch_apply(from_buf, from_buf_size, patch_buf, patch_buf_size, to_buf, kToBufferSize,
                      &to_size);
    CHECK_ERR(res);

    // Save output.
    res = save_file_from_buf(cfg->to_path, to_buf, to_size);
    CHECK_ERR(res);

    return ZP_ERR_OK;
}

int main(int argc, char** argv) {
    // Handle configuration.
    Config cfg;
    ZP_ERR res = handle_args(argc, argv, &cfg);
    if (res != ZP_ERR_OK) {
        return 1;
    }
    print_config(&cfg);

    // Run selected mode.
    switch (cfg.mode) {
        case ZP_MODE_CREATE:
            res = ZP_ERR_NOT_IMPLEMENTED;
            break;
        case ZP_MODE_APPLY:
            res = run_patch_apply(&cfg);
            break;
        default:
            res = ZP_ERR_INVALID_ARG_VALUE;
            break;
    }

    if (res != ZP_ERR_OK) {
        return 1;
    }
    return 0;
}