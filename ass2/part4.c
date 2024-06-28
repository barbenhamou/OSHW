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



    while ((opt = getopt(argc, argv, "lp")) != -1) {
        switch (opt) {
            case 'l':
                copy_symlinks = 1;
                break;
            case 'p':
                copy_permissions = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (optind + 2 != argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *src_dir = argv[optind];
    const char *dest_dir = argv[optind + 1];

    copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

    return 0;
}