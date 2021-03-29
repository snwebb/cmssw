#ifndef L1Trigger_L1THGCal_HGCalTowerMap_SA_h
#define L1Trigger_L1THGCal_HGCalTowerMap_SA_h

#include "L1Trigger/L1THGCal/interface/backend/HGCalTower_SA.h"

#include <unordered_map>
#include <vector>

namespace l1t {

  class HGCalTowerMap_SA {
    public:
      HGCalTowerMap_SA() {};
      HGCalTowerMap_SA( const std::vector<unsigned short> tower_ids );

      ~HGCalTowerMap_SA(){};

      HGCalTowerMap_SA& operator+=(HGCalTowerMap_SA& map);

      std::unordered_map<unsigned short, l1t::HGCalTower_SA>& towers() { return towerMap_; }

    private:
      std::unordered_map<unsigned short, l1t::HGCalTower_SA> towerMap_;
  };

}  // namespace l1t

#endif
