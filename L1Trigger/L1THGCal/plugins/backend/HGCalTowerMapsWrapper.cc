#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/L1THGCal/interface/backend/HGCalAlgoWrapperBase.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMapImpl_SA.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTowerMap.h"
#include "DataFormats/L1THGCal/interface/HGCalTower.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap2DImpl.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap3DImpl.h"

#include "L1Trigger/L1THGCal/interface/backend/HGCalTower_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMapsConfig_SA.h"

class HGCalTowerMapsWrapper : public HGCalTowerMapsWrapperBase {
public:
  HGCalTowerMapsWrapper(const edm::ParameterSet& conf);
  ~HGCalTowerMapsWrapper() {}

  void configure(const std::pair<const edm::EventSetup&, const edm::ParameterSet& >& parameters) override;

  void process( const std::vector<edm::Ptr<l1t::HGCalTowerMap>>& inputs, l1t::HGCalTowerBxCollection& outputs ) const override;

private:
  void convertCMSSWInputs( const std::vector<edm::Ptr<l1t::HGCalTowerMap>>& inputTowerMaps, std::vector<l1t::HGCalTowerMap_SA>& towerMaps_SA ) const;
  void convertAlgorithmOutputs( const std::vector<l1t::HGCalTower_SA>& towerMaps_SA, l1t::HGCalTowerBxCollection& outputTowerMaps ) const;
  void eventSetup(const edm::EventSetup& es) {}

  HGCalTowerMapImplSA theAlgo_;

  std::unique_ptr<l1t::towerMapsAlgoConfig_SA> theConfiguration_;
};

HGCalTowerMapsWrapper::HGCalTowerMapsWrapper(const edm::ParameterSet& conf) {
}

void HGCalTowerMapsWrapper::convertCMSSWInputs(const std::vector<edm::Ptr<l1t::HGCalTowerMap>>& inputTowerMaps, std::vector<l1t::HGCalTowerMap_SA>& towerMaps_SA ) const {

    for (const auto& map : inputTowerMaps) {

        std::vector<unsigned short> tower_ids;
        for ( const auto& tower : map->towers() ) {
            tower_ids.push_back( tower.first );
        }

        l1t::HGCalTowerMap_SA towerMapSA( tower_ids );
        std::unordered_map<unsigned short, l1t::HGCalTower_SA>& towersSA = towerMapSA.towers();
        
        for ( const auto& tower : map->towers() ) {
            l1t::HGCalTower_SA towerSA( tower.second.etEm(), tower.second.etHad());

            auto this_tower = towersSA.find(tower.first);
            if ( this_tower != towersSA.end() ) {
              this_tower->second += towerSA;
            }
        }
        towerMaps_SA.emplace_back( towerMapSA );
    }
}

void HGCalTowerMapsWrapper::convertAlgorithmOutputs(const std::vector<l1t::HGCalTower_SA>& towers_SA, l1t::HGCalTowerBxCollection& outputTowerMaps) const {

    for ( const auto& towerSA : towers_SA ) {
        // Need to carry eta, phi, ID through the SA emulation
        outputTowerMaps.push_back( 0, l1t::HGCalTower( towerSA.etEm(), towerSA.etHad(), 0, 0, 0 ) );
    }
}

void HGCalTowerMapsWrapper::process( const std::vector<edm::Ptr<l1t::HGCalTowerMap>>& inputs, l1t::HGCalTowerBxCollection& outputs ) const {
    std::vector<l1t::HGCalTowerMap_SA> inputs_SA;
    convertCMSSWInputs( inputs, inputs_SA );

    std::vector<l1t::HGCalTower_SA> outputs_SA;
    theAlgo_.runAlgorithm(inputs_SA, outputs_SA);

    convertAlgorithmOutputs(outputs_SA, outputs);
}

void HGCalTowerMapsWrapper::configure(const std::pair<const edm::EventSetup&, const edm::ParameterSet& >& parameters) {
}


DEFINE_EDM_PLUGIN(HGCalTowerMapsWrapperBaseFactory, 
                   HGCalTowerMapsWrapper, 
                   "HGCalTowerMapsWrapper");