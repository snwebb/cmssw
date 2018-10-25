#include "DataFormats/L1THGCal/interface/HGCalSuperTriggerCellMap.h"

using namespace l1t;


void HGCalSuperTriggerCellMap::addTriggerCell( l1t::HGCalTriggerCell TC ){
  
  //  if ( TC.hwPt() < 0 ) std::cout << TC.hwPt() << std::endl;
  this->setHwPt(this->hwPt()+TC.hwPt());
  if(TC.hwPt()>maxTriggerCell_.hwPt()) maxTriggerCell_ = TC;
  triggerCells_.emplace_back(TC);

  // if ( TC.mipPt() > 2 ){
  //   triggerCellCounter ++;
  // }
}

// int HGCalSuperTriggerCellMap::getNumberOfConstituentCells(){
  
//   return triggerCellCounter;

// }

double HGCalSuperTriggerCellMap::findAverageDistanceBetweenCells(){

  double sum  = 0;
  int counter = 0;
  //  double average = 0;

  if ( triggerCells_.size() <= 1 ) return 0;


  for ( size_t i = 0; i < triggerCells_.size(); i++){
    for ( size_t j = 0; j < triggerCells_.size()-1; j++){
      
      double xdiff = std::abs(triggerCells_.at(i).position().x() - triggerCells_.at(j).position().x());
      double ydiff = std::abs(triggerCells_.at(i).position().y() - triggerCells_.at(j).position().y());
      sum+= std::sqrt( xdiff*xdiff + ydiff*ydiff);
      counter++;

      
      
    }
  }


  //  std::cout << sum << " - " << counter <<  " - " << sum/double(counter) << std::endl;

  //  average = sum/double(counter);

  
  if ( triggerCells_.size() == 2 ) {
    

    
    //     if ( sum > 40 ) {
     // if ( sum > 4 ) {
     //  HGCalDetId TC_id0( triggerCells_.at(0).detId() );
     //  HGCalDetId TC_id1( triggerCells_.at(1).detId() );
    //   HGCalDetId TC_max(  maxTriggerCell_.detId() );
    //   std::cout << sum << " - " << triggerCells_.at(0).subdetId() << " - " << triggerCells_.at(0).zside() << " - " << triggerCells_.at(0).layer() << " - " << triggerCells_.at(0).detId() << " - " << TC_id0.cell() << " - " << TC_id0.wafer() << std::endl;
    //   std::cout << sum << " - " << triggerCells_.at(1).subdetId() << " - " << triggerCells_.at(1).zside() << " - " << triggerCells_.at(1).layer() << " - " << triggerCells_.at(1).detId()  << " - " << TC_id0.cell() << " - " << TC_id0.wafer() << std::endl;
    //   std::cout << sum << " - " << maxTriggerCell_.subdetId() << " - " << maxTriggerCell_.zside() << " - " << maxTriggerCell_.layer() << " - " << maxTriggerCell_.detId()  << " - " << TC_max.cell() << " - " << TC_max.wafer() << std::endl;

      // std::cout << sum << " - " << triggerCells_.at(0).position().x() << " - " << triggerCells_.at(0).position().y() <<  " - " <<  TC_id0.cell() << std::endl;
      // std::cout << sum << " - " << triggerCells_.at(1).position().x() << " - " << triggerCells_.at(1).position().y() <<  " - " << TC_id1.cell() << std::endl;

    //     }
    
    return sum;


  }

  else return 0.;


}

