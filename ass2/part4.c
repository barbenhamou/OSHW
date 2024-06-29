#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>", prog_name);
    fprintf(stderr, "  -l: Copy symbolic links as links");
    fprintf(stderr, "  -p: Copy file permissions");
}

int main(int argc, char *argv[]) {
    int opt;
    int copy_symlinks = 0;
    int copy_permissions = 0;


    opt = getopt(argc, argv, "lp");
    while (opt != -1) {
        if (opt == 'l') {
            copy_symlinks = 1;
        } else if (opt == 'p') {
            copy_permissions = 1;
        } else {
            print_usage(argv[0]);
            return -1;
        }

        opt = getopt(argc, argv, "lp");
    }

    if (optind + 2 != argc) {
        print_usage(argv[0]);
        return -1;
    }

    const char *src_dir = argv[optind];
    const char *dest_dir = argv[optind + 1];

    copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

    return 0;
}