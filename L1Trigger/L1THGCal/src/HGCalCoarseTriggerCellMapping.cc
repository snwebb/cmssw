#include "L1Trigger/L1THGCal/interface/HGCalCoarseTriggerCellMapping.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"


HGCalCoarseTriggerCellMapping::
HGCalCoarseTriggerCellMapping()
{
}


const std::map<int,int> 
HGCalCoarseTriggerCellMapping::kSplit_ = {
  {kCTCsizeVeryFine_, kSplit_v8_VeryFine_},
  {kCTCsizeFine_, kSplit_v8_Fine_},
  {kCTCsizeMid_, kSplit_v8_Mid_},
  {kCTCsizeCoarse_, kSplit_v8_Coarse_}
};

const std::map<int,int> 
HGCalCoarseTriggerCellMapping::kSplit_v9_ = {
  {kCTCsizeVeryFine_, kSplit_v9_VeryFine_},
  {kCTCsizeFine_, kSplit_v9_Fine_},
  {kCTCsizeMid_, kSplit_v9_Mid_},
};

void
HGCalCoarseTriggerCellMapping::checkSizeValidity(int ctcSize )const{
  if ( ctcSize!=kCTCsizeFine_ && ctcSize!=kCTCsizeCoarse_ && ctcSize!=kCTCsizeMid_ && ctcSize!=kCTCsizeVeryFine_){
    throw cms::Exception("HGCTriggerParameterError")
      << "Coarse Trigger Cell should be of size "<<
      kCTCsizeVeryFine_ << " or " << kCTCsizeFine_ <<
      kCTCsizeMid_ << " or " << kCTCsizeCoarse_;
  }
}

int
HGCalCoarseTriggerCellMapping::getCoarseTriggerCellId(int detid, int ctcSize) const {

  checkSizeValidity (ctcSize);

  DetId TC_id( detid );
  if ( TC_id.det() == DetId::Forward ){//V8

    HGCalDetId TC_idV8(detid);

    if( triggerTools_.isScintillator(detid) ){
      return detid; //scintillator
    }
    else{
      int TC_split = (TC_idV8.cell() & kSplit_.at( ctcSize ) );
      detid =  (detid & ~(HGCalDetId::kHGCalCellMask ) ) | TC_split;
      return detid;
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

      if (ctcSize == kCTCsizeCoarse_){
	TC_split =  rocnum;
      }
      else{
	TC_split =  (rocnum << kRocShift_) | ( (Uprime << kUShift_ | Vprime) & kSplit_v9_.at( ctcSize ) );
      }

      return TC_wafer<<kWafer_offset_ | TC_split;
      
    }
        
  }
  else{
    return -1;
  }
  
}

void 
HGCalCoarseTriggerCellMapping::
getConstituentTriggerCells( int ctcId, int ctcSize, std::vector<int> & output_ids ) const
{ 
  
  int SplitInv = ~( (~kSTCidMask_) | kSplit_.at ( ctcSize ) );

  for ( int i = 0; i < SplitInv + 1 ; i++ ){
    if (  (i & SplitInv)!=i  )  continue; 

    output_ids.emplace_back( ctcId | i );

  }

}


void 
HGCalCoarseTriggerCellMapping::
setCoarseTriggerCellPosition( l1t::HGCalTriggerCell& tc )
{ 

     if (tc.subdetId() == HGCHEB) return;

     int detid = tc.detId();        
     int detid_odd = detid | 1;
     GlobalPoint point_even = triggerTools_.getTCPosition(detid);
     GlobalPoint point_odd = triggerTools_.getTCPosition(detid_odd);
     GlobalPoint average_point  ( (point_even.x()+point_odd.x())/2,(point_even.y()+point_odd.y())/2,(point_even.z()+point_odd.z())/2  );     

     math::PtEtaPhiMLorentzVector p4(tc.pt(), average_point.eta(), average_point.phi(), tc.mass());
     tc.setPosition( average_point );
     tc.setP4(p4);

}


