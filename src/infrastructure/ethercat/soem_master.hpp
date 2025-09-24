#pragma once

#ifdef USE_SOEM

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

#include "domain/ports/ethercat_port.hpp"

// Forward decl из SOEM, чтобы не тянуть сюда все заголовки в header
extern "C" {
  struct ec_slavet;
}

namespace infrastructure::ethercat {

class SoemMaster : public domain::ports::IEthercatPort {
public:
  SoemMaster();
  ~SoemMaster() override;

  void initialize() override;
  void scanNetwork() override;
  void start() override;
  void stop() override;

  void setMotorTargetPosition(std::int32_t nodeId, double positionRadians) override;
  void setMotorTargetVelocity(std::int32_t nodeId, double velocityRadPerSec) override;

  void updateCycle() override;

private:
  std::atomic<bool> m_isRunning;
  std::mutex m_mutex;
  std::string m_ifname; // имя интерфейса, напр. "\x5CDevice\NPF_{...}" на Windows

  struct TargetState { double pos{0.0}; double vel{0.0}; };
  std::unordered_map<std::int32_t, TargetState> m_targets;

  bool m_configured{false};
};

} // namespace infrastructure::ethercat

#endif // USE_SOEM


