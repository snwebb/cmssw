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
      int TC_cell = TC_idV9.triggerCellU() << kThree_ | TC_idV9.triggerCellV();
      
      int TC_12th = ( TC_cell & kSplit12_v9_1 ) << kTwo_;
      int TC_3rd = 0;

      if ( TC_idV9.triggerCellU() > kThree_ || TC_idV9.triggerCellV() > kThree_ ){

        if ( TC_idV9.triggerCellU() <=  TC_idV9.triggerCellV() ){
          TC_12th = ( ( TC_cell & kSplit12_v9_2 ) << kTwo_ ) | kOne_;
          TC_3rd = kOne_;
        }
        else{
          TC_12th = ( ( TC_cell & kSplit12_v9_3 ) << kTwo_ ) | kTwo_;
          TC_3rd = kTwo_;
        }
      }
      int TC_split = TC_12th;


      if (stcSize_.at(thickness) == kSTCsize16_){
        TC_split =  TC_3rd;
      }

      std::cout << TC_split << " - " << TC_idV9.triggerCellU() <<  " - " << TC_idV9.triggerCellV() << std::endl;
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
