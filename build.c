#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>
#include <unistd.h>

#define CC "gcc"

#define SRC_DIR_NAME "src"
#define TESTS_DIR_NAME "tests"
#define OBJ_DIR_NAME "obj"
#define BIN_DIR_NAME "bin"

#define SRC_DIR "./" SRC_DIR_NAME "/"
#define TESTS_DIR "./" TESTS_DIR_NAME "/"
#define OBJ_DIR "./" OBJ_DIR_NAME "/"
#define BIN_DIR "./" BIN_DIR_NAME "/"

#define OBJ_SRC_DIR OBJ_DIR SRC_DIR_NAME "/"
#define OBJ_TESTS_DIR OBJ_DIR TESTS_DIR_NAME "/"

#define BIN_SRC_DIR BIN_DIR SRC_DIR_NAME "/"
#define BIN_TESTS_DIR BIN_DIR TESTS_DIR_NAME "/"

#define CFLAGS                                                      \
  "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-Wunused-variable", \
      "-std=c99", "-O0", "-c", "-g"

bool ends_with(const char *str1, const char *str2) {
  while (*str1 != '\0') {
    if (*str1 == str2[0] && strcmp(str1, str2) == 0) {
      return true;
    }
    str1++;
  }

  return false;
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

int calc_name_length(const char *str) {
  assert(str != NULL);
  int i = 0;
  while (str[i] != '.' && str[i] != '\0') {
    i++;
  }
  return i;
}

void compile(char *in_file, char *out_file) {
  assert(in_file != NULL);
  assert(out_file != NULL);

  // Run the command in a child process
  pid_t pid = fork();
  if (pid == -1) {
    // Fork failed
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Child process
    char *const cmd[] = {CC, CFLAGS, in_file, "-o", out_file, NULL};
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

void build_exe(char *obj_dir) {
  // Setup the command
  char **cmd = malloc(20 * sizeof(char *));
  if (cmd == NULL) {
    printf("unable to allocate memory\n");
  }
  cmd[0] = CC;

  // Open the directory containing the objs
  DIR *dir = opendir(obj_dir);
  if (dir == NULL) {
    perror("opendir failed");
  }

  // Append each obj to the command
  int i = 1;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (ends_with(entry->d_name, ".o")) {
      // Construct path ./obj/src/<d_name>.o or ./obj/tests/<d_name>.o
      char *out_file = malloc(
          100 * sizeof(char));  // malloc b/c we need it to live past this loop
      strcpy(out_file, obj_dir);
      strcat(out_file, entry->d_name);
      // printf("%s\n", out_file);

      // Append to command
      cmd[i] = out_file;
      i++;
    }
  }

  // Append the remaining flags
  cmd[i++] = "-o";
  cmd[i++] = BIN_SRC_DIR "main";
  cmd[i++] = NULL;
  print_cmd(cmd);
  if (execvp(CC, cmd) == -1) {
    perror("failed to link");
  }
}

void run_all_tests(void) {
  // Open the ./tests/ directory
  DIR *dir = opendir(TESTS_DIR);
  if (dir == NULL) {
    perror("opendir failed");
    exit(EXIT_FAILURE);
  }

  // Every test depends on tests/test.c
  compile(TESTS_DIR "test.c", OBJ_TESTS_DIR "test.o");

  // Compile each test
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (ends_with(entry->d_name, ".c") &&
        strcmp(entry->d_name, "test.c") != 0) {
      char in_file[100];
      strcpy(in_file, TESTS_DIR);
      strcat(in_file, entry->d_name);

      char out_file[100];
      strcpy(out_file, OBJ_TESTS_DIR);
      strncat(out_file, entry->d_name, calc_name_length(entry->d_name));
      strcat(out_file, ".o");

      char bin_file[100];
      strcpy(bin_file, BIN_TESTS_DIR);
      strncat(bin_file, entry->d_name, calc_name_length(entry->d_name));

      // printf("%s  %s\n", in_file, out_file);
      compile(in_file, out_file);

      // Setup the command; Each test is an executable
      char *cmd[] = {
          CC, out_file, OBJ_TESTS_DIR "test.o", "-o", bin_file, NULL,
      };
      print_cmd(cmd);
      if (execvp(CC, cmd) == -1) {
        perror("failed to link");
      }
    }
  }
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
  DIR *dir = opendir(SRC_DIR);
  if (dir == NULL) {
    perror("opendir failed");
    exit(EXIT_FAILURE);
  }

  // Read and compile source code to objs
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (ends_with(entry->d_name, ".c")) {
      char in_file[100];
      strcpy(in_file, SRC_DIR);
      strcat(in_file, entry->d_name);

      char out_file[100];
      strcpy(out_file, OBJ_SRC_DIR);
      strncat(out_file, entry->d_name, calc_name_length(entry->d_name));
      strcat(out_file, ".o");

      // printf("%s  %s\n", in_file, out_file);
      compile(in_file, out_file);
    }
  }

  // Link all the objs to create an executable
  build_exe(OBJ_SRC_DIR);

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
      return 0;
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
