#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorCoarsenerImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorCoarsenerImpl::
HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf)
{    
}

int
HGCalConcentratorCoarsenerImpl::getSuperTriggerCellId(int detid) const {

  DetId TC_id( detid );
  if ( TC_id.det() == DetId::Forward ){//V8

    HGCalDetId TC_idV8(detid);

    if( triggerTools_.isScintillator(detid) ){
      return detid; //scintillator
    }
    else{
      int thickness = triggerTools_.thicknessIndex(detid,true);
      int TC_split = (TC_idV8.cell() & kSplit_v8_VeryFine_ );
      detid =  (detid & ~(TC_idV8.kHGCalCellMask ) ) | TC_split;
      return detid;
    }

  }
  else{
    return -1;
  }
  
}

void 
HGCalConcentratorCoarsenerImpl::
coarseTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  std::unordered_map<unsigned,SuperTriggerCell> STCs;

  // first pass, fill the coarse trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB)  continue;
    int stcid = getSuperTriggerCellId(tc.detId());
    STCs[stcid].add(tc, stcid);
  }

  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      const auto & stc = STCs[getSuperTriggerCellId(tc.detId())]; 
    
      if (tc.detId() == stc.GetMaxId()){
        trigCellVecOutput.push_back( tc );
        stc.assignEnergy(trigCellVecOutput.back());        
        stc.setEvenDetId(trigCellVecOutput.back());        
      }

    }
  }


}
