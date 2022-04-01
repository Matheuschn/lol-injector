#include "../lib/frida-core-x86/frida-core.h"
#include "util.h"
#include <stdio.h>

typedef struct {
  FridaDeviceManager *manager;
  FridaDevice *device;
  FridaSession *session;
  FridaScript *script;
} Frida;

Frida *cleanFrida(Frida *frida) {
  if (frida->script) {
    frida_script_unload_sync(frida->script, NULL, NULL);
    frida_unref(frida->script);
  }
  if (frida->session) {
    frida_session_detach_sync(frida->session, NULL, NULL);
    frida_unref(frida->session);
  }
  if (frida->device)
    frida_unref(frida->device);

  if (frida->manager) {
    frida_device_manager_close_sync(frida->manager, NULL, NULL);
    frida_unref(frida->manager);
  }

  free(frida);

  return NULL;
}

Frida *attach(int RiotClientPID) {
  GError *error = NULL;
  Frida *frida = calloc(1, sizeof(Frida));

  frida_init();

  frida->manager = frida_device_manager_new();
  FridaDeviceList *devices =
      frida_device_manager_enumerate_devices_sync(frida->manager, NULL, &error);
  if (error != NULL) {
    frida_unref(devices);
    return cleanFrida(frida);
  }

  frida->device = frida_device_list_get(devices, 0);
  if (frida->device == NULL) {
    frida_unref(devices);
    return cleanFrida(frida);
  }

  frida_unref(devices);

  FridaSessionOptions *sessionOptions = frida_session_options_new();
  frida->session = frida_device_attach_sync(frida->device, RiotClientPID,
                                            sessionOptions, NULL, &error);
  frida_unref(sessionOptions);

  if (error != NULL)
    return cleanFrida(frida);

  DLLInfo *dllInfo = getProcessMemoryMap(RiotClientPID);
  if (dllInfo == NULL)
    return cleanFrida(frida);

  char *offset = getOffsetAddress(dllInfo->path, RiotClientPID);

  FridaScriptOptions *scriptOptions = frida_script_options_new();
  frida_script_options_set_name(scriptOptions, "lol-injector");
  frida_script_options_set_runtime(scriptOptions, FRIDA_SCRIPT_RUNTIME_V8);

  char *source = calloc(4096, sizeof(char));
  strcat(source, "let pointer = ptr('0x");
  strcat(source, dllInfo->memoryAddress);
  strcat(source, "');\n"
                 "pointer = pointer.add(0x");
  strcat(source, offset);
  strcat(source, ");\n"
                 "Interceptor.attach(new NativeFunction(pointer, 'int', "
                 "['int', 'pointer', 'pointer', 'pointer', 'pointer']), {\n"
                 "  onEnter(args) {\n"
                 "    args[4].writeInt(0x0);\n"
                 "  }\n"
                 "});");

  frida->script = frida_session_create_script_sync(frida->session, source,
                                                   scriptOptions, NULL, &error);
  free(dllInfo);
  free(offset);
  free(source);
  frida_unref(scriptOptions);
  if (error != NULL)
    return cleanFrida(frida);

  frida_script_load_sync(frida->script, NULL, &error);
  if (error != NULL)
    return cleanFrida(frida);

  return frida;
}
