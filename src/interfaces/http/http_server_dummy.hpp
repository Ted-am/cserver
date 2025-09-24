#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <atomic>
#include <thread>

#include "application/services/motor_control_service.hpp"

namespace interfaces::http {

class HttpServerDummy {
public:
  HttpServerDummy() : m_running(false) {}
  ~HttpServerDummy() { stop(); }

  void start(unsigned short port, std::shared_ptr<application::services::MotorControlService> service) {
    if (m_running) return;
    m_running = true;
    m_service = std::move(service);
    m_port = port;
    std::cout << "[HTTP] старт порт=" << port << std::endl;
    m_thread = std::thread([this]() { this->runLoop(); });
  }

  void stop() {
    if (!m_running) return;
    m_running = false;
    std::cout << "[HTTP] стоп" << std::endl;
    if (m_thread.joinable()) m_thread.join();
  }

private:
  void runLoop();

  std::atomic<bool> m_running;
  unsigned short m_port{0};
  std::shared_ptr<application::services::MotorControlService> m_service;
  std::thread m_thread;
};

} // namespace interfaces::http


