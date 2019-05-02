#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorCoarsenerImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorCoarsenerImpl::
HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf)
{    
}

void 
HGCalConcentratorCoarsenerImpl::
coarseTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  std::unordered_map<unsigned,SuperTriggerCell> CTCs;

  // first pass, fill the coarse trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB)  continue;
    int ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),2);
    CTCs[ctcid].add(tc);
  }

  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      const auto & ctc = CTCs[coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),2)]; 
    
      if (tc.detId() == ctc.GetMaxId()){
        trigCellVecOutput.push_back( tc );
        ctc.assignEnergy(trigCellVecOutput.back());        
        ctc.setEvenDetId(trigCellVecOutput.back());        
      }

    }
  }


}
