#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <unistd.h>

void initSSL() {
  SSL_load_error_strings();
  SSL_library_init();
}

int createSocket(char *hostname, int port) {
  int sd;
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(hostname);

  sd = socket(AF_INET, SOCK_STREAM, 0);

  int connection;
  do {
    connection = connect(sd, (struct sockaddr *)&addr, sizeof(addr));
  } while (connection != 0);

  return sd;
}

void connectSSL(int socket) {
  const SSL_METHOD *method = TLS_client_method();
  SSL_CTX *ctx = SSL_CTX_new(method);

  SSL *ssl = SSL_new(ctx);
  SSL_set_fd(ssl, socket);

  int connection;
  do {
    connection = SSL_connect(ssl);
  } while (connection != 1);

  SSL_free(ssl);
  SSL_CTX_free(ctx);
  close(socket);
}

void waitForPort(char *hostname, int port) {
  initSSL();

  int socket = createSocket(hostname, port);
  connectSSL(socket);
}
