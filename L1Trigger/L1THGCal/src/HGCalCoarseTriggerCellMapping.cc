#include "L1Trigger/L1THGCal/interface/HGCalCoarseTriggerCellMapping.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"


HGCalCoarseTriggerCellMapping::
HGCalCoarseTriggerCellMapping(std::vector<unsigned> stcSize )
  :  stcSize_(stcSize.size()!=0 ? stcSize : 
	      std::vector<unsigned>{2,2,2,2})
{
      if ( stcSize_.size() != kNLayers_ ){
        throw cms::Exception("HGCTriggerParameterError")
            << "Inconsistent size of coarse trigger cell size vector" << stcSize_.size() ;
    }

    for(auto stc : stcSize_) checkSizeValidity(stc);

}


const std::map<int,int> 
HGCalCoarseTriggerCellMapping::kSplit_ = {
  {kCTCsizeIndividual_, kSplit_v8_Individual_},
  {kCTCsizeVeryFine_, kSplit_v8_VeryFine_},
  {kCTCsizeFine_, kSplit_v8_Fine_},
  {kCTCsizeMid_, kSplit_v8_Mid_},
  {kCTCsizeCoarse_, kSplit_v8_Coarse_}
};

const std::map<int,int> 
HGCalCoarseTriggerCellMapping::kSplit_v9_ = {
  {kCTCsizeIndividual_, kSplit_v9_Individual_},
  {kCTCsizeVeryFine_, kSplit_v9_VeryFine_},
  {kCTCsizeFine_, kSplit_v9_Fine_},
  {kCTCsizeMid_, kSplit_v9_Mid_},
  {kCTCsizeCoarse_, kSplit_v9_Coarse_},
};

const std::map<int,int> 
HGCalCoarseTriggerCellMapping::kSplit_v9_Scin_ = {
  {kCTCsizeIndividual_, kSplit_v9_Scin_Individual_},
  {kCTCsizeVeryFine_, kSplit_v9_Scin_VeryFine_},
  {kCTCsizeFine_, kSplit_v9_Scin_Fine_},
  {kCTCsizeMid_, kSplit_v9_Scin_Mid_},
  {kCTCsizeCoarse_, kSplit_v9_Scin_Coarse_},
};

void
HGCalCoarseTriggerCellMapping::checkSizeValidity(int ctcSize )const{
  if ( ctcSize!=kCTCsizeFine_ && ctcSize!=kCTCsizeCoarse_ && ctcSize!=kCTCsizeMid_ && ctcSize!=kCTCsizeVeryFine_ && ctcSize!=kCTCsizeIndividual_){
    throw cms::Exception("HGCTriggerParameterError")
      << "Coarse Trigger Cell should be of size "<<
      kCTCsizeIndividual_ << " or " << kCTCsizeVeryFine_ << " or " << kCTCsizeFine_ << 
      " or " << kCTCsizeMid_ << " or " << kCTCsizeCoarse_;
  }
}

void
HGCalCoarseTriggerCellMapping:: setEvenDetId(l1t::HGCalTriggerCell &c) const {
  c.setDetId( getEvenDetId( c.detId() ) );
}

uint32_t
HGCalCoarseTriggerCellMapping:: getEvenDetId(uint32_t tcid) const {
  
  uint32_t evenid = 0;
  DetId TC_id( tcid );
  if ( TC_id.det() == DetId::Forward ){//V8
    evenid = tcid & ~1;
  }
  else if ( TC_id.det() == DetId::HGCalTrigger || TC_id.det() == DetId::HGCalHSc ){//V9

    if( triggerTools_.isScintillator(tcid) ){

      HGCScintillatorDetId TC_idV9(tcid);
      int newPhi = TC_idV9.iphi() & ~1;
      evenid = tcid & ~(kHGCalScinCellMask_);
      evenid |= ( ((TC_idV9.ietaAbs()& HGCScintillatorDetId::kHGCalRadiusMask) << HGCScintillatorDetId::kHGCalRadiusOffset ) |
		  ((newPhi & HGCScintillatorDetId::kHGCalPhiMask) << HGCScintillatorDetId::kHGCalPhiOffset ));

    }
    else{
      int Uprime = 0;
      int newU = 0;
      int newV = 0;
      HGCalTriggerDetId TC_idV9(tcid);  
      int rocnum = detIdToROC_.getROCNumber( TC_idV9.triggerCellU() , TC_idV9.triggerCellV(), 1 );
      if ( rocnum == 1 ){
	Uprime = TC_idV9.triggerCellU();
	newU = ( Uprime&~1 );
	newV = TC_idV9.triggerCellV() - TC_idV9.triggerCellU() + newU;
      }
      else if ( rocnum == 2 ){
	Uprime = TC_idV9.triggerCellU() - TC_idV9.triggerCellV()-1;
	newU = ( Uprime&~1 ) + TC_idV9.triggerCellV()+1;
	newV = TC_idV9.triggerCellV();
      }
      else if ( rocnum == 3 ){      
	Uprime = TC_idV9.triggerCellV() - kRotate4_;      
	newU = TC_idV9.triggerCellU();
	newV = ( Uprime&~1 ) + kRotate4_;
      }
      
      evenid = tcid & ~(kHGCalCellMaskV9_);
      evenid |= ( ((newU & kHGCalCellUMask_) << kHGCalCellUOffset_ ) |
		  ((newV & kHGCalCellVMask_) << kHGCalCellVOffset_ ));

    }
  
  }

  return evenid;
}

uint32_t
HGCalCoarseTriggerCellMapping::getCoarseTriggerCellId(uint32_t detid) const {

  int thickness = 0;
  if ( triggerTools_.isSilicon(detid)){
    thickness = triggerTools_.thicknessIndex(detid,true);
  }else if ( triggerTools_.isScintillator(detid)){
    thickness = 3;
  }

  int ctcSize = stcSize_.at(thickness);

  DetId TC_id( detid );
  if ( TC_id.det() == DetId::Forward ){//V8

    HGCalDetId TC_idV8(detid);

    if( triggerTools_.isScintillator(detid) ){
      return detid; //stc not available in scintillator for v8
    }
    else{
      int TC_split = (TC_idV8.cell() & kSplit_.at( ctcSize ) );
      detid =  (detid & ~(HGCalDetId::kHGCalCellMask ) ) | TC_split;
      return detid;
    }

  }

  else if ( TC_id.det() == DetId::HGCalTrigger || TC_id.det() == DetId::HGCalHSc ){//V9
    if( triggerTools_.isScintillator(detid) ){
      HGCScintillatorDetId TC_idV9(detid);

      int TC_split = ((TC_idV9.ietaAbs() << HGCScintillatorDetId::kHGCalRadiusOffset) | TC_idV9.iphi()) & kSplit_v9_Scin_.at( ctcSize );
      detid =  (detid & ~( kHGCalScinCellMask_ ) ) | TC_split;

      return detid;
      //      return TC_idV9.ietaAbs() << kScinEtaOffset_ | TC_idV9.iphi(); //scintillator
    }
    else {
      HGCalTriggerDetId TC_idV9(detid);

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

          Uprime = TC_idV9.triggerCellV() - kRotate4_;
          Vprime = kRotate7_ - TC_idV9.triggerCellU();

      }

      int TC_split =  (rocnum << kRocShift_) | ( (Uprime << kUShift_ | Vprime) & kSplit_v9_.at( ctcSize ) );

      detid =  (detid & ~( kHGCalCellMaskV9_ ) ) | TC_split;

      return detid;
      
    }
        
  }
  else{
    return 0;
  }
  
}

std::vector<uint32_t>
HGCalCoarseTriggerCellMapping::
getConstituentTriggerCells( uint32_t ctcId ) const
{ 

  int thickness = 0;
  if ( triggerTools_.isSilicon(ctcId)){
    thickness = triggerTools_.thicknessIndex(ctcId,true);
  }else if ( triggerTools_.isScintillator(ctcId)){
    thickness = 3;
  }
  int ctcSize = stcSize_.at(thickness);

  std::vector<uint32_t> output_ids;
  DetId TC_id( ctcId );

  if ( TC_id.det() == DetId::Forward ){//V8  

    int SplitInv = ~( (~kSTCidMask_) | kSplit_.at ( ctcSize ) );

    for ( int i = 0; i < SplitInv + 1 ; i++ ){
      if (  (i & SplitInv)!=i  )  continue; 

      output_ids.emplace_back( ctcId | i );

    }

  }
  else if ( TC_id.det() == DetId::HGCalTrigger || TC_id.det() == DetId::HGCalHSc ){//V9


    if( triggerTools_.isScintillator( ctcId ) ){

      int SplitInv = ~( (~kHGCalScinCellMask_) | kSplit_v9_Scin_.at ( ctcSize ) );
      for ( int i = 0; i < SplitInv + 1 ; i++ ){
	if (  (i & SplitInv)!=i  )  continue; 

	output_ids.emplace_back( ctcId | i );

      }      

    }
    else{
    
      int SplitInv = ~( (~kSTCidMask_v9_) | kSplit_v9_.at ( ctcSize ) );
      for ( int i = 0; i < SplitInv + 1 ; i++ ){
	if (  (i & SplitInv)!=i  )  continue; 
	
	int Uprime = (i & kUMask_) >> kUShift_; 
	int Vprime = (i & kVMask_) >> kVShift_;
	int rocnum = (ctcId & kRocMask_) >> kRocShift_;
	int U = 0;
	int V = 0;
	
	if ( rocnum == 1 ){
	  U = Uprime;
	  V = Vprime + U;
	}
	else if ( rocnum == 2 ){
	  U = Uprime + Vprime + 1;
	  V = Vprime;
	}    
	else if ( rocnum == 3 ){
	  U = kRotate7_ - Vprime;
	  V = Uprime + kRotate4_;
	}
	
	ctcId &= ~(kHGCalCellMaskV9_);
	ctcId |= ( ((U & kHGCalCellUMask_) << kHGCalCellUOffset_ ) |
		   ((V & kHGCalCellVMask_) << kHGCalCellVOffset_ ));
	
	output_ids.emplace_back( ctcId );
	
      }
    }
  }
  return output_ids;
  
  
}


void 
HGCalCoarseTriggerCellMapping::
setCoarseTriggerCellPosition( l1t::HGCalTriggerCell& tc ) const
{ 

     std::vector<uint32_t> constituentTCs = getConstituentTriggerCells ( getCoarseTriggerCellId(tc.detId() ));
     Basic3DVector<float> average_vector(0., 0., 0.);

     for ( const auto tc_id : constituentTCs ){
       average_vector += triggerTools_.getTCPosition(tc_id).basicVector();
     }

     GlobalPoint average_point( average_vector / constituentTCs.size() ) ;     
    
     math::PtEtaPhiMLorentzVector p4(tc.pt(), average_point.eta(), average_point.phi(), tc.mass());
     tc.setPosition( average_point );
     tc.setP4(p4);

}


