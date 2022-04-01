#include "../lib/frida-core-x86/frida-core.h"

typedef struct {
  FridaDeviceManager *manager;
  FridaDevice *device;
  FridaSession *session;
  FridaScript *script;
} Frida;

Frida *cleanFrida(Frida *frida);

Frida *attach(int RiotClientPID);
