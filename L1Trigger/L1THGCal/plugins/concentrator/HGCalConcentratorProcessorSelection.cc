#include "L1Trigger/L1THGCal/interface/concentrator/HGCalConcentratorProcessorSelection.h"
#include <limits>

#include "DataFormats/HGCDigi/interface/HGCDigiCollections.h"

DEFINE_EDM_PLUGIN(HGCalConcentratorFactory, 
        HGCalConcentratorProcessorSelection,
        "HGCalConcentratorProcessorSelection");

HGCalConcentratorProcessorSelection::HGCalConcentratorProcessorSelection(const edm::ParameterSet& conf)  : 
  HGCalConcentratorProcessorBase(conf),
  fixedDataSizePerHGCROC_(conf.getParameter<bool>("fixedDataSizePerHGCROC")),
  coarsenTriggerCells_(conf.getParameter<bool>("coarsenTriggerCells"))
{ 
  std::string selectionType(conf.getParameter<std::string>("Method"));
  if (selectionType == "thresholdSelect"){
    selectionType_ = thresholdSelect;
    concentratorProcImpl_ = std::make_unique<HGCalConcentratorSelectionImpl>(conf);
  }
  else if (selectionType == "bestChoiceSelect"){
    selectionType_ = bestChoiceSelect;
    concentratorProcImpl_ = std::make_unique<HGCalConcentratorSelectionImpl>(conf);
  }
  else if (selectionType == "superTriggerCellSelect"){
    selectionType_ = superTriggerCellSelect;
    concentratorSTCImpl_ = std::make_unique<HGCalConcentratorSuperTriggerCellImpl>(conf);
  }
  else{
    throw cms::Exception("HGCTriggerParameterError")
      << "Unknown type of concentrator selection '" << selectionType << "'";
  }

  if ( coarsenTriggerCells_ || fixedDataSizePerHGCROC_ ){   
    concentratorCoarsenerImpl_ = std::make_unique<HGCalConcentratorCoarsenerImpl>(conf);
  }

}

void HGCalConcentratorProcessorSelection::run(const edm::Handle<l1t::HGCalTriggerCellBxCollection>& triggerCellCollInput, 
                                              l1t::HGCalTriggerCellBxCollection& triggerCellCollOutput,
                                              const edm::EventSetup& es)
{ 
  if(concentratorProcImpl_) concentratorProcImpl_->eventSetup(es);
  const l1t::HGCalTriggerCellBxCollection& collInput = *triggerCellCollInput;

  std::unordered_map<uint32_t, std::vector<l1t::HGCalTriggerCell>> tc_modules;
  for(const auto& trigCell : collInput) {
    uint32_t module = geometry_->getModuleFromTriggerCell(trigCell.detId());
    tc_modules[module].push_back(trigCell);
  }
  if ( concentratorSTCImpl_) concentratorSTCImpl_->eventSetup(es);
  if ( concentratorCoarsenerImpl_) concentratorCoarsenerImpl_->eventSetup(es);

  for( const auto& module_trigcell : tc_modules ) {

    std::vector<l1t::HGCalTriggerCell> trigCellVecOutput;
    std::vector<l1t::HGCalTriggerCell> trigCellVecCoarsened;	  
    if ( coarsenTriggerCells_ || fixedDataSizePerHGCROC_ ){
      concentratorCoarsenerImpl_->coarseTriggerCellSelectImpl(module_trigcell.second,trigCellVecCoarsened);

      switch(selectionType_){
      case thresholdSelect:
        concentratorProcImpl_->thresholdSelectImpl(trigCellVecCoarsened, trigCellVecOutput);
        break;
      case bestChoiceSelect:
        concentratorProcImpl_->bestChoiceSelectImpl(trigCellVecCoarsened, trigCellVecOutput);     
        break;
      case superTriggerCellSelect:
	concentratorSTCImpl_->superTriggerCellSelectImpl(trigCellVecCoarsened, trigCellVecOutput);
        break;
      default:
        // Should not happen, selection type checked in constructor
        break;
      }

    }
    else{
      
      switch(selectionType_){
      case thresholdSelect:
        concentratorProcImpl_->thresholdSelectImpl(module_trigcell.second, trigCellVecOutput);
        break;
      case bestChoiceSelect:
        concentratorProcImpl_->bestChoiceSelectImpl(module_trigcell.second, trigCellVecOutput);     
        break;
      case superTriggerCellSelect:
	concentratorSTCImpl_->superTriggerCellSelectImpl(module_trigcell.second, trigCellVecOutput);
        break;
      default:
        // Should not happen, selection type checked in constructor
        break;
      }
      
      
      
    }
    
    
    for( const auto& trigCell : trigCellVecOutput){
      triggerCellCollOutput.push_back(0, trigCell);     
    }
  }
}
