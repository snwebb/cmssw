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
  
  auto & ctc = coarseTCs_[ctcid];
  
  ctc.sumPt += tc.pt();
  ctc.sumHwPt += tc.hwPt();
  ctc.sumMipPt += tc.mipPt();

  if ( tc.mipPt() > coarseTCs_[ctcid].maxMipPt ){
    ctc.maxId = tc.detId();
    ctc.maxMipPt = tc.mipPt();
  }

}

void
HGCalConcentratorCoarsenerImpl::
assignCoarseTriggerCellEnergy(l1t::HGCalTriggerCell &tc, uint32_t ctcid){
  auto & ctc = coarseTCs_[ctcid];
  tc.setHwPt(ctc.sumHwPt);
  tc.setMipPt(ctc.sumMipPt);
  tc.setPt(ctc.sumPt);
}

void 
HGCalConcentratorCoarsenerImpl::
coarsen(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  // first pass, fill the coarse trigger cell information
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {

    int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
    if ( fixedDataSizePerHGCROC_ && thickness == kHighDensityThickness_ ){
      trigCellVecOutput.push_back( tc );
      continue;
    }

    uint32_t ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId());
    updateCoarseTriggerCellMaps( tc, ctcid );
  }



  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {

    int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
    if ( fixedDataSizePerHGCROC_ && thickness == kHighDensityThickness_ ){
      continue;
    }

    uint32_t ctcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId());
    if ( tc.detId() == coarseTCs_[ctcid].maxId){
      trigCellVecOutput.push_back( tc );
      assignCoarseTriggerCellEnergy( trigCellVecOutput.back(), ctcid );
      coarseTCmapping_.setEvenDetId(trigCellVecOutput.back());
      GlobalPoint point = coarseTCmapping_.getCoarseTriggerCellPosition( trigCellVecOutput.back().detId() );
      math::PtEtaPhiMLorentzVector p4(trigCellVecOutput.back().pt(), point.eta(), point.phi(), trigCellVecOutput.back().mass());
      trigCellVecOutput.back().setPosition(point);
      trigCellVecOutput.back().setP4(p4);

    }
    
  }


  for (auto & ctc : coarseTCs_){
   l1t::HGCalTriggerCell triggerCell;   
   assignCoarseTriggerCellEnergy( triggerCell, ctc.first );
   coarseTCmapping_.setEvenDetId( triggerCell );        
   GlobalPoint point = coarseTCmapping_.getCoarseTriggerCellPosition( trigCellVecOutput.back().detId() );
   triggerCell.setPosition(point);
   trigCellVecOutput.push_back( triggerCell );
  }


}
