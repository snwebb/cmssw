#ifndef __L1Trigger_L1THGCal_HGCalCoarseTriggerCellMapping_h__
#define __L1Trigger_L1THGCal_HGCalCoarseTriggerCellMapping_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"
      
class HGCalCoarseTriggerCellMapping
{
  public:
  HGCalCoarseTriggerCellMapping(std::vector<unsigned> stcSize);
    void setEvenDetId(l1t::HGCalTriggerCell &c) const;
    uint32_t getEvenDetId(uint32_t tcid) const;
    std::vector<uint32_t> getConstituentTriggerCells( uint32_t ctcId ) const;
    void setCoarseTriggerCellPosition( l1t::HGCalTriggerCell& tc ) const;
    uint32_t getCoarseTriggerCellId(uint32_t detid) const ;
    void checkSizeValidity(int CTCsize)const;
    void eventSetup(const edm::EventSetup& es) {triggerTools_.eventSetup(es);}

    static constexpr int kCTCsizeCoarse_ = 16;
    static constexpr int kCTCsizeMid_ = 8;
    static constexpr int kCTCsizeFine_ = 4;
    static constexpr int kCTCsizeVeryFine_ = 2;    

  private:


    static const std::map<int,int> kSplit_;
    static const std::map<int,int> kSplit_v9_;
    static const std::map<int,int> kSplit_v9_Scin_;
    //    static constexpr int kWafer_offset_ = 6;
    static constexpr int kSTCidMask_ = 0x3f;
    static constexpr int kSTCidMask_v9_ = 0xf;
    static constexpr int kSplit_v8_Coarse_ = 0x30;
    static constexpr int kSplit_v8_Mid_ = 0x38;
    static constexpr int kSplit_v8_Fine_ = 0x3a;
    static constexpr int kSplit_v8_VeryFine_ = 0x3e;
    //    static constexpr int kNLayers_ = 3;
    static constexpr int kNLayers_ = 4;
    static constexpr int kSplit_v9_Coarse_ = 0;
    static constexpr int kSplit_v9_Mid_ = 0x2;
    static constexpr int kSplit_v9_Fine_ = 0xa;
    static constexpr int kSplit_v9_VeryFine_ = 0xb;

    static constexpr int kSplit_v9_Scin_Coarse_ = 0x1f9fc;
    static constexpr int kSplit_v9_Scin_Mid_ = 0x1fdfc;
    static constexpr int kSplit_v9_Scin_Fine_ = 0x1fdfe;
    static constexpr int kSplit_v9_Scin_VeryFine_ = 0x1fffe;

    //For coarse TCs
    static constexpr int kRocShift_ = 4;
    static constexpr int kRocMask_ = 0xC0;
    static constexpr int kRotate4_ = 4;
    static constexpr int kRotate7_ = 7;
    static constexpr int kUShift_ = 2;
    static constexpr int kVShift_ = 0;
    static constexpr int kUMask_ = 0xc;
    static constexpr int kVMask_ = 0x3;
    static constexpr int kHGCalCellUOffset_      = 0;
    static constexpr int kHGCalCellUMask_        = 0xF;
    static constexpr int kHGCalCellVOffset_      = 4;
    static constexpr int kHGCalCellVMask_        = 0xF;
    static constexpr int kHGCalCellMaskV9_         = 0xff;
    static constexpr int kHGCalScinCellMask_     = 0x1ffff;

    HGCalTriggerTools triggerTools_;
    HGCSiliconDetIdToROC detIdToROC_;
    std::vector<unsigned> stcSize_;

};

#endif
