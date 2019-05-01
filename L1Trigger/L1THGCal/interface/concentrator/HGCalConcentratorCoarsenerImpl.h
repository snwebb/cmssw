#ifndef __L1Trigger_L1THGCal_HGCalConcentratorCoarsenerImpl_h__
#define __L1Trigger_L1THGCal_HGCalConcentratorCoarsenerImpl_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"


      
#include <array>
#include <vector>

class HGCalConcentratorCoarsenerImpl
{
  public:
    HGCalConcentratorCoarsenerImpl(const edm::ParameterSet& conf);

    void coarseTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput);
    void eventSetup(const edm::EventSetup& es) {triggerTools_.eventSetup(es);}


  private:

    int getSuperTriggerCellId(int detid) const ;
    static const int kSplit_v8_VeryFine_ = 0x3e;

    HGCalTriggerTools triggerTools_;

    class SuperTriggerCell {
  
    private:
        float sumPt_, sumMipPt_, fracsum_;
        int sumHwPt_, maxHwPt_, stcId_; 
        unsigned maxId_;

    public:
        SuperTriggerCell(){  
          sumPt_=0, sumMipPt_=0, sumHwPt_=0, maxHwPt_=0, maxId_=0, fracsum_ = 0, stcId_ = 0; }
        void add(const l1t::HGCalTriggerCell &c, int stcId) {
            sumPt_ += c.pt();
            sumMipPt_ += c.mipPt();
            sumHwPt_ += c.hwPt();
            if (maxId_ == 0 || c.hwPt() > maxHwPt_) {
                maxHwPt_ = c.hwPt();
                maxId_ = c.detId();
            }
            
            if  ( stcId_ == 0 ){
              stcId_ = stcId;
            }

        }

        void assignEnergy(l1t::HGCalTriggerCell &c) const {

            c.setHwPt(sumHwPt_);
            c.setMipPt(sumMipPt_);
            c.setPt( sumPt_ );

        }
        void setEvenDetId(l1t::HGCalTriggerCell &c) const {	  	  
	  c.setDetId( c.detId() & ~1 );
        }
        unsigned GetMaxId()const{return maxId_;}
        unsigned GetSTCId()const{return stcId_;}
        int GetMaxHwPt()const{return maxHwPt_;}

    };
    
};

#endif
