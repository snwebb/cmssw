#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorCoarsenerImpl.h"

HGCalConcentratorCoarsenerImpl::
HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf)
  : fixedDataSizePerHGCROC_(conf.getParameter<bool>("fixedDataSizePerHGCROC")),
    coarseTCmapping_(std::vector<unsigned>{HGCalCoarseTriggerCellMapping::kCTCsizeVeryFine_,HGCalCoarseTriggerCellMapping::kCTCsizeVeryFine_,HGCalCoarseTriggerCellMapping::kCTCsizeVeryFine_,HGCalCoarseTriggerCellMapping::kCTCsizeVeryFine_})
{    
}


void 
HGCalConcentratorCoarsenerImpl::
updateCoarseTriggerCellMaps(const l1t::HGCalTriggerCell& tc, uint32_t ctcid){
  

  coarseTCs_[ctcid].sumPt += tc.pt();
  coarseTCs_[ctcid].sumHwPt += tc.hwPt();
  coarseTCs_[ctcid].sumMipPt += tc.mipPt();

  if ( tc.mipPt() > coarseTCs_[ctcid].maxMipPt ){
    coarseTCs_[ctcid].maxId = tc.detId();
    coarseTCs_[ctcid].maxHwPt = tc.hwPt();
    coarseTCs_[ctcid].maxMipPt = tc.mipPt();
  }

}

void
HGCalConcentratorCoarsenerImpl::
assignCoarseTriggerCellEnergy(l1t::HGCalTriggerCell &tc, uint32_t ctcid){
    tc.setHwPt(coarseTCs_[ctcid].sumHwPt);
    tc.setMipPt(coarseTCs_[ctcid].sumMipPt);
    tc.setPt(coarseTCs_[ctcid].sumPt);
}

void 
HGCalConcentratorCoarsenerImpl::
select(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  // first pass, fill the coarse trigger cell information
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB)  continue;
    uint32_t ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId());
    updateCoarseTriggerCellMaps( tc, ctcid );
  }

  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      uint32_t ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId());
      int thickness = triggerTools_.thicknessIndex(tc.detId(),true);

      if ( fixedDataSizePerHGCROC_ && thickness == kHighDensityThickness_ ){
	trigCellVecOutput.push_back( tc );
	continue;
      }      

      if ( tc.detId() == coarseTCs_[ctcid].maxId){
	trigCellVecOutput.push_back( tc );
	assignCoarseTriggerCellEnergy( trigCellVecOutput.back(), ctcid );
	coarseTCmapping_.setEvenDetId(trigCellVecOutput.back());        
	coarseTCmapping_.setCoarseTriggerCellPosition( trigCellVecOutput.back() );
      }


    }
  }


}
