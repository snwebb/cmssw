#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap_SA.h"

#include <unordered_map>

using namespace l1t;

HGCalTowerMap_SA::HGCalTowerMap_SA(const std::vector<unsigned short> tower_ids) {
  for (auto tower_id : tower_ids) {
    towerMap_[tower_id] = l1t::HGCalTower_SA(0., 0.);
  }
}

HGCalTowerMap_SA& HGCalTowerMap_SA::operator+=(HGCalTowerMap_SA& map) {
  for (auto& tower : map.towers()) {
    auto this_tower = towerMap_.find(tower.first);
    if (this_tower != towerMap_.end()) {
      this_tower->second += tower.second;
    }
  }

  return *this;
}
