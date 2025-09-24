#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "domain/ports/ethercat_port.hpp"

namespace infrastructure::ethercat {

class EthercatMasterDummy : public domain::ports::IEthercatPort {
public:
  EthercatMasterDummy() : m_isRunning(false) {}
  ~EthercatMasterDummy() override { stop(); }

  void initialize() override { std::cout << "[ECAT] initialize (заглушка)" << std::endl; }
  void scanNetwork() override { std::cout << "[ECAT] scanNetwork: 0" << std::endl; }
  void start() override { m_isRunning = true; std::cout << "[ECAT] start" << std::endl; }
  void stop() override { if (m_isRunning.exchange(false)) std::cout << "[ECAT] stop" << std::endl; }

  void setMotorTargetPosition(std::int32_t nodeId, double positionRadians) override {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    auto& st = m_targets[nodeId];
    st.positionRadians = positionRadians;
    std::cout << "[ECAT] set pos node=" << nodeId << " pos(rad)=" << positionRadians << std::endl;
  }

  void setMotorTargetVelocity(std::int32_t nodeId, double velocityRadPerSec) override {
    std::lock_guard<std::mutex> lock(m_dataMutex);
    auto& st = m_targets[nodeId];
    st.velocityRadPerSec = velocityRadPerSec;
    std::cout << "[ECAT] set vel node=" << nodeId << " vel(rad/s)=" << velocityRadPerSec << std::endl;
  }

  void updateCycle() override {
    if (!m_isRunning.load()) return;
  }

private:
  std::atomic<bool> m_isRunning;
  std::mutex m_dataMutex;
  struct TargetState { double positionRadians{0.0}; double velocityRadPerSec{0.0}; };
  std::unordered_map<std::int32_t, TargetState> m_targets;
};

} // namespace infrastructure::ethercat


