#pragma once
#include <stdint.h>

#include "args.h"
#include "tung.h"

#define DICT_EX_ARG 0
#define DICT_HELP_ARG 1
#define DICT_VERSION_ARG 2
#define DICT_ATTACK_ARG 3
#define DICT_TARGET_ARG 4
#define DICT_PORT_ARG 5

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

struct ttts_cmd_arg arg_attack = {
                .short_flag = "-a",
                .long_flag = "--attack",
                .description = "Type of DoS attack to perform",
                .expects_value = true,
                .value_name = "Attack Type",
                .default_value = NULL,
                .is_required = true,
                .is_repeatable = false
};

struct ttts_cmd_arg arg_target = {
                .short_flag = "-t",
                .long_flag = "--target",
                .description = "Target IP address (e.g. 192.168.1.100)",
                .expects_value = true,
                .value_name = "Ip address",
                .default_value = NULL,
                .is_required = true,
                .is_repeatable = false
};

struct ttts_cmd_arg arg_port = {
                .short_flag = "-p",
                .long_flag = "--port",
                .description = "Target port (default: 80)",
                .expects_value = true,
                .value_name = "Port",
                .default_value = DEFAULT_PORT,
                .is_required = true,
                .is_repeatable = false
};
