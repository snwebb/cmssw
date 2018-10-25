#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSelectionImpl.h"

HGCalConcentratorSelectionImpl::
HGCalConcentratorSelectionImpl(const edm::ParameterSet& conf):
  nData_(conf.getParameter<uint32_t>("NData")),
  nCellsInModule_(conf.getParameter<uint32_t>("MaxCellsInModule")),
  linLSB_(conf.getParameter<double>("linLSB")),
  adcsaturationBH_(conf.getParameter<double>("adcsaturationBH")),
  adcnBitsBH_(conf.getParameter<uint32_t>("adcnBitsBH")),
  TCThreshold_fC_(conf.getParameter<double>("TCThreshold_fC")),
  TCThresholdBH_MIP_(conf.getParameter<double>("TCThresholdBH_MIP")),
  triggercell_threshold_silicon_( conf.getParameter<double>("triggercell_threshold_silicon") ),
  triggercell_threshold_scintillator_( conf.getParameter<double>("triggercell_threshold_scintillator"))
{
  // Cannot have more selected cells than the max number of cells
  if(nData_>nCellsInModule_) nData_ = nCellsInModule_;
  adcLSBBH_ =  adcsaturationBH_/pow(2.,adcnBitsBH_);
  TCThreshold_ADC_ = (int) (TCThreshold_fC_ / linLSB_);
  TCThresholdBH_ADC_ = (int) (TCThresholdBH_MIP_ / adcLSBBH_);
}

void 
HGCalConcentratorSelectionImpl::
thresholdSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 
  for (size_t i = 0; i<trigCellVecInput.size();i++){

    int threshold = (HGCalDetId(trigCellVecInput[i].detId()).subdetId()==ForwardSubdetector::HGCHEB ? TCThresholdBH_ADC_ : TCThreshold_ADC_);
    double triggercell_threshold = (HGCalDetId(trigCellVecInput[i].detId()).subdetId()==HGCHEB ? triggercell_threshold_scintillator_ : triggercell_threshold_silicon_);
  
    if ((trigCellVecInput[i].hwPt() >= threshold) && (trigCellVecInput[i].mipPt() >= triggercell_threshold)){ 
      trigCellVecOutput.push_back(trigCellVecInput[i]);      
    }  
  }
}

void 
HGCalConcentratorSelectionImpl::
bestChoiceSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 
  trigCellVecOutput = trigCellVecInput;    
  // sort, reverse order
  sort(trigCellVecOutput.begin(), trigCellVecOutput.end(),
       [](const l1t::HGCalTriggerCell& a, 
          const  l1t::HGCalTriggerCell& b) -> bool
  { 
    return a.hwPt() > b.hwPt(); 
  } 
  );

  // keep only the first trigger cells
  if(trigCellVecOutput.size()>nData_) trigCellVecOutput.resize(nData_);
  
}


l1t::HGCalSuperTriggerCellMap*
HGCalConcentratorSelectionImpl::
getSuperTriggerCell_(uint32_t module_id, l1t::HGCalTriggerCell TC)
{

  HGCalDetId TC_id( TC.detId() );
  int TC_wafer = TC_id.wafer();

  int TC_12th = ( TC_id.cell() & 0x3a );

  //  int threshold = (TC_id.subdetId()==ForwardSubdetector::HGCHEB ? TCThresholdBH_ADC_ : TCThreshold_ADC_);

  //int TC_3rd = ((TC_id.cell())>>4)&0x3;//old
  //int TC_3rd = ((TC_id.cell())>>2)&0xf;//new
  //  int TC_3rd = 1;
  //  int TC_16th = ((TC_id.cell())>>2)&0xf;
  // int TC_12th = ((TC_id.cell())>>2)&0xf;
  // int TC_half = ((TC_id.cell())>>2)&0x1;

  //  std::cout << TC_id.cell() << " - " << TC_12th << std::endl;

  // if (TC_12th & 0x1) TC_12th += TC_half;
  // else{
  //   TC_12th -= TC_half;
  // }


  // if ( ! TC_half  ){ //if upperhalf?
    
  //   if  (  (TC_12th & 0x1)  ) TC_12th -= 1;
  //   if  (! (TC_12th & 0x1)  ) TC_12th += 1;
    
  // }
  
  //  std::cout << "finalval = " << TC_12th << std::endl;



  
  long SuperTriggerCellMap_id = 0;
  if(TC_id.subdetId()==HGCHEB) SuperTriggerCellMap_id = TC_id.cell(); //scintillator? maybe fine for now? //not fully, not sure how to select it though
  //


  //  else SuperTriggerCellMap_id = TC_3rd + (TC_wafer<<2);//old
  //  else SuperTriggerCellMap_id = TC_12th + (TC_wafer<<4);

  else SuperTriggerCellMap_id = TC_wafer<<6 | TC_12th;
  // else SuperTriggerCellMap_id = TC_3rd + (TC_wafer<<4);//new
  //  int wafout = (TC_wafer<<2);
  //  std::cout << "id = " << TC_id.cell() << " - " << TC_wafer << " - " << wafout  << " - " << module_id << " - " << SuperTriggerCellMap_id << std::endl;


  //  std::cout << "id =  " <<  TC_id.subdetId() << " - " << TC_id.cell() << " - " <<  TC_wafer << " - " << module_id << " - " << SuperTriggerCellMap_id << std::endl;
  // if ( mapSuperTriggerCellMap_[module_id].count(SuperTriggerCellMap_id) )
  //   std::cout << "STC being created" << std::endl;  
  // else{
  //   std::cout << "STC already exists" << std::endl;    
  // }

  return &mapSuperTriggerCellMap_[module_id][SuperTriggerCellMap_id];

}

void 
HGCalConcentratorSelectionImpl::
clearSuperTriggerCellMap(){
  mapSuperTriggerCellMap_.clear();
}

void 
HGCalConcentratorSelectionImpl::
superTriggerCellSelectImpl(uint32_t module_id ,const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  //Clear SuperTriggerCell Map
  //  maybe create a supertriggercellmap here
  this->clearSuperTriggerCellMap();



  for (size_t i = 0; i<trigCellVecInput.size();i++){

 
    //    std::cout << getSuperTriggerCell_( trigCellVecInput.at(i) )->getNumberOfConstituentCells() << std::endl;

    // std::cout << "--" << std::endl;
  
    //    if ( trigCellVecInput.at(i).hwPt() < threshold)    trigCellVecOutput.at(i).setHwPt(0);

    getSuperTriggerCell_( module_id, trigCellVecInput.at(i) )->addTriggerCell( trigCellVecInput.at(i) );


    //std::cout << "1st " << getSuperTriggerCell_( module_id, trigCellVecInput.at(i) )->getNumberOfConstituentCells() << std::endl;
  }
    
  for (size_t i = 0; i<trigCellVecInput.size();i++){

    //If scintillator use a simple threshold cut
    if ( HGCalDetId(trigCellVecInput[i].detId()).subdetId()==HGCHEB ){

      if  ( ( trigCellVecInput[i].hwPt() >= TCThresholdBH_ADC_ ) && (trigCellVecInput[i].mipPt() >= triggercell_threshold_scintillator_ ) ){

	trigCellVecOutput.push_back( trigCellVecInput.at(i) );
	//	trigCellVecOutput.back().setNSTC( 99 );      

      }

    }

    else{


   
      l1t::HGCalSuperTriggerCellMap* TCmap = getSuperTriggerCell_( module_id, trigCellVecInput.at(i) );    

      
      //Check if TC is the most energetic in towerMap and assign the full hwPt of the towerMap
      //Else zeroed
      

       
      if(TCmap->maxTriggerCell().detId() == trigCellVecInput.at(i).detId()) {
	
	double avedist =  TCmap->findAverageDistanceBetweenCells() ;
	int NSTC = TCmap->triggerCells().size();      //temp
	//	int temp = TCmap->triggerCells().size();      //temp
	//	int NSTC = TCmap->getNumberOfConstituentCells();      //temp
	
	//	std::cout << temp << " - " << NSTC << std::endl;

	// if ( avedist > 40 ){
	// 	std::cout << "---- " << TCmap->maxTriggerCell().layer() << std::endl;
	// 	std::cout << "---- " << trigCellVecInput.at(i).layer() << std::endl;
	// }

	// if ( trigCellVecInput.at(i).subdetId() > 4 ) 
	//   std::cout << "HERE " << trigCellVecInput.at(i).subdetId() << std::endl;
	
	trigCellVecOutput.push_back( trigCellVecInput.at(i) );
	trigCellVecOutput.back().setHwPt(TCmap->hwPt());
	
	//User
	trigCellVecOutput.back().setNSTC( NSTC );
	trigCellVecOutput.back().setAveDist( avedist );
	
	
	// if ( avedist > 40 ){
	// 	std::cout << "2---- " << trigCellVecOutput.back().layer() << std::endl;
	// 	std::cout << "2---- " << trigCellVecOutput.back().layer() << std::endl;
	// }
	
	
      }

    }
    
    
  }  
  
  
}
