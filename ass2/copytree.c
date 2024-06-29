#include "copytree.h"

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    struct stat src_stat;
    if (lstat(src, &src_stat) == -1) {
        perror("lstat failed");
        return;
    }

    if (S_ISLNK(src_stat.st_mode) && copy_symlinks) {
        char linked_to[0x1000];
        ssize_t path_length;
        if ((path_length = readlink(src, linked_to, 0x1000)) == -1) {
            perror("readlink failed");
            return;
        }

        linked_to[path_length] = '\0';
        if (symlink(linked_to, dest) == -1) {
            ("symlink failed");
            return;
        }
        
    } else {
        int fdsrc;
        if ((fdsrc = open(src, O_RDONLY)) == -1) {
            perror("open failed");
            return;
        }

        int fddst;
        if ((fddst = open(dest, O_WRONLY | O_TRUNC | O_CREAT, src_stat.st_mode)) == -1) {
            perror("open failed");
            return;
        }

        ssize_t chunk = 0;
        char* write_buffer = (char*)malloc(0x1000);
        while ((chunk = read(fdsrc, write, 0x1000))) {
            if (write(fddst, write_buffer, chunk) != chunk) {
                close(fddst);
                close(fdsrc);
                free(write_buffer);
                perror("read failed");
                return;
            }
        }
        free(write_buffer);

        // if (chunk == -1) {
        //     perror("read failed");
        // }

        if (copy_permissions) {
            if (fchmod(fddst, src_stat.st_mode) == -1) {
                perror("fchmod failed");
            }
        }

        close(fddst);
        close(fdsrc);
    }
}

ssize_t create_dir(const char* path) {

    char* temp = (char*)calloc(0x1000, 1);
    if (temp == 0) {
        perror("calloc failed");
        return -1;
    }


    snprintf(temp, strlen(path) + 1, "%s", path);

    for (char* i = temp + 1; *i != NULL; ++i) {
        if (*i == '/') {
            *i = '\0';
            errno = 0;
            if (mkdir(temp, S_IRWXU) == -1 && errno != EEXIST) {
                perror("mkdir failed");
                free(temp);
                return -1;
            }
            *i = '/';
        }
    }

    errno = 0;
    if (mkdir(temp, S_IRWXU) == -1 && errno != EEXIST) {
        free(temp);
        perror("mkdir failed");
        return -1;
    }

    free(temp);
    return 0;
}


void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    struct stat src_stat;
    if (lstat(src, &src_stat) == -1) {
        perror("lstat failed");
        return;
    }

    if (S_ISLNK(src_stat.st_mode)) {
        if (copy_symlinks) {
            copy_file(src, dest, copy_symlinks, copy_permissions);
            return;
        } else {
            if (lstat(src, &src_stat) == -1) {
                perror("lstat failed");
                return;
            }
        }
    }

    create_dir(dest);

    DIR* dir = opendir(src);
    if (dir == NULL) {
        remove(dest);
        if (remove(dest) == -1) {
            perror("remove failed");
        }
        perror("opendir failed");
        return;        
    }   

    char* src_path = (char*)malloc(1000);
    char* dest_path = (char*)malloc(1000);

    struct dirent* entry = readdir(dir);
    while (entry != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            entry = readdir(dir);
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (entry->d_type == 10 || (lstat(src_path, &src_stat) == 0 && S_ISLNK(src_stat.st_mode))) {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        } else if (entry->d_type == 4 || (lstat(src_path, &src_stat) == 0 && S_ISDIR(src_stat.st_mode))) {
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }

        entry = readdir(dir);
    }
    
    if (closedir(dir) == -1) {
        perror("closedir failed");
        if (remove(dest) == -1) {
            perror("remove failed");
        }
        return;
    }
}