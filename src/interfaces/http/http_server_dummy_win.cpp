#include "interfaces/http/http_server_dummy.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <fstream>
#include <sstream>

namespace {

std::string readFile(const char* path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) return {};
  std::ostringstream ss;
  ss << f.rdbuf();
  return ss.str();
}

void sendAll(SOCKET s, const char* data, int len) {
  int sent = 0;
  while (sent < len) {
    int r = send(s, data + sent, len - sent, 0);
    if (r <= 0) break;
    sent += r;
  }
}

}

namespace interfaces::http {

void HttpServerDummy::runLoop() {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
    std::cerr << "[HTTP] WSAStartup failed" << std::endl;
    return;
  }

  SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listenSock == INVALID_SOCKET) {
    std::cerr << "[HTTP] socket failed" << std::endl;
    WSACleanup();
    return;
  }

  sockaddr_in service{};
  service.sin_family = AF_INET;
  service.sin_addr.s_addr = htonl(INADDR_ANY);
  service.sin_port = htons(m_port);

  if (bind(listenSock, reinterpret_cast<sockaddr*>(&service), sizeof(service)) == SOCKET_ERROR) {
    std::cerr << "[HTTP] bind failed" << std::endl;
    closesocket(listenSock);
    WSACleanup();
    return;
  }

  if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
    std::cerr << "[HTTP] listen failed" << std::endl;
    closesocket(listenSock);
    WSACleanup();
    return;
  }

  while (m_running) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(listenSock, &readfds);
    timeval tv{1, 0};
    int sel = select(0, &readfds, nullptr, nullptr, &tv);
    if (sel <= 0) {
      continue;
    }
    SOCKET client = accept(listenSock, nullptr, nullptr);
    if (client == INVALID_SOCKET) continue;

    char buf[2048];
    int r = recv(client, buf, sizeof(buf)-1, 0);
    if (r <= 0) { closesocket(client); continue; }
    buf[r] = '\0';

    std::string req(buf);
    std::string path = "/";
    {
      std::istringstream is(req);
      std::string method, url, ver;
      is >> method >> url >> ver;
      if (!url.empty()) path = url;
    }

    std::string body;
    std::string contentType = "text/html; charset=utf-8";
    int status = 200;
    const char* statusText = "OK";

    if (path == "/" || path == "/index.html") {
      body = readFile("www/index.html");
      if (body.empty()) { status = 404; statusText = "Not Found"; body = "Not Found"; contentType = "text/plain"; }
    } else if (path == "/style.css") {
      body = readFile("www/style.css");
      contentType = "text/css; charset=utf-8";
      if (body.empty()) { status = 404; statusText = "Not Found"; body = "Not Found"; contentType = "text/plain"; }
    } else {
      status = 404; statusText = "Not Found"; body = "Not Found"; contentType = "text/plain";
    }

    std::ostringstream resp;
    resp << "HTTP/1.1 " << status << " " << statusText << "\r\n";
    resp << "Content-Type: " << contentType << "\r\n";
    resp << "Content-Length: " << body.size() << "\r\n";
    resp << "Connection: close\r\n\r\n";
    resp << body;
    auto respStr = resp.str();
    sendAll(client, respStr.c_str(), static_cast<int>(respStr.size()));
    closesocket(client);
  }

  closesocket(listenSock);
  WSACleanup();
}

} // namespace interfaces::http


