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
        if ((path_length = readlink(src, linked_to, sizeof(linked_to) - 1)) == -1) {
            perror("readlink failed");
            return;
        }

        linked_to[path_length] = '\0';
        if (symlink(linked_to, dest) == -1) {
            perror("symlink failed");
            return;
        }
    } else {
        int fdsrc;
        if ((fdsrc = open(src, O_RDONLY)) == -1) {
            perror("open src failed");
            return;
        }

        int fddst;
        if ((fddst = open(dest, O_WRONLY | O_TRUNC | O_CREAT, src_stat.st_mode)) == -1) {
            perror("open dest failed");
            close(fdsrc);
            return;
        }

        ssize_t chunk;
        char buffer[0x1000];
        while ((chunk = read(fdsrc, buffer, sizeof(buffer))) > 0) {
            if (write(fddst, buffer, chunk) != chunk) {
                perror("write failed");
                close(fddst);
                close(fdsrc);
                return;
            }
        }
        if (chunk == -1) {
            perror("read failed");
        }

        if (copy_permissions) {
            if (fchmod(fddst, src_stat.st_mode) == -1) {
                perror("fchmod failed");
            }
        }

        close(fddst);
        close(fdsrc);
    }
}

int create_dir(const char *path) {
    char temp[0x1000];
    snprintf(temp, sizeof(temp), "%s", path);

    for (char *i = temp + 1; *i; ++i) {
        if (*i == '/') {
            *i = '\0';
            errno = 0;
            if (mkdir(temp, S_IRWXU) == -1 && errno != EEXIST) {
                perror("mkdir failed");
                return -1;
            }
            *i = '/';
        }
    }

    errno = 0;
    if (mkdir(temp, S_IRWXU) == -1 && errno != EEXIST) {
        perror("mkdir failed");
        return -1;
    }

    return 0;
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    struct stat src_stat;
    if (lstat(src, &src_stat) == -1) {
        perror("lstat failed");
        return;
    }

    if (S_ISLNK(src_stat.st_mode) && copy_symlinks) {
        copy_file(src, dest, copy_symlinks, copy_permissions);
        return;
    }

    if (create_dir(dest) == -1) {
        return;
    }

    DIR *dir = opendir(src);
    if (dir == NULL) {
        perror("opendir failed");
        return;
    }

    char src_path[0x1000];
    char dest_path[0x1000];
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (entry->d_type == DT_LNK || (lstat(src_path, &src_stat) == 0 && S_ISLNK(src_stat.st_mode))) {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        } else if (entry->d_type == DT_DIR || (lstat(src_path, &src_stat) == 0 && S_ISDIR(src_stat.st_mode))) {
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir failed");
    }
}
