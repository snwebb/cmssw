#ifndef __L1Trigger_L1THGCal_HGCalConcentratorCoarsenerImpl_h__
#define __L1Trigger_L1THGCal_HGCalConcentratorCoarsenerImpl_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"
#include "L1Trigger/L1THGCal/interface/HGCalCoarseTriggerCellMapping.h"

      
#include <array>
#include <vector>

class HGCalConcentratorCoarsenerImpl
{
  public:
    HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf);

    void coarseTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput);
    void eventSetup(const edm::EventSetup& es) {triggerTools_.eventSetup(es);}


  private:

    HGCalTriggerTools triggerTools_;
    HGCalCoarseTriggerCellMapping coarseTCmapping_;
    bool fixedDataSizePerHGCROC_;
    static constexpr int kCoarse2Size_ = 2;
    static constexpr int kHighDensityThickness_ = 0;

    struct coarseTC{

      float sumPt;
      int sumHwPt;
      int maxHwPt;
      float sumMipPt;
      unsigned maxId;

    };

    std::unordered_map<int,coarseTC> coarseTCMap_;

    void updateCoarseTriggerCellMaps( const l1t::HGCalTriggerCell& tc, int ctcid );
    void assignCoarseTriggerCellEnergy(l1t::HGCalTriggerCell &c, int ctcid);
    void setEvenDetId(l1t::HGCalTriggerCell &c) const {
      c.setDetId( c.detId() & ~1 );
    }
    
};

#endif
