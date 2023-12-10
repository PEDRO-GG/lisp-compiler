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

  // Build up the command
  char *const cmd[] = {CC, srcfile, CFLAGS, "-o", outfile, NULL};
  print_cmd(cmd);

  // Run the command in a child process
  pid_t pid = fork();
  if (pid == -1) {
    // Fork failed
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Child process
    if (execvp(CC, cmd) == -1) {
      perror("failed to compile");
      exit(EXIT_FAILURE);
    }
  } else {
    // Parent process
    int status;
    waitpid(pid, &status,
            0);  // Optionally wait for the child process to finish
    printf("Continuing execution in the parent process.\n");
  }
}

void build_exe(void) {
  char *const cmd[] = {CC, "./obj/*", "-o", "./bin/main", NULL};
  print_cmd(cmd);
  //   if (execvp(CC, cmd) == -1) {
  //     perror("failed to link");
  //   }
}

int main(void) {
  // Open the source code directory
  DIR *dir = opendir(CODE_DIR);
  if (dir == NULL) {
    perror("opendir failed");
    return 1;
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

  return 0;
}
