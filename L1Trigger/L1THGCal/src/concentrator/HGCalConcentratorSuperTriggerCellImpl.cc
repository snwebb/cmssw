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
        if ( stc!=kSTCsize4_ && stc!=kSTCsize16_ ){
            throw cms::Exception("HGCTriggerParameterError")
              << "Super Trigger Cell should be of size 4 or 16" ;
        }
    }
    
}

std::map<std::pair<int,int>,int> HGCalConcentratorSuperTriggerCellImpl::stcMap4={

  {std::make_pair(0,1),0}, {std::make_pair(1,1),0},
  {std::make_pair(0,0),0}, {std::make_pair(1,0),0},
  {std::make_pair(0,3),1}, {std::make_pair(1,3),1},
  {std::make_pair(0,2),1}, {std::make_pair(1,2),1},
  {std::make_pair(2,3),2}, {std::make_pair(3,3),2},
  {std::make_pair(2,2),2}, {std::make_pair(3,2),2},
  {std::make_pair(2,1),3}, {std::make_pair(3,1),3},
  {std::make_pair(2,0),3}, {std::make_pair(3,0),3},

  {std::make_pair(2,5),4}, {std::make_pair(3,5),4},
  {std::make_pair(1,4),4}, {std::make_pair(2,4),4},
  {std::make_pair(4,7),5}, {std::make_pair(5,7),5},
  {std::make_pair(3,6),5}, {std::make_pair(4,6),5},
  {std::make_pair(6,7),6}, {std::make_pair(7,7),6},
  {std::make_pair(5,6),6}, {std::make_pair(6,6),6},
  {std::make_pair(4,5),7}, {std::make_pair(5,5),7},
  {std::make_pair(3,4),7}, {std::make_pair(4,4),7},

  {std::make_pair(4,1),8}, {std::make_pair(5,2),8},
  {std::make_pair(4,0),8}, {std::make_pair(5,1),8},
  {std::make_pair(4,3),9}, {std::make_pair(5,4),9},
  {std::make_pair(4,2),9}, {std::make_pair(5,3),9},
  {std::make_pair(6,5),10}, {std::make_pair(7,6),10},
  {std::make_pair(6,4),10}, {std::make_pair(7,5),10},
  {std::make_pair(6,3),11}, {std::make_pair(7,4),11},
  {std::make_pair(6,2),11}, {std::make_pair(7,3),11},

};


std::map<std::pair<int,int>,int> HGCalConcentratorSuperTriggerCellImpl::stcMap16={

  {std::make_pair(0,1),0}, {std::make_pair(1,1),0},
  {std::make_pair(0,0),0}, {std::make_pair(1,0),0},
  {std::make_pair(0,3),0}, {std::make_pair(1,3),0},
  {std::make_pair(0,2),0}, {std::make_pair(1,2),0},
  {std::make_pair(2,3),0}, {std::make_pair(3,3),0},
  {std::make_pair(2,2),0}, {std::make_pair(3,2),0},
  {std::make_pair(2,1),0}, {std::make_pair(3,1),0},
  {std::make_pair(2,0),0}, {std::make_pair(3,0),0},

  {std::make_pair(2,5),1}, {std::make_pair(3,5),1},
  {std::make_pair(1,4),1}, {std::make_pair(2,4),1},
  {std::make_pair(4,7),1}, {std::make_pair(5,7),1},
  {std::make_pair(3,6),1}, {std::make_pair(4,6),1},
  {std::make_pair(6,7),1}, {std::make_pair(7,7),1},
  {std::make_pair(5,6),1}, {std::make_pair(6,6),1},
  {std::make_pair(4,5),1}, {std::make_pair(5,5),1},
  {std::make_pair(3,4),1}, {std::make_pair(4,4),1},

  {std::make_pair(4,1),2}, {std::make_pair(5,2),2},
  {std::make_pair(4,0),2}, {std::make_pair(5,1),2},
  {std::make_pair(4,3),2}, {std::make_pair(5,4),2},
  {std::make_pair(4,2),2}, {std::make_pair(5,3),2},
  {std::make_pair(6,5),2}, {std::make_pair(7,6),2},
  {std::make_pair(6,4),2}, {std::make_pair(7,5),2},
  {std::make_pair(6,3),2}, {std::make_pair(7,4),2},
  {std::make_pair(6,2),2}, {std::make_pair(7,3),2},

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
      int TC_12th = ( TC_idV8.cell() & kSplit12_ );
      int TC_3rd = ( TC_idV8.cell() & kSplit3_ );
      
      int thickness = triggerTools_.thicknessIndex(detid,true);
      int TC_split = TC_12th;
      if (stcSize_.at(thickness) == kSTCsize16_) TC_split = TC_3rd;
      
      return TC_wafer<<kWafer_offset_ | TC_split;
    }

  }

  else if ( TC_id.det() == DetId::HGCalTrigger ){//V9
    
    HGCalTriggerDetId TC_idV9(detid);

    if( triggerTools_.isScintillator(detid) ){
      return TC_idV9.triggerCellU() << kWafer_offset_ | TC_idV9.triggerCellV(); //scintillator
    }
    else {
      int TC_wafer = TC_idV9.waferU() << kWafer_offset_ | TC_idV9.waferV() ;
      int thickness = triggerTools_.thicknessIndex(detid);
      
      int sTC_idV9 =  stcMap4[ std::make_pair( TC_idV9.triggerCellU(),TC_idV9.triggerCellV() )   ];
      
      if (stcSize_.at(thickness) == kSTCsize16_){
	sTC_idV9 =  stcMap16[ std::make_pair( TC_idV9.triggerCellU(),TC_idV9.triggerCellV() )   ];
      }
      return TC_wafer<<kWafer_offset_ | sTC_idV9;
      
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

  // first pass, fill the super trigger cells
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    if (tc.subdetId() == HGCHEB) continue;
    STCs[getSuperTriggerCellId(tc.detId())].add(tc);
  }
    
  // second pass, write them out
  for (const l1t::HGCalTriggerCell & tc : trigCellVecInput) {
    
    //If scintillator use a simple threshold cut
    if (tc.subdetId() == HGCHEB) {
      trigCellVecOutput.push_back( tc );
    } else {
      const auto & stc = STCs[getSuperTriggerCellId(tc.detId())]; 
      if (tc.detId() == stc.GetMaxId() ) {
        trigCellVecOutput.push_back( tc );
        stc.assignEnergy(trigCellVecOutput.back());
      }
    }
    
  } // end of second loop
  
}
