#ifndef __L1Trigger_L1THGCal_HGCalConcentratorSuperTriggerCellImpl_h__
#define __L1Trigger_L1THGCal_HGCalConcentratorSuperTriggerCellImpl_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"
#include "L1Trigger/L1THGCal/interface/HGCalCoarseTriggerCellMapping.h"

      
#include <array>
#include <vector>

class HGCalConcentratorSuperTriggerCellImpl
{
  public:
    HGCalConcentratorSuperTriggerCellImpl(const edm::ParameterSet& conf);

    void superTriggerCellSelectImpl(const std::vector<l1t::HGCalTriggerCell>& trigCellVecInput, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput);
    void eventSetup(const edm::EventSetup& es) {triggerTools_.eventSetup(es);}


  private:

    enum EnergyDivisionType{
      superTriggerCell,
      oneBitFraction,
      equalShare,
    };
    EnergyDivisionType energyDivisionType_;
    static constexpr int kNLayers_ = 3;
    HGCalTriggerTools triggerTools_;
    HGCalCoarseTriggerCellMapping coarseTCmapping_;
    std::vector<unsigned> stcSize_;
    bool fixedDataSizePerHGCROC_;

    //Parameters for energyDivisionType_ = oneBitFraction
    double oneBitFractionThreshold_;
    double oneBitFractionLowValue_;
    double oneBitFractionHighValue_;

    //Parameters for energyDivisionType_ = equalShare
    int nTriggerCellsForDivision_;

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
        void SetMaxTC(const l1t::HGCalTriggerCell &c){
          maxId_ = c.detId();
          maxHwPt_ = c.hwPt();
        }
	void addToFractionSum(float frac){
	  fracsum_ += frac;
	  if ( fracsum_ > 1 ) fracsum_ = 1;
	}
        unsigned GetMaxId()const{return maxId_;}
        unsigned GetSTCId()const{return stcId_;}
        int GetMaxHwPt()const{return maxHwPt_;}
        int GetSumMipPt()const{return sumMipPt_;}
        int GetSumHwPt()const{return sumHwPt_;}
        int GetSumPt()const{return sumPt_;}
        int GetFractionSum()const{
	  return fracsum_;
	}

    };
    void createMissingTriggerCells( std::unordered_map<unsigned,SuperTriggerCell>& STCs, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput) const;
    void assignSuperTriggerCellEnergy(l1t::HGCalTriggerCell &c, const SuperTriggerCell &stc) const;
    float getTriggerCellOneBitFraction( float tcPt, float sumPt );
};

#endif
