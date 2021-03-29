#include "L1Trigger/L1THGCal/interface/backend/HGCalTower_SA.h"

using namespace l1t;

HGCalTower_SA& HGCalTower_SA::operator+=(const HGCalTower_SA& tower) {
  etEm_ += tower.etEm();
  etHad_ += tower.etHad();

  return *this;
}
