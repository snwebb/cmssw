#ifndef __L1Trigger_L1THGCal_HGCalConcentratorProcessorSelection_h__
#define __L1Trigger_L1THGCal_HGCalConcentratorProcessorSelection_h__

#include "L1Trigger/L1THGCal/interface/HGCalProcessorBase.h"
#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSelectionImpl.h"
#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorSuperTriggerCellImpl.h"
#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorCoarsenerImpl.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"

class HGCalConcentratorProcessorSelection : public HGCalConcentratorProcessorBase 
{ 

  private:
    enum SelectionType{
      thresholdSelect,
      bestChoiceSelect,
      superTriggerCellSelect
    };

  public:
    HGCalConcentratorProcessorSelection(const edm::ParameterSet& conf);
  
    void run(const edm::Handle<l1t::HGCalTriggerCellBxCollection>& triggerCellCollInput, l1t::HGCalTriggerCellBxCollection& triggerCellCollOutput, const edm::EventSetup& es) override;

  private:
    SelectionType selectionType_;
    bool fixedDataSizePerHGCROC_;
    
    std::unique_ptr<HGCalConcentratorSelectionImpl> concentratorProcImpl_;
    std::unique_ptr<HGCalConcentratorSuperTriggerCellImpl> concentratorSTCImpl_;
    std::unique_ptr<HGCalConcentratorCoarsenerImpl> concentratorCoarsenerImpl_;
     
    HGCalTriggerTools triggerTools_;

};

#endif
