#ifndef __L1Trigger_L1THGCal_HGCalTowerMapsWrapper_h__
#define __L1Trigger_L1THGCal_HGCalTowerMapsWrapper_h__

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

#endif
