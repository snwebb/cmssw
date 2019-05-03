#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSuperTriggerCellImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorSuperTriggerCellImpl::
HGCalConcentratorSuperTriggerCellImpl(const edm::ParameterSet& conf)
  : stcSize_(conf.getParameter< std::vector<unsigned> >("stcSize")),
    fixedDataSizePerHGCROC_(conf.getParameter<bool>("fixedDataSizePerHGCROC"))
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

      oneBitFractionThreshold_ = conf.getParameter<double>("oneBitFractionThreshold");
      oneBitFractionLowValue_ = conf.getParameter<double>("oneBitFractionLowValue");
      oneBitFractionHighValue_ = conf.getParameter<double>("oneBitFractionHighValue");

    }else if(energyType_=="equalShare"){
      energyDivisionType_ = equalShare;

      nTriggerCellsForDivision_ = conf.getParameter<int>("nTriggerCellsForDivision");

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

	if ( fixedDataSizePerHGCROC_ && thickness > 0){
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
 assignSuperTriggerCellEnergy(l1t::HGCalTriggerCell &c, const SuperTriggerCell &stc) const {
      
  if ( energyDivisionType_ == superTriggerCell || energyDivisionType_ == coarse2TriggerCell ){
    c.setHwPt(stc.GetSumHwPt());
    c.setMipPt(stc.GetSumMipPt());
    c.setPt(stc.GetSumPt());
  }
  else if ( energyDivisionType_ == equalShare ){
    c.setHwPt( stc.GetSumHwPt() / double(nTriggerCellsForDivision_) );
    c.setMipPt( stc.GetSumMipPt() / double(nTriggerCellsForDivision_) );
    c.setPt( stc.GetSumPt() /double(nTriggerCellsForDivision_) );
  }
  else if (  energyDivisionType_ == oneBitFraction ){
    
    double f = c.pt() / stc.GetSumPt() ;
    double frac = 0;
    
    if ( c.detId() != stc.GetMaxId() ){
      if ( f < oneBitFractionThreshold_ ){
	frac = oneBitFractionLowValue_;
      }
      else{
	frac = oneBitFractionHighValue_;
      }
    }
    else{
      frac = 1-stc.GetFractionSum();
    }
    
    c.setHwPt( stc.GetSumHwPt() * frac );
    c.setMipPt( stc.GetSumMipPt() * frac );
    c.setPt( stc.GetSumPt() * frac );
  }
  
}

float 
HGCalConcentratorSuperTriggerCellImpl::
getTriggerCellOneBitFraction( float tcPt, float sumPt ){

  double f = tcPt / sumPt ;
  double frac = 0;
  if ( f < oneBitFractionThreshold_ ){
    frac = oneBitFractionLowValue_;
  }
  else{
    frac = oneBitFractionHighValue_;
  }

  return frac;

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
  if ( fixedDataSizePerHGCROC_ == true ){
    for  (l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {
      coarseTCmapping_.setCoarseTriggerCellPosition( tc );
    }
  }

  if ( energyDivisionType_ == oneBitFraction ){
    //Get the 1 bit fractions. There should be exactly 4 TCs per STC
    for (const l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {
      if (tc.subdetId() == HGCHEB) continue;
      int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
      
      int stcid = coarseTCmapping_.getCoarseTriggerCellId(tc.detId(),stcSize_.at(thickness)); 
      float stc_sumpt = STCs[stcid].GetSumPt();
      float tc_fraction = getTriggerCellOneBitFraction( tc.pt() , stc_sumpt );
      if ( tc.detId() != STCs[stcid].GetMaxId() ){
	STCs[stcid].addToFractionSum(tc_fraction);
      }
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
        assignSuperTriggerCellEnergy(trigCellVecOutput.back(), stc);        
        
      }


            
    }
    
  }


}
