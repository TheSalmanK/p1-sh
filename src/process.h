#ifndef __cs361_process__
#define __cs361_process__

// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe

// Check if the command is in the ./bin/ directory
int is_bin_command (const char *command);

// Execute a command
void execute_ls_command (char **tokens);
void execute_head_command (char **tokens);
void execute_cut_command (char **tokens);
void execute_command (char **tokens);
void execute_env_command (char **tokens);

#endif
