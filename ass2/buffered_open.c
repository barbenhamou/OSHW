#include "buffered_open.h"

last_op op = FIRST;

buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    buffered_file_t* bf = (buffered_file_t*)malloc(sizeof(buffered_file_t));
    if (bf == 0) {
        perror("malloc failed");
        return 0;
    }
    
    bf->read_buffer = (char*)malloc(BUFFER_SIZE);
    if (bf->read_buffer == 0) {
        perror("malloc failed");
        free(bf);
        return 0;
    }

    bf->write_buffer = (char*)malloc(BUFFER_SIZE);
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
            size_t off = count + left_overs - 1;

            strcpy(temp_buf, bf->write_buffer);
            strncpy(bf->write_buffer, buf + off, left_overs);
            strcpy(bf->write_buffer + left_overs, temp_buf);
            buffered_flush(bf);          

            size_t iter = (count - left_overs) / bf->write_buffer_size, remainder = (count - left_overs) % bf->write_buffer_size;
  
            for (size_t i = 0; i < iter; ++i) {
                strncpy(bf->write_buffer, buf + off - i * bf->write_buffer_size, bf->write_buffer_size);
                buffered_flush(bf);
            }

            if (remainder != 0) {
                strncpy(bf->write_buffer, buf + off - iter*bf->write_buffer_size, remainder);
                bf->write_buffer_pos = remainder;
            }

        } else {
            strcpy(temp_buf, bf->write_buffer);
            strncpy(bf->write_buffer, buf, count);
            strcpy(bf->write_buffer + count, temp_buf);
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
            buffered_flush(bf);

            size_t iter = (count - left_overs) / bf->write_buffer_size, remainder = (count - left_overs) % bf->write_buffer_size;

            for (size_t i = 0; i < iter; ++i) {
                strncpy(bf->write_buffer, buf + left_overs + i*bf->write_buffer_size, bf->write_buffer_size);
                buffered_flush(bf);
            }

            if (remainder != 0) {
                strncpy(bf->write_buffer, buf + left_overs + iter*bf->write_buffer_size, remainder);
                bf->write_buffer_pos = remainder;
            }

        } else {
            strncat(bf->write_buffer, buf, count);
            bf->write_buffer_pos += count;
        }
    }

    op = WRITE;
    return 0;
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    if (op == WRITE) {
        buffered_flush(bf);
    }

    size_t left_overs = bf->read_buffer_pos, bytes_read, total;
    
    if (left_overs < count) {
        strncpy(buf, bf->read_buffer, left_overs);
        errno = 0;
        total = read(bf->fd, bf->read_buffer, bf->read_buffer_size);

        if (bytes_read == -1) {
            perror("read failed");
            buffered_close(bf);
            return -1;
        } 

        size_t iter = (count - left_overs) / bf->read_buffer_size, remainder = (count - left_overs) % bf->read_buffer_size;

        for (size_t i = 0; i < iter; ++i) {
            strncpy(buf + left_overs + i*bf->read_buffer_size, bf->read_buffer, bf->read_buffer_size);
            errno = 0;
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
            strncpy(buf + left_overs + iter*bf->read_buffer_size, bf->read_buffer, bf->read_buffer_size);
        }

        bf->read_buffer_pos = remainder;

    } else {
        strncpy(buf, bf->read_buffer, count);
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
        while (total < bf->write_buffer_size) {
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
    buffered_flush(bf);
    if (close(bf->fd) == -1) {
        perror("close failed");
        return -1;
    }

    free(bf->read_buffer);
    free(bf->write_buffer);
    free(bf);
    return 0;
}

int main() {
    buffered_file_t *bf = buffered_open("example.txt", O_WRONLY | O_CREAT , 0644);
    if (!bf) {
        perror("buffered_open");
        return 1;
    }

    const char *text = "Hello, World!";
    if (buffered_write(bf, text, strlen(text)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }

    const char *text1 = "Hello1, World!";

    if (buffered_write(bf, text1, strlen(text1)) == -1) {
        perror("buffered_write");
        buffered_close(bf);
        return 1;
    }

    if (buffered_close(bf) == -1) {
        perror("buffered_close");
        return 1;
    }

    return 0;
}
