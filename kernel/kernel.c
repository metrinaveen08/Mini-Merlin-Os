#include "../cpu/isr.h"
#include "../cpu/timer.h"
#include "../cpu/ports.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../drivers/ata.h"
#include "filesystem.h"
#include "process.h"

#define COM1 0x3F8

static void serial_init() {
    port_byte_out(COM1 + 1, 0x00);
    port_byte_out(COM1 + 3, 0x80);
    port_byte_out(COM1 + 0, 0x03);
    port_byte_out(COM1 + 1, 0x00);
    port_byte_out(COM1 + 3, 0x03);
    port_byte_out(COM1 + 2, 0xC7);
    port_byte_out(COM1 + 4, 0x0B);
}

static int serial_ready() {
    return port_byte_in(COM1 + 5) & 0x20;
}

static void serial_write_char(char c) {
    while (serial_ready() == 0) {}
    port_byte_out(COM1, c);
}

static void serial_write_string(char *s) {
    int i = 0;
    while (s[i] != '\0') {
        serial_write_char(s[i]);
        i++;
    }
}

void kernel_main() {
    serial_init();
    serial_write_string("KERNEL: entered kernel_main()\r\n");

    isr_install();
    irq_install();
    fs_init();
    proc_init();

    clear_screen();

    kprint("Mini-Merlin-OS ready. Type 'help' to see all commands.\n ");
    kprint("This OS is made by \nMETRI NAVEEN KUMAR [24R11A6670]\nSHAIK ZAHEER ABBAS [24R11A6690]\nSAHIT KUMAR YADAV [24R11A6687]\nA.SAI ESHWAR [25R15A6610]\n ");
    kprint("It is a 4th sem Project for |Subject: Operating Systems| College: GCET |\n It is licensed under the MIT License.\n");
    kprint("[MMos]> ");
}

void user_input(char *input) {
    char argv[MAX_ARGS][64];
    int  argc = str_split(input, argv);

    if (argc == 0) {
        kprint("\n> ");
        return;
    }

    /* ---- help / what can i run ---- */
    if (strcmp(argv[0], "HELP") == 0 ||
        strcmp(input, "WHAT CAN I RUN") == 0 ||
        strcmp(input, "what can i run") == 0) {
        kprint(
            "Available commands:\n"
            "  help                   - show this list\n"
            "  DIE                    - halt the CPU\n"
            "  ATA RESET              - software-reset ATA primary drive\n"
            "  clear                  - clear the screen\n"
            "--- filesystem ---\n"
            "  ls                     - list files\n"
            "  touch <name>           - create empty file\n"
            "  cat <name>             - print file contents\n"
            "  rm <name>              - delete file\n"
            "  cp <src> <dst>         - copy file\n"
            "--- processes ---\n"
            "  spawn <name>           - fork a fake process\n"
            "  kill <pid>             - kill a process\n"
            "  pstree                 - show process tree\n"
        );

    /* ---- legacy kernel commands ---- */
    } else if (strcmp(input, "DIE") == 0) {
        kprint("Stopping the CPU\n");
        asm volatile("hlt");

    } else if (strcmp(input, "ATA RESET") == 0) {
        kprint("ATA software reset issued\n");
        ata_init();

    } else if (strcmp(argv[0], "CLEAR") == 0) {
        clear_screen();
        kprint("> ");
        return;

    /* ---- filesystem commands ---- */
    } else if (strcmp(argv[0], "LS") == 0) {
        cmd_ls();

    } else if (strcmp(argv[0], "TOUCH") == 0) {
        if (argc < 2) kprint("usage: touch <name>\n");
        else          cmd_touch(argv[1]);

    } else if (strcmp(argv[0], "CAT") == 0) {
        if (argc < 2) kprint("usage: cat <name>\n");
        else          cmd_cat(argv[1]);

    } else if (strcmp(argv[0], "RM") == 0) {
        if (argc < 2) kprint("usage: rm <name>\n");
        else          cmd_rm(argv[1]);

    } else if (strcmp(argv[0], "CP") == 0) {
        if (argc < 3) kprint("usage: cp <src> <dst>\n");
        else          cmd_cp(argv[1], argv[2]);

    /* ---- process commands ---- */
    } else if (strcmp(argv[0], "SPAWN") == 0) {
        if (argc < 2) kprint("usage: spawn <name>\n");
        else          cmd_spawn(argv[1]);

    } else if (strcmp(argv[0], "KILL") == 0) {
        if (argc < 2) { kprint("usage: kill <pid>\n"); }
        else {
            /* simple atoi */
            int pid = 0;
            char *p = argv[1];
            while (*p >= '0' && *p <= '9') pid = pid * 10 + (*p++ - '0');
            cmd_kill(pid);
        }

    } else if (strcmp(argv[0], "PSTREE") == 0) {
        cmd_pstree();

    } else {
        kprint("Command not found. Type 'help' to see all commands.\n");
    }

    kprint("\n> ");
}
