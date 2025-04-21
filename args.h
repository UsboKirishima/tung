#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ARGS_NUM 10

struct ttts_cmd_arg {
        char short_flag[3];         /* Short option like "-h" */
        const char *long_flag;      /* Long option like "--help" */
        const char *description;    /* Description for help messages */

        /* Argument Value */
        bool expects_value;         /* Does it expect a value? */
        const char *value_name;     /* Name shown in help (e.g., FILE, PATH) */
        const char *default_value;  /* Default value if not provided */

        bool is_required;           /* Is this option mandatory? */
        bool is_repeatable;         /* Can this option be used multiple times? */
};

struct ttts_cmd_arg_dict_item {
        uint16_t index;
        struct ttts_cmd_arg arg;
};

char *args_extract_value(int argc, char **argv, struct ttts_cmd_arg arg_tf);
bool args_line_contains_arg(int argc, char **argv, struct ttts_cmd_arg arg_tf);
void args_parse_full_buffer(int _argc, char **_argv);

#endif /* ARGS_H */