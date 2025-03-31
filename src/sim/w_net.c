#include "sim.h"
#include <sys/socket.h>


#ifdef NET


#define NET_BUFFER_SIZE 1024


int net_listen_port;
int net_listen_socket;


int
udp_listen(int port)
{
  struct sockaddr_in addr;
  int flags;

  net_listen_port = port;

  net_listen_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (net_listen_socket < 0) {
    perror("socket()");
    return 0;
  }

  flags = 1;
  if (setsockopt(net_listen_socket, SOL_SOCKET, SO_REUSEADDR,
		 (char *)&flags, sizeof(flags)) == -1) {
    perror("setsockopt SO_REUSEADDR");
    return 0;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = net_listen_port;
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(net_listen_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind()");
    return 0;
  }

  if ((flags = fcntl(net_listen_socket, F_GETFL)) < 0) {
    perror("fcntl F_GETFL");
    return 0;
  }

  if (fcntl(net_listen_socket, F_SETFL, flags|O_NDELAY) < 0) {
    perror("fcntl F_SETFL");
    return 0;
  }

  Tcp_MakeOpenFile(tk_mainInterp, net_listen_socket, 1, 1);

  return (net_listen_socket);
}


udp_hear(int sock)
{
  struct sockaddr_in addr;
  int addr_len;
  int len, i;
  unsigned char buf[NET_BUFFER_SIZE];
  char cmd[NET_BUFFER_SIZE * 4 + 256];
  char *cp;

  while (1) {
    len = recvfrom(sock, buf, NET_BUFFER_SIZE, 0,
		   (struct sockaddr *)&addr, &addr_len);

    if (len < 0) {
      if (errno == EINTR) continue;
      if (errno == EWOULDBLOCK) break;
      perror("recvfrom");
      return;
    }

    sprintf(cmd, "HandlePacket %d {%s} {", sock, inet_ntoa(addr.sin_addr));

    cp = cmd + strlen(cmd);
    for (i = 0; i < len; i++) {
      sprintf(cp, "%3d ", buf[i]);
      cp += 4;
    }
    sprintf(cp, "}");
    Eval(cmd);
  }
}


#endif
