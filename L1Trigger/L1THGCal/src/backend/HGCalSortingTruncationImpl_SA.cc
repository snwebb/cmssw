#include "L1Trigger/L1THGCal/interface/backend/HGCalSortingTruncationImpl_SA.h"

#include <cmath>

HGCalSortingTruncationImplSA::HGCalSortingTruncationImplSA() {}

// void HGCalSortingTruncationImplSA::runAlgorithm() const {

// }

void HGCalSortingTruncationImplSA::sortAndTruncate_SA(
						      const std::vector<l1t::HGCalMulticluster_SA> inputMulticlusters,	std::vector<l1t::HGCalMulticluster_SA>& outputMulticlusters,
      const std::unique_ptr<l1t::sortingTruncationAlgoConfig_SA>& configuration) const {

}
