#pragma once

#include <cstdint>

namespace domain::ports {

class IEthercatPort {
public:
  virtual ~IEthercatPort() = default;

  virtual void initialize() = 0;
  virtual void scanNetwork() = 0;
  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void setMotorTargetPosition(std::int32_t nodeId, double positionRadians) = 0;
  virtual void setMotorTargetVelocity(std::int32_t nodeId, double velocityRadPerSec) = 0;

  virtual void updateCycle() = 0;
};

} // namespace domain::ports


