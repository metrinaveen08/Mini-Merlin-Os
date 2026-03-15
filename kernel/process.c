#include "process.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"

static proc_t proc_table[PROC_MAX];
static int    next_pid = 2;
static int    last_pid = 1;  /* tracks "current shell" context for ppid */

void proc_init() {
    memory_set((u8 *)proc_table, 0, sizeof(proc_table));
    /* PID 1: kernel / init */
    proc_table[0].pid   = 1;
    proc_table[0].ppid  = 0;
    proc_table[0].alive = 1;
    strcopy(proc_table[0].name, "kernel");
}

void cmd_spawn(char *name) {
    for (int i = 0; i < PROC_MAX; i++) {
        if (!proc_table[i].alive) {
            int pid  = next_pid++;
            proc_table[i].pid   = pid;
            proc_table[i].ppid  = last_pid;
            proc_table[i].alive = 1;
            strcopy(proc_table[i].name, name);

            /* simulate fork() output */
            char pids[12];
            kprint("[kernel] fork() called by pid ");
            int_to_ascii(last_pid, pids);
            kprint(pids);
            kprint("\n");
            kprint("[kernel] child process spawned: pid=");
            int_to_ascii(pid, pids);
            kprint(pids);
            kprint("  name=");
            kprint(name);
            kprint("\n");

            last_pid = pid;   /* new process becomes current context */
            return;
        }
    }
    kprint("spawn: process table full\n");
}

void cmd_kill(int pid) {
    if (pid == 1) { kprint("kill: cannot kill kernel\n"); return; }
    for (int i = 0; i < PROC_MAX; i++) {
        if (proc_table[i].alive && proc_table[i].pid == pid) {
            proc_table[i].alive = 0;
            /* re-parent children to pid 1 */
            for (int j = 0; j < PROC_MAX; j++) {
                if (proc_table[j].alive && proc_table[j].ppid == pid)
                    proc_table[j].ppid = 1;
            }
            if (last_pid == pid) last_pid = 1;
            char s[12];
            kprint("killed pid ");
            int_to_ascii(pid, s);
            kprint(s);
            kprint("\n");
            return;
        }
    }
    kprint("kill: pid not found\n");
}

/* recursive helper: print all alive children of 'parent' with indent */
static void print_children(int parent, int depth) {
    for (int i = 0; i < PROC_MAX; i++) {
        if (!proc_table[i].alive) continue;
        if (proc_table[i].ppid != parent) continue;

        for (int d = 0; d < depth; d++) kprint("  ");
        kprint("+-");
        kprint(proc_table[i].name);
        kprint("(");
        char s[12];
        int_to_ascii(proc_table[i].pid, s);
        kprint(s);
        kprint(")\n");
        print_children(proc_table[i].pid, depth + 1);
    }
}

void cmd_pstree() {
    /* print PID 1 (root) then recurse */
    kprint("kernel(1)\n");
    print_children(1, 1);
}
