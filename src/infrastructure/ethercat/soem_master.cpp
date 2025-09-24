#ifdef USE_SOEM

#include "infrastructure/ethercat/soem_master.hpp"

extern "C" {
#include <ethercattype.h>
#include <nicdrv.h>
#include <ethercatmain.h>
#include <ethercatconfig.h>
#include <ethercatdc.h>
#include <ethercatprint.h>
}

#include <iostream>

namespace infrastructure::ethercat {

SoemMaster::SoemMaster() : m_isRunning(false) {}
SoemMaster::~SoemMaster() { stop(); }

void SoemMaster::initialize() {
  // На Windows имя интерфейса должно соответствовать Npcap/WinPcap, пример:
  // m_ifname = "\\\\.\\NDIS\\{GUID}" или имя устройства Npcap. Оставим пустым до конфигурации.
}

void SoemMaster::scanNetwork() {
  const char* ifname = m_ifname.empty() ? nullptr : m_ifname.c_str();
  if (!ec_init(const_cast<char*>(ifname))) {
    throw std::runtime_error("SOEM: ec_init failed");
  }
  if (ec_config_init(FALSE) <= 0) {
    ec_close();
    throw std::runtime_error("SOEM: no slaves found");
  }
  ec_config_map(nullptr);
  ec_configdc();
  m_configured = true;
  std::cout << "[SOEM] slaves: " << ec_slavecount << std::endl;
}

void SoemMaster::start() {
  if (!m_configured) return;
  if (ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE) != EC_STATE_SAFE_OP) {
    std::cerr << "[SOEM] SAFE_OP not reached" << std::endl;
  }
  ec_slave[0].state = EC_STATE_OPERATIONAL;
  ec_writestate(0);
  if (ec_statecheck(0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE) != EC_STATE_OPERATIONAL) {
    std::cerr << "[SOEM] OP not reached" << std::endl;
  }
  m_isRunning = true;
}

void SoemMaster::stop() {
  if (!m_isRunning.exchange(false) && !m_configured) return;
  ec_slave[0].state = EC_STATE_INIT;
  ec_writestate(0);
  ec_close();
  m_configured = false;
}

void SoemMaster::setMotorTargetPosition(std::int32_t nodeId, double positionRadians) {
  std::lock_guard<std::mutex> lk(m_mutex);
  m_targets[nodeId].pos = positionRadians;
}

void SoemMaster::setMotorTargetVelocity(std::int32_t nodeId, double velocityRadPerSec) {
  std::lock_guard<std::mutex> lk(m_mutex);
  m_targets[nodeId].vel = velocityRadPerSec;
}

void SoemMaster::updateCycle() {
  if (!m_isRunning.load()) return;
  // Пример обмена: ec_send_processdata(); ec_receive_processdata(EC_TIMEOUTRET);
  ec_send_processdata();
  ec_receive_processdata(EC_TIMEOUTRET);
  // Здесь нужно записывать в PDO выходы исходя из m_targets и читать входы.
}

} // namespace infrastructure::ethercat

#endif // USE_SOEM


