#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSuperTriggerCellImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorSuperTriggerCellImpl::
HGCalConcentratorSuperTriggerCellImpl(const edm::ParameterSet& conf)
  : energyType_(conf.getParameter<string>("type_energy_division")),
    stcSize_(conf.getParameter< std::vector<unsigned> >("stcSize"))
{

    if ( stcSize_.size() != kNLayers_ ){
        throw cms::Exception("HGCTriggerParameterError")
            << "Inconsistent size of super trigger cell size vector" << stcSize_.size() ;
    }
    for(auto stc : stcSize_) {
        if ( stc!=kSTCsizeFine_ && stc!=kSTCsizeCoarse_ && stc!=kSTCsizeMid_ && stc!=kSTCsizeVeryFine_){
            throw cms::Exception("HGCTriggerParameterError")
              << "Super Trigger Cell should be of size "<<
              kSTCsizeVeryFine_ << " or " << kSTCsizeFine_ <<
	      kSTCsizeMid_ << " or " << kSTCsizeCoarse_;
        }
    }

    if(energyType_=="superTriggerCell"){
      energyDivisionType_ = superTriggerCell;
    }else if(energyType_=="oneBitFraction"){
      energyDivisionType_ = oneBitFraction;
    }else if(energyType_=="equalShare"){
      energyDivisionType_ = equalShare;
    }else {
      energyDivisionType_ = superTriggerCell;
    } 

    
}

std::map<int,int> 
HGCalConcentratorSuperTriggerCellImpl::kSplit_ = {
  {kSTCsizeVeryFine_, kSplit_v8_VeryFine_},
  {kSTCsizeFine_, kSplit_v8_Fine_},
  {kSTCsizeMid_, kSplit_v8_Mid_},
  {kSTCsizeCoarse_, kSplit_v8_Coarse_}
};

std::map<int,int> 
HGCalConcentratorSuperTriggerCellImpl::kSplit_v9_ = {
  {kSTCsizeVeryFine_, kSplit_v9_VeryFine_},
  {kSTCsizeFine_, kSplit_v9_Fine_},
  {kSTCsizeMid_, kSplit_v9_Mid_},
};

int
HGCalConcentratorSuperTriggerCellImpl::getSuperTriggerCellId(int detid, int STCsize) const {

  if ( STCsize > 0 ){
    if ( STCsize!=kSTCsizeFine_ && STCsize!=kSTCsizeCoarse_ && STCsize!=kSTCsizeMid_ && STCsize!=kSTCsizeVeryFine_){
      throw cms::Exception("HGCTriggerParameterError")
	<< "Super Trigger Cell should be of size "<<
	kSTCsizeVeryFine_ << " or " << kSTCsizeFine_ <<
	kSTCsizeMid_ << " or " << kSTCsizeCoarse_;
    }
  }

  DetId TC_id( detid );
  if ( TC_id.det() == DetId::Forward ){//V8

    HGCalDetId TC_idV8(detid);

    if( triggerTools_.isScintillator(detid) ){
      return TC_idV8.cell(); //scintillator
    }
    else{
      int TC_wafer = TC_idV8.wafer();
      int thickness = triggerTools_.thicknessIndex(detid,true);
      int TC_split = -1;
      if ( STCsize > 0 ){
	( TC_idV8.cell() & kSplit_.at( STCsize ) );
      }
      else{
	( TC_idV8.cell() & kSplit_.at( stcSize_.at(thickness) ) );
      }


      return TC_wafer<<kWafer_offset_ | TC_split;
    }

  }

  else if ( TC_id.det() == DetId::HGCalTrigger ){//V9
    
    if( triggerTools_.isScintillator(detid) ){
      HGCScintillatorDetId TC_idV9(detid);
      return TC_idV9.ietaAbs() << HGCScintillatorDetId::kHGCalPhiOffset | TC_idV9.iphi(); //scintillator
    }
    else {
      HGCalTriggerDetId TC_idV9(detid);

      int TC_wafer = TC_idV9.waferU() << kWafer_offset_ | TC_idV9.waferV() ;
      int thickness = triggerTools_.thicknessIndex(detid);

      int TC_12th = 0;
      int Uprime = 0;
      int Vprime = 0;
      int rocnum = detIdToROC_.getROCNumber( TC_idV9.triggerCellU() , TC_idV9.triggerCellV(), 1 );

      if ( rocnum == 1 ){

          Uprime = TC_idV9.triggerCellU();
          Vprime = TC_idV9.triggerCellV() - TC_idV9.triggerCellU();

      }
      else if ( rocnum == 2 ){

          Uprime = TC_idV9.triggerCellU()-TC_idV9.triggerCellV()-1;
          Vprime = TC_idV9.triggerCellV();

      }
      else if ( rocnum == 3 ){

          Uprime = TC_idV9.triggerCellU()-kRotate4_;
          Vprime = TC_idV9.triggerCellV()-kRotate4_;

      }

      int TC_split = -1;

      if ( STCsize > 0 ){
	TC_split =  (rocnum << kRocShift_) | ( (Uprime << kUShift_ | Vprime) & kSplit_v9_.at( 2 ) );
      }
      else{
	
	if (stcSize_.at(thickness) == kSTCsizeCoarse_){
	  TC_split =  rocnum;
	}
	else{
	  TC_split =  (rocnum << kRocShift_) | ( (Uprime << kUShift_ | Vprime) & kSplit_v9_.at( stcSize_.at(thickness) ) );
	}

      }
    
      return TC_wafer<<kWafer_offset_ | TC_split;
      
    }
    
    
  }
  else{
    return -1;
  }
  
}

void 
HGCalConcentratorSuperTriggerCellImpl::
createMissingTriggerCells( std::unordered_map<unsigned,SuperTriggerCell>& STCs, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 


    for (auto& s: STCs){

      bool satisfy = false;
      //Find and create missing TCs (for super TC 4)
      int thickness = triggerTools_.thicknessIndex(s.second.GetTCList().at(0),true);
            
      if ( stcSize_.at(thickness) ==  4 ){
        
        for ( int i = 0; i < 2; i++ ){
          for ( int j = 0; j < 2; j++ ){
            
            satisfy = false;
            for (auto tc:s.second.GetTCList() ){
              HGCalDetId TC_idV8(tc);
              if ( ((TC_idV8.cell() & 1) == i && ((TC_idV8.cell()>>2) & 1) == j )){
                satisfy = true;
              }
              
            }
            if ( satisfy == false ){//Create new TC
              int tc_base = s.second.GetTCList().at(0);
              //Clear relevant bits         
              tc_base = tc_base & ~(1 << 2); 
              tc_base = tc_base & ~(1); 
              //Set bits based on i and j values
              int newtc = tc_base | i;
              newtc = newtc | (j<<2);

                if ( !triggerTools_.validTriggerCell(newtc) ) {
                  s.second.reject();
                  continue;
                }
              
              l1t::HGCalTriggerCell triggerCell;
              GlobalPoint point = triggerTools_.getTCPosition(newtc);
              math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);

              triggerCell.setPosition(point);
              triggerCell.setP4(p4);
              triggerCell.setDetId(newtc);
              
              s.second.addToList( triggerCell );
              trigCellVecOutput.push_back ( triggerCell );
              
            }
            
          }
        }
      }

      else if (stcSize_.at(thickness) == 2){
        
        for ( int i = 0; i < 2; i++ ){
          
          satisfy = false;
          for (auto tc:s.second.GetTCList() ){
            HGCalDetId TC_idV8(tc);
            if ( ((TC_idV8.cell() & 1) == i)){
              satisfy = true;
            }
          }
          if ( satisfy == false ){//Create new TC
            int tc_base = s.second.GetTCList().at(0);
            //Clear relevant bits           
            tc_base = tc_base & ~(1); 
            //Set bits based on i value
            int newtc = tc_base | i;
            
            if ( !triggerTools_.validTriggerCell(newtc) ) {
              s.second.reject();
              continue;
            }
            
            
            l1t::HGCalTriggerCell triggerCell;
            GlobalPoint point = triggerTools_.getTCPosition(newtc);
            math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);
            
            triggerCell.setPosition(point);         
            triggerCell.setP4(p4);
            triggerCell.setDetId(newtc);

            s.second.addToList( triggerCell );      
            
            trigCellVecOutput.push_back ( triggerCell );
            
          }
          
        }
        
      }
      

      else if (stcSize_.at(thickness) == 8){
        
        for ( int i = 0; i < 2; i++ ){
          for ( int j = 0; j < 2; j++ ){
            for ( int k = 0; k < 2; k++ ){
              
              satisfy = false;
              for (auto tc:s.second.GetTCList() ){
                HGCalDetId TC_idV8(tc);
                if ( ((TC_idV8.cell() & 1) == i && ((TC_idV8.cell()>>1) & 1) == j && ((TC_idV8.cell()>>2) & 1) == k)){
                  satisfy = true;
                }
              }
              if ( satisfy == false ){//Create new TC
                int tc_base = s.second.GetTCList().at(0);
                //Clear relevant bits       
                tc_base = tc_base & ~(1); 
                tc_base = tc_base & ~(1 << 1); 
                tc_base = tc_base & ~(1 << 2); 
                //Set bits based on i value
                int newtc = tc_base | i;
                newtc = newtc | (j<<1);
                newtc = newtc | (k<<2);
                
                if ( !triggerTools_.validTriggerCell(newtc) ) {
                  s.second.reject();
                  continue;
                }
                
                
                l1t::HGCalTriggerCell triggerCell;
                GlobalPoint point = triggerTools_.getTCPosition(newtc);
                math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);
                
                triggerCell.setPosition(point);     
                triggerCell.setP4(p4);
                triggerCell.setDetId(newtc);

                s.second.addToList( triggerCell );                      
                trigCellVecOutput.push_back ( triggerCell );
                
              }
              
            }
            
          }
          
        }
        
      }
     

 
    }

        
 
}


void 
HGCalConcentratorSuperTriggerCellImpl::
coarsenTriggerCells( std::unordered_map<unsigned,SuperTriggerCell>& STCs, const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 
  std::unordered_map<unsigned,SuperTriggerCell> coarseTCs;


  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() != HGCHEB) {
      coarseTCs[getSuperTriggerCellId(tc.detId(),2)].add(tc);
    }
    else{
      trigCellVecOutput.push_back( tc );
    }
  }

  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
     if (tc.subdetId() == HGCHEB) continue;

     int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
     if ( thickness > 0 ){

      if (!(tc.detId() & 1)) { //if even
        const auto & ctc = coarseTCs[getSuperTriggerCellId(tc.detId(),2)]; 
        auto & stc = STCs[getSuperTriggerCellId(tc.detId())]; 
        trigCellVecOutput.push_back( tc );
        ctc.assignEnergy(trigCellVecOutput.back(), "STC");      
        //        //reassign max id to the correct one;
        if ( trigCellVecOutput.back().hwPt() >= stc.GetMaxHwPt() ){

          if ( stc.GetMaxId() != tc.detId() ){
            stc.SetMaxTC( trigCellVecOutput.back() );
          }

        }
      }

    }
    else{

      trigCellVecOutput.push_back( tc );

    }


  }


}


void 
HGCalConcentratorSuperTriggerCellImpl::
superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  fixedDataSize_ = true;

  std::unordered_map<unsigned,SuperTriggerCell> STCs;
  std::vector<l1t::HGCalTriggerCell> trigCellVecInputEnlarged;

  // first pass, fill the "coarse" trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    trigCellVecInputEnlarged.push_back ( tc );
    if (tc.subdetId() == HGCHEB) continue;
    STCs[getSuperTriggerCellId(tc.detId())].add(tc);
  }


  //The missing trigger cells are needed to be created both for coarsening the existing TC
  // in the thick modules (for the fixed data size choice), or for any of the energy spread algorithms (except super TCs)

  if ( fixedDataSize_ == true || energyDivisionType_!=superTriggerCell){
    createMissingTriggerCells( STCs, trigCellVecInputEnlarged);
  }


  //Coarsen if needed
  std::vector<l1t::HGCalTriggerCell> trigCellVecInputCoarsened;
  if ( fixedDataSize_ == true ){
    coarsenTriggerCells( STCs, trigCellVecInputEnlarged, trigCellVecInputCoarsened);
  }
  else{
    trigCellVecInputCoarsened = trigCellVecInputEnlarged;
  }

  
  if ( energyDivisionType_ == oneBitFraction ){
    //Get the 1 bit fractions. There should be exactly 4 TCs per STC
    for (const l1t::HGCalTriggerCell & tc : trigCellVecInputCoarsened) {
      if (tc.subdetId() == HGCHEB) continue;
      STCs[getSuperTriggerCellId(tc.detId())].getFractionSum(tc);
    }
  }

  // second pass, write them out
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInputCoarsened) {
    //If scintillator use a simple threshold cut
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      const auto & stc = STCs[getSuperTriggerCellId(tc.detId())]; 
      if ( stc.rejected() ) continue;
    
      if ( (energyDivisionType_!=superTriggerCell) || ( energyDivisionType_==superTriggerCell && (tc.detId() == stc.GetMaxId()) ) )  {

        trigCellVecOutput.push_back( tc );
        
        if (energyDivisionType_==superTriggerCell)  stc.assignEnergy(trigCellVecOutput.back(), "STC");
        if (energyDivisionType_==equalShare)  stc.assignEnergy(trigCellVecOutput.back(), "EqualShare");
        if (energyDivisionType_==oneBitFraction)  stc.assignEnergy(trigCellVecOutput.back(), "1bit");
        
      }
            
    }
    
  }


}
