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
getSuperTriggerCell_(l1t::HGCalTriggerCell TC)
{

  HGCalDetId TC_id( TC.detId() );
  int TC_wafer = TC_id.wafer();
  int TC_3rd = ((TC_id.cell())>>4)&0x3;

  long SuperTriggerCellMap_id = 0;
  if(TC_id.subdetId()==HGCHEB) SuperTriggerCellMap_id = TC_id.cell();
  else SuperTriggerCellMap_id = TC_3rd + (TC_wafer<<2);

  return &mapSuperTriggerCellMap_[SuperTriggerCellMap_id];

}

void 
HGCalConcentratorSelectionImpl::
superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput)
{ 

  std::cout << "Going into the SuperTriggerCell" << std::endl;

  for (size_t i = 0; i<trigCellVecInput.size();i++){

  
    getSuperTriggerCell_( trigCellVecInput.at(i) )->addTriggerCell( trigCellVecInput.at(i) );

  }

  for (size_t i = 0; i<trigCellVecInput.size();i++){

    trigCellVecOutput.push_back( trigCellVecInput.at(i) );

    int threshold = (HGCalDetId( trigCellVecInput.at(i).detId()).subdetId()==ForwardSubdetector::HGCHEB ? TCThresholdBH_ADC_ : TCThreshold_ADC_);
    if ( trigCellVecInput.at(i).hwPt() < threshold)    trigCellVecOutput.at(i).setHwPt(0);

    l1t::HGCalSuperTriggerCellMap* TCmap = getSuperTriggerCell_( trigCellVecInput.at(i) );    
    //Check if TC is the most energetic in towerMap and assign the full hwPt of the towerMap
    //Else zeroed
    if(TCmap->maxTriggerCell().detId() == trigCellVecInput.at(i).detId()) trigCellVecOutput.at(i).setHwPt(TCmap->hwPt());
    else trigCellVecOutput.at(i).setHwPt(0);
  
  }  


}
