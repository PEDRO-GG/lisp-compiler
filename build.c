#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <unistd.h>

#define CC "gcc"

#define CODE_DIR "./src/"
#define TEST_DIR "./tests/"
#define OBJ_DIR "./obj/"

#define CFLAGS                                                      \
  "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Wunused-variable", \
      "-std=c99", "-O0", "-c", "-g"

bool ends_with(const char *name, const char *postfix) {
  // Traverse the string until a period is found
  while (*name != '\0' && *name != '.') {
    name++;
  }

  return strcmp(name, postfix) == 0;
}

void print_cmd(char *const *cmd) {
  assert(cmd != NULL);
  uint8_t i = 0;
  while (cmd[i]) {
    assert(cmd[i] != NULL);
    printf("%s ", cmd[i]);
    i++;
  }
  printf("\n");
}

void no_ext(const char *name, char *buffer, uint32_t start) {
  assert(name != NULL);
  assert(buffer != NULL);
  assert(start >= 0);

  // Copy everything to the buffer except for the extension
  while (name[start] != '\0' && name[start] != '.') {
    buffer[start] = name[start];
    start++;
  }
  buffer[start] = '\0';
}

// Assumes buffer is big enough to contain name and prefix
void prepend(char *buffer, const char *prefix, const char *name) {
  assert(buffer != NULL);
  assert(prefix != NULL);
  assert(name != NULL);

  // Append prefix
  uint32_t i = 0;
  while (prefix[i] != '\0') {
    buffer[i] = prefix[i];
    i++;
  }

  // Append name
  uint32_t j = 0;
  while (name[j] != '\0') {
    buffer[i + j] = name[j];
    j++;
  }

  buffer[i + j] = '\0';
}

void compile(char *name) {
  // Get the name without the extension
  char no_ext_buffer[100];
  no_ext(name, no_ext_buffer, 0);

  // Prepend obj/
  char outfile[100];
  prepend(outfile, OBJ_DIR, no_ext_buffer);

  // Append .o
  strcat(outfile, ".o");

  // Prepend src/
  char srcfile[100];
  prepend(srcfile, CODE_DIR, name);

  // Run the command in a child process
  pid_t pid = fork();
  if (pid == -1) {
    // Fork failed
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Child process
    char *const cmd[] = {CC, CFLAGS, srcfile, "-o", outfile, NULL};
    print_cmd(cmd);
    execvp(CC, cmd);

    // If execvp returns, an error occurred
    perror("failed to compile");
    exit(EXIT_FAILURE);
  } else {
    // Parent process
    int status;
    waitpid(pid, &status, 0);  // Wait for the child process to finish

    if (WIFEXITED(status)) {
      int exit_status = WEXITSTATUS(status);
      if (exit_status != 0) {
        printf("Child process exited with error status %d\n", exit_status);
        exit(EXIT_FAILURE);
      }
    }
  }
}

void build_exe(void) {
  // Setup the command
  char **cmd = malloc(20 * sizeof(char *));
  if (cmd == NULL) {
    printf("unable to allocate memory\n");
  }
  cmd[0] = CC;

  // Open the obj directory
  DIR *dir = opendir(OBJ_DIR);
  if (dir == NULL) {
    perror("opendir failed");
  }

  // Append each obj to the command
  int i = 1;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (ends_with(entry->d_name, ".o")) {
      // Prepend obj/
      char *outfile = malloc(100 * sizeof(char));
      prepend(outfile, OBJ_DIR, entry->d_name);
      // Append to command
      cmd[i] = outfile;
      i++;
    }
  }

  // Append the remaining flags
  cmd[i++] = "-o";
  cmd[i++] = "./bin/main";
  cmd[i++] = NULL;
  print_cmd(cmd);
  if (execvp(CC, cmd) == -1) {
    perror("failed to link");
  }
}

void run_all_tests(void) {
  printf("Running all tests...\n");
  // Logic to run all tests
}

void run_specific_test(const char *file) {
  printf("Running test in file: %s\n", file);
  // Logic to run a specific test
}

void handle_test_command(int argc, char *argv[]) {
  for (int i = 2; i < argc; ++i) {
    if (strcmp(argv[i], "-A") == 0 || strcmp(argv[i], "--all") == 0) {
      run_all_tests();
    } else if (strncmp(argv[i], "-f=", 3) == 0 ||
               strncmp(argv[i], "--file=", 7) == 0) {
      char *test_file = strchr(argv[i], '=') + 1;
      run_specific_test(test_file);
    } else {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return;
    }
  }
}

void handle_build(void) {
  // Open the source code directory
  DIR *dir = opendir(CODE_DIR);
  if (dir == NULL) {
    perror("opendir failed");
    exit(EXIT_FAILURE);
  }

  // Read and compile source code to objs
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (ends_with(entry->d_name, ".c")) {
      compile(entry->d_name);
    }
  }

  // Link all the objs to create an executable
  build_exe();

  // Close the directory
  closedir(dir);
}

void print_usage(const char *program_name) {
  printf("Usage: %s <command> [options]\n", program_name);
  printf("\nCommands and options:\n");
  printf("  test -A, --all          Run all tests\n");
  printf("  test -f, --file=file    Run a specific test\n");
  printf("  -h, --help              Display this help and exit\n");
}

int main(int argc, char **argv) {
  // Check if the `test` subcommand was passed in
  if (argc >= 2) {
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
      print_usage(argv[0]);
      return 0;
    } else if (strcmp(argv[1], "test") == 0) {
      if (argc <= 2) {
        fprintf(stderr, "No test option specified\n");
        print_usage(argv[0]);
        return 1;
      }
      handle_test_command(argc, argv);
    } else {
      fprintf(stderr, "Unknown command: %s\n", argv[1]);
      print_usage(argv[0]);
      return 1;
    }
    return 1;
  } else {
    handle_build();
  }

  return 0;
}
