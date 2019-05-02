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
    void getConstituentTriggerCells( int ctcId, int ctcSize, std::vector<int> & output_ids) const;
    void setCoarseTriggerCellPosition( l1t::HGCalTriggerCell& tc );
    int getCoarseTriggerCellId(int detid, int CTCsize = -1) const ;
    void checkSizeValidity(int CTCsize)const;

  private:


    static const std::map<int,int> kSplit_;
    static const std::map<int,int> kSplit_v9_;
    static constexpr int kWafer_offset_ = 6;
    static constexpr int kSTCidMask_ = 63;
    static constexpr int kCTCsizeCoarse_ = 16;
    static constexpr int kCTCsizeMid_ = 8;
    static constexpr int kCTCsizeFine_ = 4;
    static constexpr int kCTCsizeVeryFine_ = 2;
    static constexpr int kSplit_v8_Coarse_ = 0x30;
    static constexpr int kSplit_v8_Mid_ = 0x38;
    static constexpr int kSplit_v8_Fine_ = 0x3a;
    static constexpr int kSplit_v8_VeryFine_ = 0x3e;
    static constexpr int kNLayers_ = 3;
    static constexpr int kSplit_v9_VeryFine_ = 0x37;
    static constexpr int kSplit_v9_Fine_ = 0x36;
    static constexpr int kSplit_v9_Mid_ = 0x26;

    static constexpr int kRocShift_ = 6;
    static constexpr int kRotate4_ = 4;
    static constexpr int kUShift_ = 3;

    HGCalTriggerTools triggerTools_;
    HGCSiliconDetIdToROC detIdToROC_;
    
};

#endif
