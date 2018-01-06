#ifndef _GAME_PLATFORM__H_
#define _GAME_PLATFORM__H_

#define RUN_COMMAND_FUNC(name) u32(name)(char *command)
typedef RUN_COMMAND_FUNC(run_command_func);

#define READ_FILE_FUNC(name) char*(name)(char *filename)
typedef READ_FILE_FUNC(read_file_func);

#define WRITE_TO_LOG_FUNC(name) void(name)(char *why)
typedef WRITE_TO_LOG_FUNC(write_to_log_func);

struct platform_code {
    run_command_func *run_command;
    read_file_func *read_file;
    write_to_log_func *write_to_log;
};

#endif
