#include "L1Trigger/L1THGCal/interface/HGCalProcessorBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTowerMap.h"
#include "DataFormats/L1THGCal/interface/HGCalTower.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap2DImpl.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap3DImpl.h"

#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMapsWrapper.h"

// Can never get MessageLogger to do what I want...
#include <iostream>

class HGCalTowerProcessorSA : public HGCalTowerProcessorBase {
public:
  HGCalTowerProcessorSA(const edm::ParameterSet& conf) : HGCalTowerProcessorBase(conf), conf_(conf) {
    std::cout << "In stand alone Tower Map Processor" << std::endl;

    towermap2D_ = std::make_unique<HGCalTowerMap2DImpl>(conf.getParameterSet("towermap_parameters"));
    towermap3D_ = std::make_unique<HGCalTowerMap3DImpl>();

    towerMapWrapper_ = std::make_unique<HGCalTowerMapsWrapper>( conf );
  }

  void eventSetup(const edm::EventSetup& es) override { towermap2D_->eventSetup(es); }

  void run(const std::pair<edm::Handle<l1t::HGCalTowerMapBxCollection>, edm::Handle<l1t::HGCalClusterBxCollection>>&
               collHandle,
           l1t::HGCalTowerBxCollection& collTowers,
           const edm::EventSetup& es) override {



      es.get<CaloGeometryRecord>().get("", triggerGeometry_);

      auto& towerMapCollHandle = collHandle.first;
      auto& unclTCsCollHandle = collHandle.second;

      /* create a persistent vector of pointers to the towerMaps */
      std::vector<edm::Ptr<l1t::HGCalTowerMap>> towerMapsPtrs;
      for (unsigned i = 0; i < towerMapCollHandle->size(); ++i) {
        towerMapsPtrs.emplace_back(towerMapCollHandle, i);
      }

      // Configuration
      const std::pair<const edm::EventSetup&, const edm::ParameterSet& > configuration{ es, conf_ };
      towerMapWrapper_->configure(configuration);
      towerMapWrapper_->process(towerMapsPtrs, collTowers);

  }

private:
  edm::ESHandle<HGCalTriggerGeometryBase> triggerGeometry_;

  /* algorithms instances */
  std::unique_ptr<HGCalTowerMap2DImpl> towermap2D_;
  std::unique_ptr<HGCalTowerMap3DImpl> towermap3D_;

  /* Standalone algorithm instance */
  std::unique_ptr<HGCalTowerMapsWrapper> towerMapWrapper_;

  const edm::ParameterSet conf_;

};

DEFINE_EDM_PLUGIN(HGCalTowerFactory, HGCalTowerProcessorSA, "HGCalTowerProcessorSA");
