#ifndef DataFormats_L1TCalorimeter_HGCalSuperTriggerCellMap_h
#define DataFormats_L1TCalorimeter_HGCalSuperTriggerCellMap_h


#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/L1THGCal/interface/HGCalTriggerCell.h"
#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"

namespace l1t 
{

  class HGCalSuperTriggerCellMap;
  typedef BXVector<HGCalSuperTriggerCellMap> HGCalSuperTriggerCellMapBxCollection;

  class HGCalSuperTriggerCellMap : public HGCalTriggerCell
  {

  public:

    //HGCalSuperTriggerCellMap() { triggerCellCounter=0;}
     HGCalSuperTriggerCellMap() {}

    ~HGCalSuperTriggerCellMap() {}

    void addTriggerCell( HGCalTriggerCell TC );
    //   int getNumberOfConstituentCells( );
    std::vector<HGCalTriggerCell> triggerCells() { return triggerCells_; }
    HGCalTriggerCell maxTriggerCell() { return maxTriggerCell_; }
    double findAverageDistanceBetweenCells();
    
          
  private:
  
    std::vector<HGCalTriggerCell> triggerCells_;
    HGCalTriggerCell maxTriggerCell_;
    //        int triggerCellCounter;

  };

}

#endif
