#include "io.hpp"
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

const int TCPServer::DEFAULT_PORT = 8899;

TCPServer::TCPServer(int port_number) {
  if (port_number == 0 || port_number > 65535) {
    port_number = this->DEFAULT_PORT;
  }
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_number);
  server_address.sin_addr.s_addr = INADDR_ANY;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  bind(sock, (struct sockaddr *)&server_address, sizeof(server_address));
  if (sock < 0) {
    cerr << "Error binding socket" << endl;
    exit(1);
  }
  if (listen(sock, 5) < 0) {
    cerr << "Error listening on socket" << endl;
    exit(1);
  }
  this->server_socket = sock;
}

TCPServer::~TCPServer() { close(this->server_socket); }

ClientConnection::ClientConnection(int connfd) : connfd(connfd) {}

ClientConnection TCPServer::wait_and_accept_connection() {
  int connfd = accept(this->server_socket, (struct sockaddr *)NULL, NULL);
  if (connfd < 0) {
    cerr << "Error accepting connection" << endl;
    exit(1);
  }
  cout << "Accepted connection, connfd: " << connfd << endl;
  return ClientConnection(connfd);
}

void ClientConnection::close_connection() {
  if (this->connfd >= 0) {
    close(this->connfd);
    this->connfd = -1;
  }
}

string ClientConnection::receive_message() {
  cout << "Current connfd: " << this->connfd << endl;
  char buffer[1024];
  int input_length = recv(this->connfd, buffer, 1024, 0);
  if (input_length < 0) {
    cerr << "Error reading from socket" << endl;
    exit(1);
  }
  // send the message back
  send(this->connfd, &buffer, input_length, 0);
  return buffer;
}