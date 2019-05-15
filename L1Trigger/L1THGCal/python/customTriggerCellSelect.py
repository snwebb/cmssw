import FWCore.ParameterSet.Config as cms
import SimCalorimetry.HGCalSimProducers.hgcalDigitizer_cfi as digiparam
from L1Trigger.L1THGCal.hgcalConcentratorProducer_cfi import threshold_conc_proc, best_conc_proc, supertc_conc_proc, typeenergydivision_onebitfraction_proc, typeenergydivision_equalshare_proc

def custom_triggercellselect_supertriggercell(process,
                                              stcSize=supertc_conc_proc.stcSize,
                                              type_energy_division=supertc_conc_proc.type_energy_division,
                                              fixedDataSizePerHGCROC=supertc_conc_proc.fixedDataSizePerHGCROC
                                              ):
    parameters = supertc_conc_proc.clone()
    parameters.stcSize = stcSize
    parameters.type_energy_division = type_energy_division
    parameters.fixedDataSizePerHGCROC = fixedDataSizePerHGCROC
    process.hgcalConcentratorProducer.ProcessorParameters = parameters
    return process


def custom_triggercellselect_threshold(process,
                                       threshold_silicon=threshold_conc_proc.threshold_silicon,  # in mipT
                                       threshold_scintillator=threshold_conc_proc.threshold_scintillator  # in mipT
                                       ):
    parameters = threshold_conc_proc.clone()
    parameters.threshold_silicon = threshold_silicon
    parameters.threshold_scintillator = threshold_scintillator
    process.hgcalConcentratorProducer.ProcessorParameters = parameters
    return process


def custom_triggercellselect_bestchoice(process,
                                        triggercells=best_conc_proc.NData
                                        ):
    parameters = best_conc_proc.clone()
    parameters.NData = triggercells
    process.hgcalConcentratorProducer.ProcessorParameters = parameters
    return process


def custom_typeenergydivision_onebitfraction(process,
                                             oneBitFractionThreshold = typeenergydivision_onebitfraction_proc.oneBitFractionThreshold,
                                             oneBitFractionLowValue = typeenergydivision_onebitfraction_proc.oneBitFractionLowValue,
                                             oneBitFractionHighValue = typeenergydivision_onebitfraction_proc.oneBitFractionHighValue,
                                             ):
    parameters = typeenergydivision_onebitfraction_proc.clone()
    parameters.oneBitFractionThreshold = oneBitFractionThreshold;
    parameters.oneBitFractionLowValue = oneBitFractionLowValue;
    parameters.oneBitFractionHighValue = oneBitFractionHighValue;

    return process




def custom_typeenergydivision_equalshare(process,
                                         nTriggerCellsForDivision = 4
                                         ):
    parameters = typeenergydivision_equalshare_proc.clone()
    parameters.nTriggerCellsForDivision = nTriggerCellsForDivision;

    return process
    
