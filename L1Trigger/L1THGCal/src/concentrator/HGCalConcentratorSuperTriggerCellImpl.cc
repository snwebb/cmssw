#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSuperTriggerCellImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorSuperTriggerCellImpl::
HGCalConcentratorSuperTriggerCellImpl(const edm::ParameterSet& conf)
  : stcSize_(conf.getParameter< std::vector<unsigned> >("stcSize")),
    fixedDataSize_(conf.getParameter<bool>("fixedDataSize"))
{

    if ( stcSize_.size() != kNLayers_ ){
        throw cms::Exception("HGCTriggerParameterError")
            << "Inconsistent size of super trigger cell size vector" << stcSize_.size() ;
    }
    for(auto stc : stcSize_) {

      coarseTCmapping_.checkSizeValidity(stc);

    }

    std::string energyType_(conf.getParameter<string>("type_energy_division"));

    if(energyType_=="superTriggerCell"){
      energyDivisionType_ = superTriggerCell;
    }else if(energyType_=="oneBitFraction"){
      energyDivisionType_ = oneBitFraction;
    }else if(energyType_=="equalShare"){
      energyDivisionType_ = equalShare;
    }else if(energyType_=="coarse2TriggerCell"){
      energyDivisionType_ = coarse2TriggerCell;
    }else {
      energyDivisionType_ = superTriggerCell;
    } 

    
}

void 
HGCalConcentratorSuperTriggerCellImpl::
createMissingTriggerCells( std::unordered_map<unsigned,SuperTriggerCell>& STCs, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput) const
{ 

    for (const auto& s: STCs){

      int thickness = triggerTools_.thicknessIndex(s.second.GetSTCId(),true);
      
      std::vector<int> output_ids;
      coarseTCmapping_.getConstituentTriggerCells( s.second.GetSTCId(), stcSize_.at(thickness), output_ids );

      for (const auto& id: output_ids){

	if ( fixedDataSize_ && thickness > 0){
	  if (id&1) continue;
	}

        if ( !triggerTools_.validTriggerCell(id) ) {
          continue;
        }
        
        l1t::HGCalTriggerCell triggerCell;
        GlobalPoint point = triggerTools_.getTCPosition(id);
        math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);
        triggerCell.setPosition(point);
        triggerCell.setP4(p4);
        triggerCell.setDetId(id);
        
        trigCellVecOutput.push_back ( triggerCell );
        
      }

    }

}



void 
HGCalConcentratorSuperTriggerCellImpl::
superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  std::unordered_map<unsigned,SuperTriggerCell> STCs;
  std::vector<l1t::HGCalTriggerCell> trigCellVecInputEnlarged;

  // first pass, fill the "coarse" trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB){
      trigCellVecInputEnlarged.push_back( tc );
      continue;
    }
    int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
    int stcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),stcSize_.at(thickness));
    STCs[stcid].add(tc, stcid);
  }

   //The missing trigger cells are needed to be created both for coarsening the existing TC
  // in the thick modules (for the fixed data size choice), or for any of the energy spread algorithms (except super TCs)

  createMissingTriggerCells( STCs, trigCellVecInputEnlarged);

  //Change coarse TC positions if needed
  if ( fixedDataSize_ == true ){
    for  (l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {
      coarseTCmapping_.setCoarseTriggerCellPosition( tc );
    }
  }

  if ( energyDivisionType_ == oneBitFraction ){
    //Get the 1 bit fractions. There should be exactly 4 TCs per STC
    for (const l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {
      if (tc.subdetId() == HGCHEB) continue;
      int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
      STCs[coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),stcSize_.at(thickness))].getFractionSum(tc);
    }
  }

  // second pass, write them out
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {
    //If scintillator use a simple threshold cut
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
      const auto & stc = STCs[coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),stcSize_.at(thickness))]; 
    
      if ( (energyDivisionType_!=superTriggerCell && energyDivisionType_!=coarse2TriggerCell)
           || ( energyDivisionType_==superTriggerCell && (tc.detId() == stc.GetMaxId()) )
           || ( energyDivisionType_==coarse2TriggerCell && (!(tc.detId() & 1))  )
           )  {

        trigCellVecOutput.push_back( tc );
        stc.assignEnergy(trigCellVecOutput.back(), energyDivisionType_);        
        
      }


            
    }
    
  }


}
