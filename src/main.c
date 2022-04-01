#include "frida.h"
#include "ssl.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (geteuid() != 0) {
    char command[7 + maxPathSize + 1] = "pkexec ";
    strcat(command, argv[0]);

    return system(command);
  }

  pid_t RiotClientPID = 0, LeagueClientPID = 0, LeagueClientUXPID = 0;

  printf("[*] Waiting for RiotClientServices.exe...\n");
  while (!RiotClientPID) {
    RiotClientPID = getPID("RiotClientServices.exe", "RiotClientServi");
  }
  printf("[*] Found process RiotClientServices.exe, PID: %d\n", RiotClientPID);

  printf("[*] Waiting for LeagueClient.exe...\n");
  while (!LeagueClientPID) {
    // TODO: For some reason, calling getPID with the alt_name as
    // "LeagueClient.ex" breaks everything
    LeagueClientPID = getPID("LeagueClient.exe", NULL);
  }
  printf("[*] Found process LeagueClient.exe, PID: %d\n", LeagueClientPID);

  Frida *frida = NULL;
  printf("[*] Waiting for LeagueClientUx.exe...\n");

  while (!LeagueClientUXPID) {
    if (frida == NULL) {
      printf("[*] Attaching...\n");
      frida = attach(RiotClientPID);
    }

    LeagueClientUXPID = getPID("LeagueClientUx.exe", "LeagueClientUx.");
  }
  printf("[*] Found process LeagueClientUx.exe, PID: %d\n", LeagueClientUXPID);

  int appPort = strtoul(getAppPort(LeagueClientUXPID), NULL, 10);
  printf("[*] Waiting for port %d...\n", appPort);

  waitForPort("127.0.0.1", appPort);
  printf("[*] Connected!\n");

  if (frida) {
    printf("[*] Detaching...\n");
    cleanFrida(frida);
  }

  printf("[*] Done!\n");

  return 0;
}
