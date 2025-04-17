#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ARGS_NUM 4
#define DICT_EX_ARG 0

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

/* Add an argument to the argument dictionary */
int8_t args_dict_add(struct ttts_cmd_arg_dict_item *dict, const uint16_t index, struct ttts_cmd_arg arg);

/* Initialize the argument dictionary */
struct ttts_cmd_arg_dict_item *args_dict_init(void);

/* Load some predefined arguments into the dictionary */
void ttts_load_args(struct ttts_cmd_arg_dict_item *args);

/* Function to parse command-line arguments */
void parse_cmd_args(int argc, char **argv);

/* Extract the value of a specific argument */
char *extract_arg_value(int argc, char **argv, struct ttts_cmd_arg arg_tf);

/* Check if an argument is contained */
bool line_contains_arg(int argc, char **argv, struct ttts_cmd_arg arg_tf);

/* Parse and find all the arguments in the command line buffer */
void parse_full_buffer(int _argc, char **_argv);

/* Function to print help message */
void print_help(void);

#endif /* ARGS_H */

