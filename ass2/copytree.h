// copytree.h
#ifndef COPYTREE_H
#define COPYTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions);
void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions);

#ifdef __cplusplus
}
#endif

#endif // COPYTREE_H