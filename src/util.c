#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define maxPathSize 4096
#define maxPipeSize 65336
#define maxFileNameSize 255

typedef struct {
  char *memoryAddress;
  char *path;
} DLLInfo;

char *getWine(pid_t pid) {
  char path[64] = "/proc/";
  sprintf(path + strlen(path), "%d", pid);
  strcat(path, "/exe");

  char buf[maxPathSize + 1];
  int len = readlink(path, buf, maxPathSize);
  buf[len] = '\0';

  // 4096 bytes for MAX_PATH + 1 byte for \0
  char *wineEnv = calloc(4096 + 1, sizeof(char));
  int pathSize = strrchr(buf, '/') - buf + 1;
  memcpy(wineEnv, buf, pathSize);
  memcpy(wineEnv + pathSize, "wine", 5);

  return wineEnv;
}

char *getWineDump(pid_t pid) {
  char *wine = getWine(pid);

  return strcat(wine, "dump");
}

char *getOffsetAddress(char *path, pid_t pid) {
  char command[maxPathSize * 2 + 32];
  char *winedumpBin = getWineDump(pid);

  strcpy(command, winedumpBin);
  free(winedumpBin);

  strcat(command, " -j export ");
  strcat(command, path);
  strcat(command, " | grep select");

  FILE *stream = popen(command, "r");

  char buf[maxPipeSize];
  fgets(buf, maxPipeSize, stream);
  pclose(stream);

  // 8 bytes for 64 bit address + 1 byte for \0
  char *offset = calloc(8 + 1, sizeof(char));
  strcpy(offset, strtok(buf, " "));

  return offset;
}

DLLInfo *getProcessMemoryMap(pid_t pid) {
  char command[64] = "cat /proc/";
  sprintf(command + strlen(command), "%d", pid);
  strcat(command, "/maps | grep ws2_32.dll");

  FILE *stream = popen(command, "r");

  char buf[maxPipeSize];
  fgets(buf, maxPipeSize, stream);
  pclose(stream);

  DLLInfo *info = NULL;

  if (strstr(buf, "ws2_32.dll") != NULL) {
    char *firstLine = strtok(buf, "\n");

    info = malloc(sizeof(DLLInfo));

    // 8 bytes for 64bits address + 1 byte for \0
    char *addr = calloc(8 + 1, sizeof(char));
    int len = strchr(firstLine, '-') - firstLine;
    memcpy(addr, firstLine, len);
    addr[len] = '\0';

    // 4096 bytes for MAX_PATH + 1 byte for \0
    char *path = calloc(4096 + 1, sizeof(char));
    strcpy(path, strchr(firstLine, '/'));

    info->memoryAddress = addr;
    info->path = path;
  }

  return info;
}

char *getAppPort(pid_t pid) {
  char command[64] = "cat /proc/";
  sprintf(command + strlen(command), "%d", pid);
  strcat(command, "/cmdline | sed -e \"s/\\x00/ /g\"");

  FILE *stream = popen(command, "r");

  char buf[maxPipeSize];
  fgets(buf, maxPipeSize, stream);
  pclose(stream);

  char *found = strstr(buf, "--app-port=");

  if (found != NULL) {
    // 11 bytes for the string "--app-port=, 5 bytes for max TCP port number, 1
    // byte for \0"
    char *appPort = calloc(11 + 5 + 1, sizeof(char));
    strcpy(appPort, strtok(found, " ") + 11);

    return appPort;
  } else {
    return NULL;
  }
}

pid_t getPID(char *process_name, char *alt_name) {
  // 9 bytes for the command, 255 bytes for file name and 1 byte for \0
  char buf[9 + maxFileNameSize + 1] = "pidof -s ";
  strcat(buf, process_name);

  FILE *cmd_pipe = popen(buf, "r");

  fgets(buf, 265, cmd_pipe);
  pid_t pid = strtoul(buf, NULL, 10);

  pclose(cmd_pipe);

  if (pid == 0 && alt_name != NULL) {
    pid = getPID(alt_name, NULL);
  }

  return pid;
}
