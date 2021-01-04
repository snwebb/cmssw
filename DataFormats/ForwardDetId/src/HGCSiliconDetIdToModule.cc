#include "DataFormats/ForwardDetId/interface/HGCSiliconDetIdToModule.h"

HGCSiliconDetIdToModule::HGCSiliconDetIdToModule() {}

std::vector<HGCSiliconDetId> HGCSiliconDetIdToModule::getDetIds(HGCSiliconDetId const& id) const {
  std::vector<HGCSiliconDetId> ids;
  int nCells = (id.type() == 0) ? HGCSiliconDetId::HGCalFineN : HGCSiliconDetId::HGCalCoarseN;
  for (int u = 0; u < 2 * nCells; ++u) {
    for (int v = 0; v < 2 * nCells; ++v) {
      if (((v - u) < nCells) && (u - v) <= nCells) {
        HGCSiliconDetId newId(id.det(), id.zside(), id.type(), id.layer(), id.waferU(), id.waferV(), u, v);
        ids.emplace_back(newId);
      }
    }
  }
  return ids;
}

std::vector<HGCalTriggerDetId> HGCSiliconDetIdToModule::getDetTriggerIds(HGCSiliconDetId const& id) const {
  std::vector<HGCalTriggerDetId> ids;
  int nCells = HGCSiliconDetId::HGCalFineN / HGCSiliconDetId::HGCalFineTrigger;
  int subdet = (id.det() == DetId::HGCalEE) ? HGCalEETrigger : HGCalHSiTrigger;
  for (int u = 0; u < 2 * nCells; ++u) {
    for (int v = 0; v < 2 * nCells; ++v) {
      if (((v - u) < nCells) && (u - v) <= nCells) {
        HGCalTriggerDetId newId(subdet, id.zside(), id.type(), id.layer(), id.waferU(), id.waferV(), u, v);
        ids.emplace_back(newId);
      }
    }
  }
  return ids;
}

std::vector<HGCalTriggerDetId> HGCSiliconDetIdToModule::getDetTriggerIds(HGCalModuleDetId const& id) const {
  std::vector<HGCalTriggerDetId> ids;
  int nCells = HGCSiliconDetId::HGCalFineN / HGCSiliconDetId::HGCalFineTrigger;
  int subdet = (id.det() == DetId::HGCalEE) ? HGCalEETrigger : HGCalHSiTrigger;
  int moduleU = 0;
  int moduleV = 0;
  uvMappingFromSector0( id.layer(), moduleU, moduleV, id.sector() );

  for (int u = 0; u < 2 * nCells; ++u) {
    for (int v = 0; v < 2 * nCells; ++v) {
      if (((v - u) < nCells) && (u - v) <= nCells) {
        HGCalTriggerDetId newId(subdet, id.zside(), id.type(), id.layer(), moduleU, moduleV, u, v);
        ids.emplace_back(newId);
      }
    }
  }
  return ids;
}

void HGCSiliconDetIdToModule::uvMappingFromSector0(unsigned layer, int& moduleU, int& moduleV, unsigned sector) const{
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
