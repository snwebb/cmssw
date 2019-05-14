#ifndef __L1Trigger_L1THGCal_HGCalCoarseTriggerCellMapping_h__
#define __L1Trigger_L1THGCal_HGCalCoarseTriggerCellMapping_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"
      
class HGCalCoarseTriggerCellMapping
{
  public:
    HGCalCoarseTriggerCellMapping();
    void setEvenDetId(l1t::HGCalTriggerCell &c) const;
    uint32_t getEvenDetId(int tcid) const;
    std::vector<uint32_t> getConstituentTriggerCells( int ctcId, int ctcSize) const;
    void setCoarseTriggerCellPosition( l1t::HGCalTriggerCell& tc, const int coarse_size ) const;
    int getCoarseTriggerCellId(int detid, int CTCsize = -1) const ;
    void checkSizeValidity(int CTCsize)const;

    static constexpr int kCTCsizeCoarse_ = 16;
    static constexpr int kCTCsizeMid_ = 8;
    static constexpr int kCTCsizeFine_ = 4;
    static constexpr int kCTCsizeVeryFine_ = 2;    

  private:


    static const std::map<int,int> kSplit_;
    static const std::map<int,int> kSplit_v9_;
    static constexpr int kWafer_offset_ = 6;
    static constexpr int kSTCidMask_ = 63;
    static constexpr int kSplit_v8_Coarse_ = 0x30;
    static constexpr int kSplit_v8_Mid_ = 0x38;
    static constexpr int kSplit_v8_Fine_ = 0x3a;
    static constexpr int kSplit_v8_VeryFine_ = 0x3e;
    static constexpr int kNLayers_ = 3;
    static constexpr int kSplit_v9_VeryFine_ = 0x37;
    static constexpr int kSplit_v9_Fine_ = 0x36;
    static constexpr int kSplit_v9_Mid_ = 0x26;
    static constexpr int kSplit_v9_Coarse_ = 0x24;

    //For coarse TCs
    static constexpr int kRocShift_ = 6;
    static constexpr int kRocMask_ = 0xC0;
    static constexpr int kRotate4_ = 4;
    static constexpr int kUShift_ = 3;
    static constexpr int kVShift_ = 0;
    static constexpr int kUMask_ = 0x38;
    static constexpr int kVMask_ = 0x7;

    //For original cells 
    static constexpr int kHGCalCellUOffset_      = 0;
    static constexpr int kHGCalCellUMask_        = 0xF;
    static constexpr int kHGCalCellVOffset_      = 4;
    static constexpr int kHGCalCellVMask_        = 0xF;

    HGCalTriggerTools triggerTools_;
    HGCSiliconDetIdToROC detIdToROC_;
    
};

#endif
