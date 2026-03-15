#include "filesystem.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"

static fs_file_t fs_table[FS_MAX_FILES];

void fs_init() {
    memory_set((u8 *)fs_table, 0, sizeof(fs_table));
    kprint("filesystem: in-memory FS initialised (16 slots)\n");
}

void cmd_ls() {
    int found = 0;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used) {
            kprint(fs_table[i].name);
            kprint("\n");
            found = 1;
        }
    }
    if (!found) kprint("(empty)\n");
}

void cmd_touch(char *name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strcmp(fs_table[i].name, name) == 0) {
            kprint("touch: file already exists\n");
            return;
        }
    }
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!fs_table[i].used) {
            strcopy(fs_table[i].name, name);
            fs_table[i].data[0] = '\0';
            fs_table[i].used = 1;
            return;
        }
    }
    kprint("touch: filesystem full\n");
}

void cmd_cat(char *name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strcmp(fs_table[i].name, name) == 0) {
            if (fs_table[i].data[0] == '\0')
                kprint("(empty file)\n");
            else {
                kprint(fs_table[i].data);
                kprint("\n");
            }
            return;
        }
    }
    kprint("cat: file not found\n");
}

void cmd_rm(char *name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strcmp(fs_table[i].name, name) == 0) {
            fs_table[i].used = 0;
            return;
        }
    }
    kprint("rm: file not found\n");
}

void cmd_cp(char *src, char *dst) {
    int src_idx = -1;
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strcmp(fs_table[i].name, src) == 0) {
            src_idx = i;
            break;
        }
    }
    if (src_idx < 0) { kprint("cp: source not found\n"); return; }
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strcmp(fs_table[i].name, dst) == 0) {
            kprint("cp: destination already exists\n");
            return;
        }
    }
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!fs_table[i].used) {
            strcopy(fs_table[i].name, dst);
            strcopy(fs_table[i].data, fs_table[src_idx].data);
            fs_table[i].used = 1;
            return;
        }
    }
    kprint("cp: filesystem full\n");
}
