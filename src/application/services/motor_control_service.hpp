#pragma once

#include <cstdint>
#include <memory>

#include "domain/ports/ethercat_port.hpp"

namespace application::services {

class MotorControlService {
public:
  explicit MotorControlService(std::shared_ptr<domain::ports::IEthercatPort> ethercatPort)
    : m_ethercatPort(std::move(ethercatPort)) {}

  void setPosition(std::int32_t nodeId, double positionRadians) {
    m_ethercatPort->setMotorTargetPosition(nodeId, positionRadians);
  }

  void setVelocity(std::int32_t nodeId, double velocityRadPerSec) {
    m_ethercatPort->setMotorTargetVelocity(nodeId, velocityRadPerSec);
  }

  void updateCycle() { m_ethercatPort->updateCycle(); }
  void start() { m_ethercatPort->start(); }
  void stop() { m_ethercatPort->stop(); }
  void initialize() { m_ethercatPort->initialize(); }
  void scanNetwork() { m_ethercatPort->scanNetwork(); }

private:
  std::shared_ptr<domain::ports::IEthercatPort> m_ethercatPort;
};

} // namespace application::services


