#include "buffered_open.h"

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    buffered_file_t* bf = (buffered_file_t*)malloc(sizeof(buffered_file_t));
    if (bf == 0) {
        return -1;
    }
    
    bf->read_buffer = (char*)calloc(BUFFER_SIZE, 1);
    if (bf->read_buffer == 0) {
        free(bf);
        return -1;
    }

    bf->write_buffer = (char*)calloc(BUFFER_SIZE, 1);
    if (bf->write_buffer == 0) {
        free(bf->read_buffer);
        free(bf);
        return -1;
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
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return -1;
    }

    bf->OP = FIRST;

    return bf;
}

ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) {
    if (bf->preappend) {
        char* temp_buf = (char*)malloc(bf->write_buffer_pos);
        if (temp_buf == 0) {
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

    bf->OP = WRITE;
    return count;
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    int off = 0;

    if (bf->OP == WRITE) {
        buffered_flush(bf);
    }

    size_t left_overs = bf->read_buffer_size - bf->read_buffer_pos, bytes_read = 0, total = 0;
    
    if (left_overs < count && strlen(bf->read_buffer) != 0) {
        memcpy(buf, bf->read_buffer, left_overs);
        memset(bf->read_buffer, 0, bf->read_buffer_size);

        if (lseek(bf->fd, left_overs, SEEK_CUR) == -1) {
            return -1;
        }

        errno = 0;
        total = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

        if (bytes_read == -1) {
            return -1;
        } 

        size_t iter = (count - left_overs) / bf->read_buffer_size, remainder = (count - left_overs) % bf->read_buffer_size;

        for (size_t i = 0; i < iter; ++i) {
            memcpy(buf + left_overs + i * bf->read_buffer_size, bf->read_buffer, bf->read_buffer_size);
            errno = 0;
            memset(bf->read_buffer, 0, bf->read_buffer_size);
            bytes_read = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

            if (bytes_read == -1) {
                if (errno == EINTR) {
                    --i;
                    continue;
                }
                return -1;
            } 

            total += bytes_read;
        }

        if (remainder != 0) {
            memcpy(buf + left_overs + iter * bf->read_buffer_size, bf->read_buffer, remainder);
            if (lseek(bf->fd, remainder - total, SEEK_CUR) == -1) {
                return -1;
            }
        }

        bf->read_buffer_pos = remainder;

    } else if (strlen(bf->read_buffer) == 0) {
        errno = 0;
        total = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

        if (total == -1) {
            return -1;
        }

        size_t iter = count / bf->read_buffer_size, remainder = count % bf->read_buffer_size;

        for (size_t i = 0; i < iter; ++i) {
            memcpy(buf + i*bf->read_buffer_size, bf->read_buffer, bf->read_buffer_size);
            errno = 0;
            memset(bf->read_buffer, 0, bf->read_buffer_size);
            bytes_read = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

            if (bytes_read == -1) {
                if (errno == EINTR) {
                    --i;
                    continue;
                }
                return -1;
            } 

            total += bytes_read;
        }

        if (remainder != 0) {
            memcpy(buf + iter * bf->read_buffer_size, bf->read_buffer, remainder);
            if (lseek(bf->fd, remainder - total, SEEK_CUR) == -1) {
                return -1;
            }
        }

        bf->read_buffer_pos = remainder;

    } else {
        memcpy(buf, bf->read_buffer + bf->read_buffer_pos, count);
        bf->read_buffer_pos += count;
        total = count;

        if (lseek(bf->fd, total, SEEK_CUR) == -1) {
            return -1;
        }
    }

    bf->OP = READ;
    return total;
}

int buffered_flush(buffered_file_t *bf) {
    if (bf->preappend) {
        off_t file_size = lseek(bf->fd, 0, SEEK_END);
        if (file_size == -1) {
            return -1;
        }

        char* temp_buf = (char*)malloc(file_size);
        if (temp_buf == 0) {
            return -1;
        }

        if (lseek(bf->fd, 0, SEEK_SET) == -1) {
            return -1;
        }

        if (read(bf->fd, temp_buf, file_size) == -1) {
            return -1;
        }

        if (lseek(bf->fd, 0, SEEK_SET) == -1) {
            return -1;
        }

        
        if (write(bf->fd, bf->write_buffer, bf->write_buffer_pos) == -1) {
            return -1;
        }
        
        if (write(bf->fd, temp_buf, file_size) == -1) {
            return -1;
        }

        if (lseek(bf->fd, bf->write_buffer_pos, SEEK_SET) == -1) {
            return -1;
        }

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
        return -1;
    }

    free(bf->read_buffer);
    free(bf->write_buffer);
    free(bf);
    return 0;
}

int main() {
    char* buff = "hello";
    char buf[strlen(buff)];
    buffered_file_t* bf = buffered_open("hi.txt",  O_RDWR | O_PREAPPEND, 0666);
    // ssize_t bytes = buffered_write(bf, buff, strlen(buff));
    ssize_t bytes = buffered_write(bf, "w", 1);
    bytes = buffered_read(bf, buf, 3);
    printf("%s\n", buf);
}
