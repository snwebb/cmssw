import FWCore.ParameterSet.Config as cms 
from Configuration.StandardSequences.Eras import eras
from Configuration.ProcessModifiers.convertHGCalDigisSim_cff import convertHGCalDigisSim

process = cms.Process('DIGI',eras.Phase2,convertHGCalDigisSim)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended2023D17Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D17_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedHLLHC14TeV_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

# Input source
process.source = cms.Source("PoolSource",
#       fileNames = cms.untracked.vstring('/store/relval/CMSSW_9_3_0/RelValSinglePiPt25Eta1p7_2p7/GEN-SIM-DIGI-RAW/93X_upgrade2023_realistic_v2_2023D17noPU-v1/00000/240935CF-1C9B-E711-9F7D-0025905A60BE.root'),

       fileNames = cms.untracked.vstring('/store/mc/PhaseIITDRFall17DR/VBF_HToInvisible_M125_14TeV_powheg_pythia8/GEN-SIM-RECO/PU200_93X_upgrade2023_realistic_v2-v1/30000/000529D7-EEB8-E711-95AF-7845C4FC37B5.root'),
       inputCommands=cms.untracked.vstring(
           'keep *',
           'drop l1tEMTFHit2016Extras_simEmtfDigis_CSC_HLT',
           'drop l1tEMTFHit2016Extras_simEmtfDigis_RPC_HLT',
           'drop l1tEMTFHit2016s_simEmtfDigis__HLT',
           'drop l1tEMTFTrack2016Extras_simEmtfDigis__HLT',
           'drop l1tEMTFTrack2016s_simEmtfDigis__HLT',
           )
       )

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('SingleElectronPt10_cfi nevts:10'),
    name = cms.untracked.string('Applications')
)

# Output definition
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("ntuple.root")
    )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')

# load HGCAL TPG simulation
process.load('L1Trigger.L1THGCal.hgcalTriggerPrimitives_cff')
process.hgcl1tpg_step = cms.Path(process.hgcalTriggerPrimitives)

from L1Trigger.L1THGCal.customTriggerCellSelect import custom_triggercellselect_supertriggercell, custom_triggercellselect_threshold
from L1Trigger.L1THGCal.customClustering import *

#TCs
process = custom_triggercellselect_threshold(process)
#process = custom_triggercellselect_supertriggercell(process)
#1st Stage
process = custom_2dclustering_dummy(process)
#2nd Stage

#process = custom_3dclustering_distance(process)
#process = custom_3dclustering_histoMax(process,0.03)
#process = custom_3dclustering_histoInterpolatedMax2ndOrder(process)
#process = custom_3dclustering_histoInterpolatedMax1stOrder(process)
process = custom_3dclustering_histoModifiedMax(process)


# Change to V7 trigger geometry for older samples
#  from L1Trigger.L1THGCal.customTriggerGeometry import custom_geometry_ZoltanSplit_V7
#  process = custom_geometry_ZoltanSplit_V7(process)

# load ntuplizer
process.load('L1Trigger.L1THGCal.hgcalTriggerNtuples_cff')
process.ntuple_step = cms.Path(process.hgcalTriggerNtuples)

# Schedule definition
process.schedule = cms.Schedule(process.hgcl1tpg_step, process.ntuple_step)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

