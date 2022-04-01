#include <stdlib.h>

#define maxPathSize 4096
#define maxPipeSize 65336
#define maxFileNameSize 255

typedef struct {
  char *memoryAddress;
  char *path;
} DLLInfo;

char *getOffsetAddress(char *path, pid_t pid);

char *getWine(pid_t pid);

char *getWineDump(pid_t pid);

DLLInfo *getProcessMemoryMap(pid_t pid);

char *getAppPort(pid_t pid);

pid_t getPID(char *process_name, char *alt_name);
