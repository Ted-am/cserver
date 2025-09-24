#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "application/services/motor_control_service.hpp"
#include "infrastructure/ethercat/ethercat_master_dummy.hpp"
#ifdef USE_SOEM
#include "infrastructure/ethercat/soem_master.hpp"
#endif
#include "interfaces/http/http_server_dummy.hpp"

namespace {
volatile std::sig_atomic_t g_shouldExit = 0;

void signalHandler(int) {
  g_shouldExit = 1;
}
}

int main(int argc, char** argv) {
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  std::cout << "cserver: старт" << std::endl;

  try {
    std::shared_ptr<domain::ports::IEthercatPort> ecatPort;
#ifdef USE_SOEM
    ecatPort = std::make_shared<infrastructure::ethercat::SoemMaster>();
#else
    ecatPort = std::make_shared<infrastructure::ethercat::EthercatMasterDummy>();
#endif
    auto motorService = std::make_shared<application::services::MotorControlService>(ecatPort);

    motorService->initialize();
    motorService->scanNetwork();
    motorService->start();

    interfaces::http::HttpServerDummy httpServer;
    const unsigned short port = 8080;
    httpServer.start(port, motorService);
    std::cout << "HTTP сервер запущен на порту " << port << std::endl;

    while (!g_shouldExit) {
      motorService->updateCycle();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    httpServer.stop();
    motorService->stop();
  } catch (const std::exception& ex) {
    std::cerr << "Ошибка: " << ex.what() << std::endl;
    return 1;
  }

  std::cout << "cserver: завершение" << std::endl;
  return 0;
}


