#include "file.h"

#include <sys/stat.h>

bool file_exist(const char* file_path) {
    CHECK_NULL(file_path);

    struct stat st;
    return stat(file_path, &st) == 0;
}

ZP_ERR file_size(const char* file_path, size_t* file_size) {
    CHECK_NULL(file_path);
    CHECK_NULL(file_size);

    // Get file attributes.
    struct stat st;
    if (stat(file_path, &st) != 0) {
        msg_err("File not found: %s", file_path);
        return ZP_ERR_FILE_NOT_FOUND;
    }

    // Validate size of the file.
    long size_s = st.st_size;
    size_t size_u = (size_t)size_s;
    if (size_s < 0 || size_s != (long)size_u) {
        msg_err("File too large: %s", file_path);
        return ZP_ERR_FILE_TOO_LARGE;
    }

    // Set output parameter.
    *file_size = size_u;

    return ZP_ERR_OK;
}

ZP_ERR file_open(const char* file_path, const char* mode, FILE** fd) {
    CHECK_NULL(file_path);
    CHECK_NULL(mode);
    CHECK_NULL(fd);
    CHECK_NONNULL_OUT(*fd);

    // Open file.
    FILE* new_fd = fopen(file_path, mode);
    if (!new_fd) {
        msg_err("File not found: %s", file_path);
        return ZP_ERR_FILE_NOT_FOUND;
    }

    // Set output parameter.
    *fd = new_fd;

    return ZP_ERR_OK;
}

ZP_ERR file_close(FILE** fd) {
    CHECK_NULL(*fd);

    // Close file.
    if (fclose(*fd) != 0) {
        msg_err("Failed to close file");
        return ZP_ERR_FILE_FAILED_TO_CLOSE;
    }

    // Set output paramteter.
    *fd = NULL;

    return ZP_ERR_OK;
}

ZP_ERR file_load(FILE* fd, void* buf, const size_t buf_size) {
    CHECK_NULL(fd);
    CHECK_NULL(buf);

    // Read file content.
    fread(buf, 1, buf_size, fd);

    // Check if error happened.
    if (ferror(fd)) {
        msg_err("I/O error encountered");
        return ZP_ERR_FILE_IO_ERROR;
    }

    return ZP_ERR_OK;
}

ZP_ERR file_save(FILE* fd, void* buf, const size_t buf_size) {
    CHECK_NULL(fd);
    CHECK_NULL(buf);

    fwrite(buf, 1, buf_size, fd);

    return ZP_ERR_OK;
}
