#ifndef _FUTURE_INIT_H
#define _FUTURE_INIT_H


extern const char *g_revision_str;
extern const char *g_main_version_str;
extern const char *compile_option;

extern char confdev[];
extern char appdev[];
extern char logdev[];



extern void fs_early_init(char *cmd_line);
extern void fs_last_init(void);

void gate_initialize(void);
void gate_initialize_final(void);
int reinit_proc(void);


#endif
