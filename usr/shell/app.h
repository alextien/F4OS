#ifndef USR_SHELL_APP_H_INCLUDED
#define USR_SHELL_APP_H_INCLUDED

typedef struct command {
    char *name;
    void (*fptr)(int, char **);
} command_t;

#define DEFINE_APP(fn) command_t fn##_command __attribute__((section(".user")))= {\
        .name = #fn,        \
        .fptr = fn          \
    };

#endif
