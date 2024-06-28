#include "buffered_open.h"

last_op op = FIRST;

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    buffered_file_t* bf = (buffered_file_t*)malloc(sizeof(buffered_file_t));
    if (bf == 0) {
        perror("malloc failed");
        return 0;
    }
    
    bf->read_buffer = (char*)calloc(BUFFER_SIZE, 1);
    if (bf->read_buffer == 0) {
        perror("malloc failed");
        free(bf);
        return 0;
    }

    bf->write_buffer = (char*)calloc(BUFFER_SIZE, 1);
    if (bf->write_buffer == 0) {
        perror("malloc failed");
        free(bf->read_buffer);
        free(bf);
        return 0;
    }

    bf->write_buffer_size = BUFFER_SIZE;
    bf->read_buffer_size = BUFFER_SIZE;


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
        perror("open failed");
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return -1;
    }

    return bf;
}

ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) {
    if (bf->preappend) {
        char* temp_buf = (char*)malloc(bf->write_buffer_pos);
        if (temp_buf == 0) {
            perror("malloc failed");
            buffered_close(bf);
            return -1;
        }

        size_t left_overs = bf->write_buffer_size - bf->write_buffer_pos;

        if (left_overs < count) {
            size_t off = count - left_overs;

            memcpy(temp_buf, bf->write_buffer,bf->write_buffer_pos);
            memcpy(bf->write_buffer, buf + off, left_overs);
            memcpy(bf->write_buffer + left_overs, temp_buf, bf->write_buffer_pos);
            bf->write_buffer_pos = bf->write_buffer_size;
            buffered_flush(bf);          

            size_t iter = (count - left_overs) / bf->write_buffer_size, remainder = (count - left_overs) % bf->write_buffer_size;
  
            for (size_t i = 0; i < iter; ++i) {
                memcpy(bf->write_buffer, buf + off - (i+1) * bf->write_buffer_size, bf->write_buffer_size);
                bf->write_buffer_pos = bf->write_buffer_size;
                buffered_flush(bf);
            }

            if (remainder != 0) {
                memcpy(bf->write_buffer, buf + off - iter*bf->write_buffer_size, remainder);
                bf->write_buffer_pos = remainder;
            }

        } else {
            memcpy(temp_buf, bf->write_buffer, bf->write_buffer_pos);
            memcpy(bf->write_buffer, buf, count);
            memcpy(bf->write_buffer + count, temp_buf, bf->write_buffer_pos);
            bf->write_buffer_pos += count;
        }
        

    } else {
        if (lseek(bf->fd, 0, SEEK_END) == -1) {
            perror("lseek failed");
            buffered_close(bf);
            return -1;
        }

        size_t left_overs = bf->write_buffer_size - bf->write_buffer_pos;
        
        if (left_overs < count) {
            strncat(bf->write_buffer, buf, left_overs);
            bf->write_buffer_pos = bf->read_buffer_size;
            buffered_flush(bf);

            size_t iter = (count - left_overs) / bf->write_buffer_size, remainder = (count - left_overs) % bf->write_buffer_size;

            for (size_t i = 0; i < iter; ++i) {
                memcpy(bf->write_buffer, buf + left_overs + i*bf->write_buffer_size, bf->write_buffer_size);
                bf->write_buffer_pos = bf->read_buffer_size;
                buffered_flush(bf);
            }

            if (remainder != 0) {
                memcpy(bf->write_buffer, buf + left_overs + iter*bf->write_buffer_size, remainder);
                bf->write_buffer_pos = remainder;
            }

        } else {
            strncat(bf->write_buffer, buf, count);
            bf->write_buffer_pos += count;
        }
    }

    op = WRITE;
    return count;
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    int off = 0;
    if (lseek(bf->fd, 0, SEEK_SET) == -1) {
        perror("lseek failed");
        buffered_close(bf);
        return -1;
    }

    if (op == WRITE) {
        buffered_flush(bf);
    }

    size_t left_overs = bf->read_buffer_pos, bytes_read = 0, total = 0;
    
    if (left_overs < count) {
        memcpy(buf, bf->read_buffer, left_overs);
        memset(bf->read_buffer, 0, bf->read_buffer_size);
        errno = 0;
        total = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

        if (bytes_read == -1) {
            perror("read failed");
            buffered_close(bf);
            return -1;
        } 

        size_t iter = (count - left_overs) / bf->read_buffer_size, remainder = (count - left_overs) % bf->read_buffer_size;

        for (size_t i = 0; i < iter; ++i) {
            memcpy(buf + left_overs + i*bf->read_buffer_size, bf->read_buffer, bf->read_buffer_size);
            errno = 0;
            memset(bf->read_buffer, 0, bf->read_buffer_size);
            bytes_read = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

            if (bytes_read == -1) {
                if (errno == EINTR) {
                    --i;
                    continue;
                }
                perror("read failed");
                buffered_close(bf);
                return -1;
            } 

            total += bytes_read;
        }

        if (remainder != 0) {
            memcpy(buf + left_overs + iter*bf->read_buffer_size, bf->read_buffer, remainder);
        }

        bf->read_buffer_pos = remainder;

    } else {
        memcpy(buf, bf->read_buffer + bf->read_buffer_pos, count);
        bf->read_buffer_pos += count;
        total = count;
    }

    op = READ;
    return total;
}

int buffered_flush(buffered_file_t *bf) {
    if (bf->preappend) {
        off_t file_size = lseek(bf->fd, 0, SEEK_END);
        if (file_size == -1) {
            perror("lseek failed");
            buffered_close(bf);
            return -1;
        }

        char* temp_buf = (char*)malloc(file_size);
        if (temp_buf == 0) {
            perror("malloc failed");
            buffered_close(bf);
            return -1;
        }

        lseek(bf->fd, 0, SEEK_SET);
        read(bf->fd, temp_buf, file_size);

        lseek(bf->fd, 0, SEEK_SET);
        write(bf->fd, bf->write_buffer, bf->write_buffer_pos);
        write(bf->fd, temp_buf, file_size);

        free(temp_buf);

    } else {
        ssize_t total = 0, current = 0;
        while (total < bf->write_buffer_pos) {
            errno = 0;
            current = write(bf->fd, bf->write_buffer + total, bf->write_buffer_pos - total);
            if (current == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("write failed");
                buffered_close(bf);
                return -1;
            }

            total += current;
        }
    }

    memset(bf->write_buffer, 0, bf->write_buffer_size);
    bf->write_buffer_pos = 0;
    return 0;
}

int buffered_close(buffered_file_t *bf) {
    if (bf->write_buffer_pos != 0) {
        buffered_flush(bf);
    }
    if (close(bf->fd) == -1) {
        perror("close failed");
        return -1;
    }

    free(bf->read_buffer);
    free(bf->write_buffer);
    free(bf);
    return 0;
}
