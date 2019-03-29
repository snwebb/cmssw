#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSuperTriggerCellImpl.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"

#include <unordered_map>

HGCalConcentratorSuperTriggerCellImpl::
HGCalConcentratorSuperTriggerCellImpl(const edm::ParameterSet& conf)
  : stcSize_(conf.getParameter< std::vector<unsigned> >("stcSize"))
{

    if ( stcSize_.size() != kNLayers_ ){
        throw cms::Exception("HGCTriggerParameterError")
            << "Inconsistent size of super trigger cell size vector" << stcSize_.size() ;
    }
    for(auto stc : stcSize_) {
        if ( stc!=kSTCsizeFine_ && stc!=kSTCsizeCoarse_ && stc!=8 && stc!=2){
            throw cms::Exception("HGCTriggerParameterError")
              << "Super Trigger Cell should be of size 2, 4, 8 or 16" ;
        }
    }
    
}

std::map<int,int> 
HGCalConcentratorSuperTriggerCellImpl::kSplit_ = {
  {2, 0x3e},
  {4, 0x3a},
  {8, 0x38},
  {16, 0x30}
};

int
HGCalConcentratorSuperTriggerCellImpl::getSuperTriggerCellId(int detid) const {


  DetId TC_id( detid );
  if ( TC_id.det() == DetId::Forward ){//V8

    HGCalDetId TC_idV8(detid);

    if( triggerTools_.isScintillator(detid) ){
      return TC_idV8.cell(); //scintillator
    }
    else{
      int TC_wafer = TC_idV8.wafer();
      int thickness = triggerTools_.thicknessIndex(detid,true);
      int TC_split = ( TC_idV8.cell() & kSplit_.at( stcSize_.at(thickness) ) );

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

          Uprime = TC_idV9.triggerCellU()-4;
          Vprime = TC_idV9.triggerCellV()-4;

      }

      TC_12th =  (rocnum << 6) | ( (Uprime << 3 | Vprime) & kSplit_v9_ );

      int TC_split =  TC_12th;
      if (stcSize_.at(thickness) == kSTCsizeCoarse_){
        TC_split =  rocnum;
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
superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  std::unordered_map<unsigned,SuperTriggerCell> STCs;
  std::vector<l1t::HGCalTriggerCell> trigCellVecInputEnlarged;


  // first pass, fill the super trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) continue;
    STCs[getSuperTriggerCellId(tc.detId())].add(tc);
    trigCellVecInputEnlarged.push_back ( tc );
  }

  //  std::cout << trigCellVecInputEnlarged.size() << " - " << trigCellVecInput.size() << std::endl;

  //If we want to find and create the missing TCs (i.e. TCs with pT = 0)
  //   std::cout << "** - " << STCs.size() << std::endl;
  
  for (auto& s: STCs){
    bool satisfy = false;
    //    std::cout << s.first << " - " << s.second.GetNTCs() << std::endl;
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

	    l1t::HGCalTriggerCell triggerCell;
	    GlobalPoint point = triggerTools_.getTCPosition(newtc);
	    math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);
	    triggerCell.setPosition(point);
	    triggerCell.setP4(p4);
	    triggerCell.setDetId(newtc);

	    s.second.add( triggerCell );
	    trigCellVecInputEnlarged.push_back ( triggerCell );
	    
	  }

	}
      }
    }

    else if (stcSize_.at(thickness) ==  2){

      for ( int i = 0; i < 2; i++ ){

	  satisfy = false;
	  for (auto tc:s.second.GetTCList() ){
	    HGCalDetId TC_idV8(tc);
	    if ( ((TC_idV8.cell() & 1) == i)){
	      
	      // std::bitset<32> x(tc);
	      // std::cout << "existing TC: "<<x << " --- " << (TC_idV8.cell() & 1) <<  std::endl;
	      satisfy = true;
	    }
	  }
	  if ( satisfy == false ){//Create new TC
	    int tc_base = s.second.GetTCList().at(0);
	    //Clear relevant bits	    
	    tc_base = tc_base & ~(1); 
	    //Set bits based on i value
	    int newtc = tc_base | i;
	    
	    // std::bitset<32> x(newtc);
	    // std::cout <<  "missing TC: " << x  << " - " << i <<  std::endl;
	    
	    l1t::HGCalTriggerCell triggerCell;
	    GlobalPoint point = triggerTools_.getTCPosition(newtc);
	    math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);
	    triggerCell.setPosition(point);
	    triggerCell.setP4(p4);
	    triggerCell.setDetId(newtc);
	    
	    s.second.add( triggerCell );
	    trigCellVecInputEnlarged.push_back ( triggerCell );
	      
	  }
	  
      }
      
    }
    
    
    

    

  }



  
  //  std::cout << "Enlarge: " << trigCellVecInputEnlarged.size() << " - " << trigCellVecInput.size() << std::endl;  
  // second pass, write them out
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {

  //  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    
    //If scintillator use a simple threshold cut
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      const auto & stc = STCs[getSuperTriggerCellId(tc.detId())]; 
      //      if (tc.detId() == stc.GetMaxId() ) { //Standard selection

      // std::bitset<32> x(tc.detId());
      // std::cout << x << std::endl;
      if (!(tc.detId() & 1)) { //Selecting even TCs (for the stc-2 case)
        trigCellVecOutput.push_back( tc );
        stc.assignEnergy(trigCellVecOutput.back());
      }

      //if tc
    }
    
  } // end of second loop

}
