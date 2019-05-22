#include "L1Trigger/L1THGCal/interface/HGCalCoarseTriggerCellMapping.h"
#include "DataFormats/ForwardDetId/interface/HGCalTriggerDetId.h"


HGCalCoarseTriggerCellMapping::
HGCalCoarseTriggerCellMapping(const std::vector<unsigned>& ctcSize )
  :  ctcSize_(ctcSize.size()!=0 ? ctcSize : 
	      std::vector<unsigned>{2,2,2,2})
{
      if ( ctcSize_.size() != kNLayers_ ){
        throw cms::Exception("HGCTriggerParameterError")
            << "Inconsistent size of coarse trigger cell size vector" << ctcSize_.size() ;
    }

    for(auto ctc : ctcSize_) checkSizeValidity(ctc);

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
  DetId tc_Id( tcid );
  if ( tc_Id.det() == DetId::Forward ){//V8
    evenid = tcid & ~1;
  }
  else if ( tc_Id.det() == DetId::HGCalTrigger || tc_Id.det() == DetId::HGCalHSc ){//V9

    if( triggerTools_.isScintillator(tcid) ){

      HGCScintillatorDetId tc_IdV9(tcid);
      uint32_t newPhi = tc_IdV9.iphi() & ~1;
      evenid = tcid & ~(kHGCalScinCellMask_);
      evenid |= ( ((tc_IdV9.ietaAbs()& HGCScintillatorDetId::kHGCalRadiusMask) << HGCScintillatorDetId::kHGCalRadiusOffset ) |
		  ((newPhi & HGCScintillatorDetId::kHGCalPhiMask) << HGCScintillatorDetId::kHGCalPhiOffset ));

    }
    else{
      int uPrime = 0;
      int newU = 0;
      int newV = 0;
      HGCalTriggerDetId tc_IdV9(tcid);  
      int rocnum = detIdToROC_.getROCNumber( tc_IdV9.triggerCellU() , tc_IdV9.triggerCellV(), 1 );
      if ( rocnum == 1 ){
	uPrime = tc_IdV9.triggerCellU();
	newU = ( uPrime&~1 );
	newV = tc_IdV9.triggerCellV() - tc_IdV9.triggerCellU() + newU;
      }
      else if ( rocnum == 2 ){
	uPrime = tc_IdV9.triggerCellU() - tc_IdV9.triggerCellV()-1;
	newU = ( uPrime&~1 ) + tc_IdV9.triggerCellV()+1;
	newV = tc_IdV9.triggerCellV();
      }
      else if ( rocnum == 3 ){      
	uPrime = tc_IdV9.triggerCellV() - kRotate4_;      
	newU = tc_IdV9.triggerCellU();
	newV = ( uPrime&~1 ) + kRotate4_;
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
  int ctcSize = ctcSize_.at(thickness);

  DetId tc_Id( detid );
  if ( tc_Id.det() == DetId::Forward ){//V8

    HGCalDetId tc_IdV8(detid);

    if( triggerTools_.isScintillator(detid) ){
      return detid; //stc not available in scintillator for v8
    }
    else{
      int TC_split = (tc_IdV8.cell() & kSplit_.at( ctcSize ) );
      detid =  (detid & ~(HGCalDetId::kHGCalCellMask ) ) | TC_split;
      return detid;
    }

  }

  else if ( tc_Id.det() == DetId::HGCalTrigger || tc_Id.det() == DetId::HGCalHSc ){//V9
    if( triggerTools_.isScintillator(detid) ){
      HGCScintillatorDetId tc_IdV9(detid);

      int TC_split = ((tc_IdV9.ietaAbs() << HGCScintillatorDetId::kHGCalRadiusOffset) | tc_IdV9.iphi()) & kSplit_v9_Scin_.at( ctcSize );
      detid =  (detid & ~( kHGCalScinCellMask_ ) ) | TC_split;

      return detid;

    }
    else {
      HGCalTriggerDetId tc_IdV9(detid);

      int uPrime = 0;
      int vPrime = 0;
      int rocnum = detIdToROC_.getROCNumber( tc_IdV9.triggerCellU() , tc_IdV9.triggerCellV(), 1 );

      if ( rocnum == 1 ){

          uPrime = tc_IdV9.triggerCellU();
          vPrime = tc_IdV9.triggerCellV() - tc_IdV9.triggerCellU();

      }
      else if ( rocnum == 2 ){

          uPrime = tc_IdV9.triggerCellU()-tc_IdV9.triggerCellV()-1;
          vPrime = tc_IdV9.triggerCellV();

      }
      else if ( rocnum == 3 ){

          uPrime = tc_IdV9.triggerCellV() - kRotate4_;
          vPrime = kRotate7_ - tc_IdV9.triggerCellU();

      }

      int TC_split =  (rocnum << kRocShift_) | ( (uPrime << kUShift_ | vPrime) & kSplit_v9_.at( ctcSize ) );
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
  int ctcSize = ctcSize_.at(thickness);

  std::vector<uint32_t> output_ids;
  DetId tc_Id( ctcId );

  if ( tc_Id.det() == DetId::Forward ){//V8  

    int SplitInv = ~( (~kSTCidMask_) | kSplit_.at ( ctcSize ) );

    for ( int i = 0; i < SplitInv + 1 ; i++ ){
      if (  (i & SplitInv)!=i  )  continue; 

      output_ids.emplace_back( ctcId | i );

    }

  }
  else if ( tc_Id.det() == DetId::HGCalTrigger || tc_Id.det() == DetId::HGCalHSc ){//V9


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
	std::bitset<32> x(ctcId);
	int uPrime = ((ctcId|i) & kUMask_) >> kUShift_; 
	int vPrime = ((ctcId|i) & kVMask_) >> kVShift_;
	int rocnum = (ctcId & kRocMask_) >> kRocShift_;

	int u = 0;
	int v = 0;
	
	if ( rocnum == 1 ){
	  u = uPrime;
	  v = vPrime + u;
	}
	else if ( rocnum == 2 ){
	  u = uPrime + vPrime + 1;
	  v = vPrime;
	}    
	else if ( rocnum == 3 ){
	  u = kRotate7_ - vPrime;
	  v = uPrime + kRotate4_;
	}
	
	uint32_t outid = ctcId & ~(kHGCalCellMaskV9_);
	outid |= ( ((u & kHGCalCellUMask_) << kHGCalCellUOffset_ ) |
		   ((v & kHGCalCellVMask_) << kHGCalCellVOffset_ ));
	
	output_ids.emplace_back( outid );
	
      }
    }
  }
  return output_ids;
  
  
}


GlobalPoint
HGCalCoarseTriggerCellMapping::
getCoarseTriggerCellPosition( uint32_t tcid ) const
{ 

     std::vector<uint32_t> constituentTCs = getConstituentTriggerCells ( getCoarseTriggerCellId( tcid ) );
     Basic3DVector<float> average_vector(0., 0., 0.);

     for ( const auto constituent : constituentTCs ){
       average_vector += triggerTools_.getTCPosition(constituent).basicVector();
     }

     GlobalPoint average_point( average_vector / constituentTCs.size() ) ;     

     return average_point;

}


