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
        if ( stc!=kSTCsizeFine_ && stc!=kSTCsizeCoarse_ && stc!=kSTCsizeMid_ && stc!=kSTCsizeVeryFine_){
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
	      
	      l1t::HGCalTriggerCell triggerCell;
	      GlobalPoint point = triggerTools_.getTCPosition(newtc);
	      math::PtEtaPhiMLorentzVector p4(0, point.eta(), point.phi(), 0.);
	      triggerCell.setPosition(point);
	      triggerCell.setP4(p4);
	      triggerCell.setDetId(newtc);
	      
	      s.second.add( triggerCell );
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
	    
	    s.second.add( triggerCell );
	    
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
		
		s.second.add( triggerCell );
		
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
superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 
  
  std::unordered_map<unsigned,SuperTriggerCell> STCs;
  std::vector<l1t::HGCalTriggerCell> trigCellVecInputEnlarged;

  //  int thickness = 0;
  // first pass, fill the super trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    trigCellVecInputEnlarged.push_back ( tc );
    if (tc.subdetId() == HGCHEB) continue;
    STCs[getSuperTriggerCellId(tc.detId())].add(tc);

    //    thickness = triggerTools_.thicknessIndex(tc.detId(),true);
  }

  // //  bool doCoarseTCs = false;
  // bool doCoarseTCs = false; //Would have a configurable option if it was the correct way to go.
  // //If we want to create the coarse TCs

  
  // if ( thickness = > 1 ){
  //   doCoarseTCs = true;
  // }

  ///  if ( doCoarseTCs ){


  createMissingTriggerCells( STCs, trigCellVecInputEnlarged);
    ///  }







    // second pass, write them out
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInputEnlarged) {
    int thickness = triggerTools_.thicknessIndex(tc.detId(),true);
    //If scintillator use a simple threshold cut
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      const auto & stc = STCs[getSuperTriggerCellId(tc.detId())]; 

      //Easy case - STC
      
      //If thick then coarsen 
      if ( thickness > 1 ){
	
	
	if (!(tc.detId() & 1)) { //if even	  
	  if ( !stc.rejected() ) {
	    

	    // if (  (tc.detId() == stc.GetMaxId())
	    // 	  ||  ((tc.detId() | 1) == stc.GetMaxId()))
	    //   {
		
		trigCellVecOutput.push_back( tc );
		//		stc.assignEnergy(trigCellVecOutput.back(), "STC");
		stc.assignEnergy(trigCellVecOutput.back(), "EqualShare");
		
		//	      }
	   
	  }
	  
	}
      }
      else{
	
	//	if (  tc.detId() == stc.GetMaxId()  ){
	  
	  trigCellVecOutput.push_back( tc );
	  stc.assignEnergy(trigCellVecOutput.back(), "EqualShare");
	  
	  //	}
	
      }
      
    }
    
  }
  //     //If not thick



  //     if (tc.detId() == stc.GetMaxId() ) { //Standard selection


  //     }

  //     // //If selecting the max id
  //     // if ( !doCoarseTCs ){
  //     // 	if (tc.detId() == stc.GetMaxId() ) { //Standard selection
  //     // 	  trigCellVecOutput.push_back( tc );
  //     // 	  stc.assignEnergy(trigCellVecOutput.back());
  //     // 	}
  //     // }
  //     // else{
  //     // //If selecting the even id (currently only works for coarse-2)
  //     // 	if (!(tc.detId() & 1)) { 
  //     // 	  if ( !stc.rejected() ) { //Selecting even TCs (for the stc-2 case)
  //     // 	    trigCellVecOutput.push_back( tc );
  //     // 	    stc.assignEnergy(trigCellVecOutput.back());
  //     // 	  }
  //     // 	}
  //     // }










  //   }

    
  // } // end of second loop


}
