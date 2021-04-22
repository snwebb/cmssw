#ifndef __L1Trigger_L1THGCal_HGCalSortingTruncationImplSA_h__
#define __L1Trigger_L1THGCal_HGCalSortingTruncationImplSA_h__

#include "L1Trigger/L1THGCal/interface/backend/HGCalCluster_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalSeed_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalMulticluster_SA.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalSortingTruncationConfig_SA.h"

#include <string>
#include <map>
#include <vector>
#include <utility>
#include <memory>

class HGCalSortingTruncationImplSA {
public:
  HGCalSortingTruncationImplSA();
  ~HGCalSortingTruncationImplSA() {}

  void runAlgorithm() const;

  void sortAndTruncate_SA(
			  const std::vector<l1t::HGCalMulticluster_SA> inputMulticlusters,	std::vector<l1t::HGCalMulticluster_SA>& outputMulticlusters,
      const std::unique_ptr<l1t::sortingTruncationAlgoConfig_SA>& configuration) const;

private:



};

#endif
