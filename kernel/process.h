#ifndef PROCESS_H
#define PROCESS_H

#define PROC_MAX   16
#define PROC_NLEN  32

typedef struct {
    int  pid;
    int  ppid;
    char name[PROC_NLEN];
    int  alive;
} proc_t;

void proc_init();
void cmd_spawn(char *name);
void cmd_kill(int pid);
void cmd_pstree();

#endif
