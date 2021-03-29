#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMapImpl_SA.h"

void HGCalTowerMapImplSA::runAlgorithm( std::vector<l1t::HGCalTowerMap_SA>& inputTowerMaps_SA, std::vector<l1t::HGCalTower_SA>& outputTowers_SA ) const {
    // Need better way to initialise the output tower map
    if ( inputTowerMaps_SA.size() == 0 ) return;
    std::vector<unsigned short> tower_ids;
    for ( const auto& tower : inputTowerMaps_SA.front().towers() ) {
        tower_ids.push_back( tower.first );
    }
    l1t::HGCalTowerMap_SA towerMap( tower_ids );


    for (auto& map : inputTowerMaps_SA) {
        towerMap += map;
    }

    for ( auto& tower : towerMap.towers() ) {
        if ( tower.second.etEm() > 0 || tower.second.etHad() > 0 ) {
            outputTowers_SA.push_back( tower.second );
        }
    }
}