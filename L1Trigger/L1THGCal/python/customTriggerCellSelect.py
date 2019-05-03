import FWCore.ParameterSet.Config as cms
import SimCalorimetry.HGCalSimProducers.hgcalDigitizer_cfi as digiparam
from L1Trigger.L1THGCal.hgcalConcentratorProducer_cfi import threshold_conc_proc, best_conc_proc, supertc_conc_proc


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
                                       threshold_silicon=threshold_conc_proc.triggercell_threshold_silicon,  # in mipT
                                       threshold_scintillator=threshold_conc_proc.triggercell_threshold_scintillator  # in mipT
                                       ):
    parameters = threshold_conc_proc.clone()
    parameters.triggercell_threshold_silicon = threshold_silicon
    parameters.triggercell_threshold_scintillator = threshold_scintillator
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
                                             oneBitFractionThreshold = 0.125,
                                             oneBitFractionLowValue = 0.0625,
                                             oneBitFractionHighValue = 0.25,
                                             ):
    parameters = process.hgcalConcentratorProducer.ProcessorParameters = parameters
    parameters.oneBitFractionThreshold = cms.double(oneBitFractionThreshold);
    parameters.oneBitFractionLowValue = cms.double(oneBitFractionLowValue);
    parameters.oneBitFractionHighValue = cms.double(oneBitFractionHighValue);

    return process




def custom_typeenergydivision_equalshare(process,
                                         nTriggerCellsForDivision = 4
                                         ):
    parameters = process.hgcalConcentratorProducer.ProcessorParameters = parameters
    parameters.nTriggerCellsForDivision = cms.int32(nTriggerCellsForDivision);

    return process
