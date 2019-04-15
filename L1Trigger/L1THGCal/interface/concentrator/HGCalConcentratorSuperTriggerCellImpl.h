#ifndef __L1Trigger_L1THGCal_HGCalConcentratorSuperTriggerCellImpl_h__
#define __L1Trigger_L1THGCal_HGCalConcentratorSuperTriggerCellImpl_h__

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"

#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerSums.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToROC.h"

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
    static std::map<int,int> kSplit_;
    static const int kWafer_offset_ = 6;
    static const int kSTCsizeCoarse_ = 16;
    static const int kSTCsizeMid_ = 8;
    static const int kSTCsizeFine_ = 4;
    static const int kSTCsizeVeryFine_ = 2;
    static const int kNLayers_ = 3;
    static const int kSplit_v9_ = 0x36;

    HGCalTriggerTools triggerTools_;
    HGCSiliconDetIdToROC detIdToROC_;
    std::vector<unsigned> stcSize_;

    class SuperTriggerCell {
  
    private:
        float sumPt_, sumMipPt_;
        int sumHwPt_, maxHwPt_; 
        unsigned maxId_;
	std::vector<int> TClist_;
	bool reject_;

    public:
        SuperTriggerCell(){  sumPt_=0, sumMipPt_=0, sumHwPt_=0, maxHwPt_=0, maxId_=0, reject_=false ;}
        void add(const l1t::HGCalTriggerCell &c) {
            sumPt_ += c.pt();
            sumMipPt_ += c.mipPt();
            sumHwPt_ += c.hwPt();
            if (maxId_ == 0 || c.hwPt() > maxHwPt_) {
                maxHwPt_ = c.hwPt();
                maxId_ = c.detId();
            }

	    TClist_.push_back( c.detId() );
        }
        void assignEnergy(l1t::HGCalTriggerCell &c, std::string type) const {

	  if ( type == "STC" ){
            c.setHwPt(sumHwPt_);
            c.setMipPt(sumMipPt_);
            c.setPt( sumPt_ );
	  }
	  if ( type == "EqualShare" ){
            c.setHwPt( sumHwPt_/4 );
            c.setMipPt( sumMipPt_/4 );
            c.setPt( sumPt_/4 );
	  }




        }
        unsigned GetMaxId()const{return maxId_;}
        unsigned GetNTCs()const{return TClist_.size();}
	std::vector<int> GetTCList()const{return TClist_;}
        bool rejected()const{return reject_;}
        void reject(){reject_ = true;}

    };
    void createMissingTriggerCells( std::unordered_map<unsigned,SuperTriggerCell>& STCs, std::vector<l1t::HGCalTriggerCell>& trigCellVecOutput);
    
};

#endif
