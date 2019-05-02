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
      coarse2TriggerCell
    };
    EnergyDivisionType energyDivisionType_;
    static const int kNLayers_ = 3;
    HGCalTriggerTools triggerTools_;
    HGCalCoarseTriggerCellMapping coarseTCmapping_;
    std::vector<unsigned> stcSize_;
    bool fixedDataSize_;

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
        void getFractionSum(const l1t::HGCalTriggerCell &c) {

          if ( c.detId() != maxId_ ){
            double f = c.pt() / sumPt_ ;
            double frac = 0;
            if ( f < 1./8. ){
              frac = 1./16.;
            }
            else{
              frac = 1./4.;
            }
            fracsum_ += frac;
          }

        }

        void assignEnergy(l1t::HGCalTriggerCell &c, EnergyDivisionType energyDivisionType_) const {

          if ( energyDivisionType_ == superTriggerCell || energyDivisionType_ == coarse2TriggerCell ){
            c.setHwPt(sumHwPt_);
            c.setMipPt(sumMipPt_);
            c.setPt( sumPt_ );
          }
          else if ( energyDivisionType_ == equalShare ){
            c.setHwPt( sumHwPt_/4 );
            c.setMipPt( sumMipPt_/4 );
            c.setPt( sumPt_/4 );
          }
          else if (  energyDivisionType_ == oneBitFraction ){

            double f = c.pt() / sumPt_ ;
            double frac = 0;
            
            if ( c.detId() != maxId_ ){
              if ( f < 1./8. ){
                frac = 1./16.;
              }
              else{
                frac = 1./4.;
              }
            }
            else{
              frac = 1-fracsum_;
            }
            
            c.setHwPt(sumHwPt_ * frac );
            c.setMipPt(sumMipPt_ * frac );
            c.setPt( sumPt_ * frac );
          }

        }
        void SetMaxTC(const l1t::HGCalTriggerCell &c){
          maxId_ = c.detId();
          maxHwPt_ = c.hwPt();
        }
        unsigned GetMaxId()const{return maxId_;}
        unsigned GetSTCId()const{return stcId_;}
        int GetMaxHwPt()const{return maxHwPt_;}

    };
    void createMissingTriggerCells( std::unordered_map<unsigned,SuperTriggerCell>& STCs, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput) const;
    
};

#endif
