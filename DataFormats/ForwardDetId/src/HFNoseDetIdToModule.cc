#include "DataFormats/ForwardDetId/interface/HFNoseDetIdToModule.h"

HFNoseDetIdToModule::HFNoseDetIdToModule() {}

std::vector<HFNoseDetId> HFNoseDetIdToModule::getDetIds(HFNoseDetId const& id) const {
  std::vector<HFNoseDetId> ids;
  int nCells = (id.type() == 0) ? HFNoseDetId::HFNoseFineN : HFNoseDetId::HFNoseCoarseN;
  for (int u = 0; u < 2 * nCells; ++u) {
    for (int v = 0; v < 2 * nCells; ++v) {
      if (((v - u) < nCells) && (u - v) <= nCells) {
        HFNoseDetId newId(id.zside(), id.type(), id.layer(), id.waferU(), id.waferV(), u, v);
        ids.emplace_back(newId);
      }
    }
  }
  return ids;
}

std::vector<HFNoseTriggerDetId> HFNoseDetIdToModule::getTriggerDetIds(HFNoseDetId const& id) const {
  std::vector<HFNoseTriggerDetId> ids;
  int nCells = HFNoseDetId::HFNoseFineN / HFNoseDetId::HFNoseFineTrigger;
  for (int u = 0; u < 2 * nCells; ++u) {
    for (int v = 0; v < 2 * nCells; ++v) {
      if (((v - u) < nCells) && (u - v) <= nCells) {
        HFNoseTriggerDetId newId(HFNoseTrigger, id.zside(), id.type(), id.layer(), id.waferU(), id.waferV(), u, v);
        ids.emplace_back(newId);
      }
    }
  }
  return ids;
}

std::vector<HFNoseTriggerDetId> HFNoseDetIdToModule::getTriggerDetIds(HGCalModuleDetId const& id) const {
  std::vector<HFNoseTriggerDetId> ids;
  int nCells = HFNoseDetId::HFNoseFineN / HFNoseDetId::HFNoseFineTrigger;

  int moduleU = 0;
  int moduleV = 0;
  uvMappingFromSector0( id.layer(), moduleU, moduleV, id.sector();

  for (int u = 0; u < 2 * nCells; ++u) {
    for (int v = 0; v < 2 * nCells; ++v) {
      if (((v - u) < nCells) && (u - v) <= nCells) {
        HFNoseTriggerDetId newId(HFNoseTrigger, id.zside(), id.type(), id.layer(), moduleU, moduleV, u, v);
        ids.emplace_back(newId);
      }
    }
  }
  return ids;
}

void HFNoseDetIdToModule::uvMappingFromSector0(unsigned layer, int& moduleU, int& moduleV, unsigned sector) const{
  int offset;
  
  if (sector==0){
    return;
  }
  
  if(layer<=28) { // CE-E
    offset=0;
  } else if((layer%2)==1) { // CE-H Odd
    offset=-1;
  } else { // CE-H Even
    offset=1;
  }
  
  int uPrime,vPrime;
  
  if(sector==1) {
    uPrime=-moduleV + offset;
    vPrime=moduleU - moduleV + offset;
  } else {
    uPrime=moduleV - moduleU;
    vPrime=-moduleU + offset;
  }
  
  moduleU=uPrime;
  moduleV=vPrime;
  
}
