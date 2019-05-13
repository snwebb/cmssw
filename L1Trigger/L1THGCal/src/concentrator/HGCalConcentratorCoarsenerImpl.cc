#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorCoarsenerImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorCoarsenerImpl::
HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf)
  : fixedDataSizePerHGCROC_(conf.getParameter<bool>("fixedDataSizePerHGCROC"))
{    
}


void 
HGCalConcentratorCoarsenerImpl::
updateCoarseTriggerCellMaps(const l1t::HGCalTriggerCell& tc, int ctcid){
  

  coarseTCMap_[ctcid].sumPt += tc.pt();
  coarseTCMap_[ctcid].sumHwPt += tc.hwPt();
  coarseTCMap_[ctcid].sumMipPt += tc.mipPt();

  if ( tc.hwPt() > coarseTCMap_[ctcid].sumHwPt ){
    coarseTCMap_[ctcid].maxId = tc.detId();
    coarseTCMap_[ctcid].maxHwPt = tc.hwPt();
  }

}

void
HGCalConcentratorCoarsenerImpl::
assignCoarseTriggerCellEnergy(l1t::HGCalTriggerCell &tc, int ctcid){
    tc.setHwPt(coarseTCMap_[ctcid].sumHwPt);
    tc.setMipPt(coarseTCMap_[ctcid].sumMipPt);
    tc.setPt(coarseTCMap_[ctcid].sumPt);
}

void 
HGCalConcentratorCoarsenerImpl::
coarseTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  // first pass, fill the coarse trigger cell information
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB)  continue;

    int ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),kCoarse2Size_);
    updateCoarseTriggerCellMaps( tc, ctcid );
  }

  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      int ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),kCoarse2Size_);
      int thickness = triggerTools_.thicknessIndex(tc.detId(),true);

      if ( fixedDataSizePerHGCROC_ && thickness == kHighDensityThickness_ ){
	trigCellVecOutput.push_back( tc );
	continue;
      }      

      if ( tc.detId() == coarseTCMap_[ctcid].maxId){
	trigCellVecOutput.push_back( tc );
	assignCoarseTriggerCellEnergy( trigCellVecOutput.back(), ctcid );
	coarseTCmapping_.setEvenDetId(trigCellVecOutput.back());        
	coarseTCmapping_.setCoarseTriggerCellPosition( trigCellVecOutput.back(), kCoarse2Size_ );
      }


    }
  }


}
