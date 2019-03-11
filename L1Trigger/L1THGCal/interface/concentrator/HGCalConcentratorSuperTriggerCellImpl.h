#ifndef __L1Trigger_L1THGCal_HGCalConcentratorSuperTriggerCellImpl_h__
#define __L1Trigger_L1THGCal_HGCalConcentratorSuperTriggerCellImpl_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"

#include <array>
#include <vector>

class HGCalConcentratorSuperTriggerCellImpl
{
  public:
    HGCalConcentratorSuperTriggerCellImpl(const edm::ParameterSet& conf);

    void superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput);
    void eventSetup(const edm::EventSetup& es) {triggerTools_.eventSetup(es);}

  private:

    int getSuperTriggerCellId(int detid) const ;
    static const int kSplit12_ = 0x3a;
    static const int kSplit3_ = 0x30;
    static const int kWafer_offset_ = 6;

    static const int kSTCsize16_ = 16;
    static const int kSTCsize4_ = 4;
    static const int kNLayers_ = 3;

    static const int kSplit12_v9_1 = 0x36;
    static const int kSplit12_v9_2 = 0x26;
    static const int kSplit12_v9_3 = 0x30;

    static const int kOne_ = 1;
    static const int kTwo_ = 2;
    static const int kThree_ = 3;


    HGCalTriggerTools triggerTools_;
    std::vector<unsigned> stcSize_;

    class SuperTriggerCell {
  
    private:
        float sumPt_, sumMipPt_;
        int sumHwPt_, maxHwPt_; 
        unsigned maxId_;

    public:
        SuperTriggerCell(){  sumPt_=0, sumMipPt_=0, sumHwPt_=0, maxHwPt_=0, maxId_=0 ;}
        void add(const l1t::HGCalTriggerCell &c) {
            sumPt_ += c.pt();
            sumMipPt_ += c.mipPt();
            sumHwPt_ += c.hwPt();
            if (maxId_ == 0 || c.hwPt() > maxHwPt_) {
                maxHwPt_ = c.hwPt();
                maxId_ = c.detId();
            }
        }
        void assignEnergy(l1t::HGCalTriggerCell &c) const {
            c.setHwPt(sumHwPt_);
            c.setMipPt(sumMipPt_);
            c.setPt( sumPt_ );
        }
        unsigned GetMaxId()const{return maxId_;}
    };
    
};

#endif
