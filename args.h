#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ARGS_NUM 4
#define DICT_EX_ARG 0

/* Structure representing a command-line argument */
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

/* Structure representing an item in the argument dictionary */
struct ttts_cmd_arg_dict_item {
    uint16_t index;              /* Index of the argument in the dictionary */
    struct ttts_cmd_arg arg;     /* The actual argument */
};

/* Function to add an argument to the argument dictionary */
int8_t args_dict_add(struct ttts_cmd_arg_dict_item *dict, const uint16_t index, struct ttts_cmd_arg arg);

/* Function to initialize the argument dictionary */
struct ttts_cmd_arg_dict_item *args_dict_init(void);

/* Function to load some predefined arguments into the dictionary */
void ttts_load_args(struct ttts_cmd_arg_dict_item *args);

/* Function to parse command-line arguments */
void parse_cmd_args(int argc, char **argv);

/* Function to print help message */
void print_help(void);

#endif /* ARGS_H */

