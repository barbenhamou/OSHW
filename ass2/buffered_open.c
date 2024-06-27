#include "buffered_open.h"

last_op op = FIRST;

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    buffered_file_t* bf = (buffered_file_t*)malloc(sizeof(buffered_file_t));
    if (bf == 0) {
        return 0;
    }
    
    bf->read_buffer = (char*)malloc(BUFFER_SIZE);
    if (bf->read_buffer == 0) {
        free(bf);
        return 0;
    }

    bf->write_buffer = (char*)malloc(BUFFER_SIZE);
    if (bf->write_buffer == 0) {
        free(bf->read_buffer);
        free(bf);
        return 0;
    }

    bf->write_buffer_pos = 0;
    bf->read_buffer_pos = 0;

    bf->flags = flags & (~O_PREAPPEND);

    if (flags & O_PREAPPEND) {
        bf->preappend = 1;
    } else {
        bf->preappend = 0;
    }

    mode_t perm = 0;

    if (flags & O_CREAT) {
        va_list per;
        va_start(per, flags);
        perm = va_arg(per, mode_t);
        va_end(per);
    }

    bf->fd = open(pathname, flags, perm);

    if (bf->fd == -1) {
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return -1;
    }

    return bf;
}

ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) {
    size_t remainder = bf->write_buffer_size - bf->write_buffer_pos;
    
    if (remainder < count) {
        do {
            strncat(bf->write_buffer, buf, remainder);
            buffered_flush(bf);
            remainder = bf->write_buffer_size - bf->write_buffer_pos;
            count -= remainder;
        } while (remainder < count);        

        strncat(bf->write_buffer, buf, count);
        bf->write_buffer_pos = count;
    } else {
        srncat(bf->write_buffer, buf, count);
        bf->write_buffer_pos = count;
    }
}


int buffered_flush(buffered_file_t *bf) {
    if (write(bf->fd, bf->write_buffer, bf->write_buffer_size) == -1) {
        perror("write failed");
        return -1;
    }

    memset(bf->write_buffer, 0, bf->write_buffer_size);
    bf->write_buffer_pos = 0;
    return 0;
}


