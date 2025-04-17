#pragma once
#include <stdint.h>

#include "args.h"

#define DICT_EX_ARG 0
#define DICT_HELP_ARG 1
#define DICT_VERSION_ARG 2

struct ttts_cmd_arg arg_help = {
                .short_flag = "-h",
                .long_flag = "--help",
                .description = "Show the help menu",
                .expects_value = false,
                .value_name = NULL,
                .default_value = NULL,
                .is_required = false,
                .is_repeatable = false
};

struct ttts_cmd_arg arg_version = {
                .short_flag = "-v",
                .long_flag = "--version",
                .description = "Show the current version",
                .expects_value = false,
                .value_name = NULL,
                .default_value = NULL,
                .is_required = false,
                .is_repeatable = false
};
