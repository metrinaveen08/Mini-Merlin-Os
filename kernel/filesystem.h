#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../libc/mem.h"

#define FS_MAX_FILES  16
#define FS_NAME_LEN   32
#define FS_DATA_LEN   256

typedef struct {
    char name[FS_NAME_LEN];
    char data[FS_DATA_LEN];
    int  used;
} fs_file_t;

void fs_init();
void cmd_ls();
void cmd_touch(char *name);
void cmd_cat(char *name);
void cmd_rm(char *name);
void cmd_cp(char *src, char *dst);

#endif
