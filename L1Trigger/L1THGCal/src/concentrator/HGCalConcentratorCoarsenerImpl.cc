#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorCoarsenerImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorCoarsenerImpl::
HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf)
{    
}


void 
HGCalConcentratorCoarsenerImpl::
updateCoarseTriggerCellMaps(const l1t::HGCalTriggerCell& tc, int ctcid){

  coarseTCsumPt[ctcid] += tc.pt();
  coarseTCsumHwPt[ctcid] += tc.hwPt();
  coarseTCsumMipPt[ctcid] += tc.mipPt();

  if ( tc.hwPt() > coarseTCmaxHwPt[ctcid] ){
    coarseTCmaxId[ctcid] = tc.detId();
    coarseTCmaxHwPt[ctcid] = tc.hwPt();
  }

}

void
HGCalConcentratorCoarsenerImpl::
assignCoarseTriggerCellEnergy(l1t::HGCalTriggerCell &tc, int ctcid){
    tc.setHwPt(coarseTCsumHwPt[ctcid]);
    tc.setMipPt(coarseTCsumMipPt[ctcid]);
    tc.setPt(coarseTCsumPt[ctcid]);
}

void 
HGCalConcentratorCoarsenerImpl::
coarseTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  // first pass, fill the coarse trigger cell information
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB)  continue;
    int ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),2);
    updateCoarseTriggerCellMaps( tc, ctcid );
  }

  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      int ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),2);
      
      if ( tc.detId() == coarseTCmaxId[ctcid]){
        trigCellVecOutput.push_back( tc );
	assignCoarseTriggerCellEnergy( trigCellVecOutput.back(), ctcid );
	setEvenDetId(trigCellVecOutput.back());        
      }

    }
  }


}
