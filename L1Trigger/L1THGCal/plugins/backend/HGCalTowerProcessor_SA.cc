#include "L1Trigger/L1THGCal/interface/HGCalProcessorBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTowerMap.h"
#include "DataFormats/L1THGCal/interface/HGCalTower.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap2DImpl.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalTowerMap3DImpl.h"

#include "L1Trigger/L1THGCal/interface/backend/HGCalAlgoWrapperBase.h"

class HGCalTowerProcessorSA : public HGCalTowerProcessorBase {
public:
  HGCalTowerProcessorSA(const edm::ParameterSet& conf) : HGCalTowerProcessorBase(conf), conf_(conf) {
    
    const std::string towerMapsAlgoName(conf.getParameterSet("towermap_parameters").getParameter<std::string>("AlgoName"));
    towerMapWrapper_ = std::unique_ptr<HGCalTowerMapsWrapperBase>{
        HGCalTowerMapsWrapperBaseFactory::get()->create(towerMapsAlgoName, conf)};
  }

  void eventSetup(const edm::EventSetup& es) override { }

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

  /* Standalone algorithm instance */
  std::unique_ptr<HGCalTowerMapsWrapperBase> towerMapWrapper_;

  const edm::ParameterSet conf_;

};

DEFINE_EDM_PLUGIN(HGCalTowerFactory, HGCalTowerProcessorSA, "HGCalTowerProcessorSA");
