#ifndef __L1Trigger_L1THGCal_HGCalTowerMapImplSA_h__
#define __L1Trigger_L1THGCal_HGCalTowerMapImplSA_h__

#include "L1Trigger/L1THGCal/interface/backend/HGCalTower_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMapsConfig_SA.h"

class HGCalTowerMapImplSA {
public:
  HGCalTowerMapImplSA() {}
  ~HGCalTowerMapImplSA() {}

  void runAlgorithm(std::vector<l1t::HGCalTowerMap_SA>& inputTowerMaps_SA,
                    std::vector<l1t::HGCalTower_SA>& outputTowers_SA) const;
};

#endif
