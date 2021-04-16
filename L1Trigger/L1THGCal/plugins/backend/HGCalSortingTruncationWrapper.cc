#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/L1THGCal/interface/backend/HGCalAlgoWrapperBase.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalSortingTruncationImpl_SA.h"

#include "DataFormats/L1THGCal/interface/HGCalMulticluster.h"
#include "L1Trigger/L1THGCal/interface/backend/HGCalMulticluster_SA.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "L1Trigger/L1THGCal/interface/HGCalTriggerGeometryBase.h"
#include "L1Trigger/L1THGCal/interface/HGCalTriggerTools.h"

class HGCalSortingTruncationWrapper : public HGCalSortingTruncationWrapperBase {
public:
  HGCalSortingTruncationWrapper(const edm::ParameterSet& conf);
  ~HGCalSortingTruncationWrapper() override {}

  void configure(const std::pair<const edm::EventSetup&, const edm::ParameterSet&>& configuration) override;

  void process(const l1t::HGCalMulticlusterBxCollection, l1t::HGCalMulticlusterBxCollection) const override;

private:
  void convertCMSSWInputs(const l1t::HGCalMulticlusterBxCollection& multiclusters, l1t::HGCalMulticlusterSACollection& multiclusters_SA) const;
  void convertAlgorithmOutputs(const l1t::HGCalMulticlusterSACollection& multiclusters_out,
                               l1t::HGCalMulticlusterBxCollection& multiclusters) const;

  void sortAndTruncate(const l1t::HGCalMulticlusterSACollection& inputMulticlusters, l1t::HGCalMulticlusterSACollection& outputMulticlusters) const;

  void eventSetup(const edm::EventSetup& es) {
    triggerTools_.eventSetup(es);
  }

  HGCalTriggerTools triggerTools_;
  HGCalSortingTruncationImplSA theAlgo_;
  std::unique_ptr<l1t::sortingTruncationAlgoConfig_SA> theConfiguration_;

};

HGCalSortingTruncationWrapper::HGCalSortingTruncationWrapper(const edm::ParameterSet& conf)
    : theAlgo_(),
      theConfiguration_(std::make_unique<l1t::sortingAlgoConfig_SA>(truncation_byRoverZbin)) {
}

void HGCalSortingTruncationWrapper::convertCMSSWInputs(const l1t::HGCalMulticlusterBxCollection& multiclusters,
						       l1t::HGCalMulticlusterSACollection& multiclusters_SA) const {


  std::vector<l1t::HGCalMulticluster> multiclusters;
  multiclusters.reserve(multiclusters_out.size());
  for (unsigned int imulticluster = 0; imulticluster < multiclusters_out.size(); ++imulticluster) {
    bool firstConstituent = true;
    for (const auto& constituent : multiclusters_out[imulticluster].constituents()) {
      if (firstConstituent) {
        multiclusters.emplace_back(l1t::HGCalMulticluster(clustersPtrs.at(constituent.index_cmssw()), 1.));
      } else {
        multiclusters.at(imulticluster).addConstituent(clustersPtrs.at(constituent.index_cmssw()), 1.);
      }
      firstConstituent = false;
    }
  }


  multiclusters_SA.clear();
  multiclusters_SA.reserve(multiclusters.size());
  unsigned int clusterIndex = 0;
  for (const auto& cluster : clustersPtrs) {
    clusters_SA.emplace_back(l1t::HGCalCluster_SA(cluster->centreProj().x(),
                                                  cluster->centreProj().y(),
                                                  triggerTools_.zside(cluster->detId()),
                                                  triggerTools_.layerWithOffset(cluster->detId()),
                                                  cluster->eta(),
                                                  cluster->phi(),
                                                  cluster->pt(),
                                                  cluster->mipPt(),
                                                  clusterIndex));
    ++clusterIndex;
  }

  seeds_SA.clear();
  seeds_SA.reserve(seeds.size());
  for (const auto& seed : seeds) {
    seeds_SA.emplace_back(l1t::HGCalSeed_SA(seed.first.x(), seed.first.y(), seed.first.z(), seed.second));
  }
}

void HGCalSortingTruncationWrapper::convertAlgorithmOutputs(
    const std::vector<l1t::HGCalMulticluster_SA>& multiclusters_out,
    const std::vector<l1t::HGCalCluster_SA>& rejected_clusters_out,
    const std::vector<edm::Ptr<l1t::HGCalCluster>>& clustersPtrs,
    l1t::HGCalMulticlusterBxCollection& multiclustersBXCollection,
    l1t::HGCalClusterBxCollection& rejected_clusters) const {
  // Not doing completely the correct thing here
  // Taking the multiclusters from the stand alone emulation
  // Getting their consistuent clusters (stand alone objects)
  // Linking back to the original CMSSW-type cluster
  // And creating a CMSSW-type multicluster based from these clusters
  // So the output multiclusters will not be storing bit accurate quantities (or whatever was derived by the stand along emulation)
  // As these inherit from L1Candidate, could set their HW quantities to the bit accurate ones
  for (const auto& rejected_cluster : rejected_clusters_out) {
    rejected_clusters.push_back(0, *clustersPtrs.at(rejected_cluster.index_cmssw()));
  }

  std::vector<l1t::HGCalMulticluster> multiclusters;
  multiclusters.reserve(multiclusters_out.size());
  for (unsigned int imulticluster = 0; imulticluster < multiclusters_out.size(); ++imulticluster) {
    bool firstConstituent = true;
    for (const auto& constituent : multiclusters_out[imulticluster].constituents()) {
      if (firstConstituent) {
        multiclusters.emplace_back(l1t::HGCalMulticluster(clustersPtrs.at(constituent.index_cmssw()), 1.));
      } else {
        multiclusters.at(imulticluster).addConstituent(clustersPtrs.at(constituent.index_cmssw()), 1.);
      }
      firstConstituent = false;
    }
  }

  for (const auto& multicluster : multiclusters) {
    multiclustersBXCollection.push_back(0, multicluster);
  }
}
	
void HGCalSortingTruncationWrapper::process(const l1t::HGCalMulticlusterBxCollection inputMulticlusters, l1t::HGCalMulticlusterBxCollection outputMulticlusters) const {

      l1t::HGCalMulticlusterSACollection multiclusters_SA;
      convertCMSSWInputs(inputMulticlusters, multiclusters_SA);
      
      l1t::HGCalMulticlusterSACollection multiclusters_finalized_SA;
      sortAndTruncate(multiclusters_SA, multiclusters_finalized_SA);
      
      convertAlgorithmOutputs(multiclusters_finalized_SA, outputMulticlusters);

}

void HGCalSortingTruncationWrapper::sortAndTruncate(const l1t::HGCalMulticlusterSACollection& inputMulticlusters, l1t::HGCalMulticlusterSACollection& outputMulticlusters) const {
  // Call SA sorting
  std::vector<l1t::HGCalMulticluster_SA> multiclusters_vec_sortedtruncated_SA =
      theAlgo_.sortAndTruncate_SA(inputMulticlusters, outputMulticlusters, theConfiguration_);

}

void HGCalSortingTruncationWrapper::configure(
    const std::pair<const edm::EventSetup&, const edm::ParameterSet&>& configuration) {
  eventSetup(configuration.first);
  // theConfiguration_->setParameters( ... );
};

DEFINE_EDM_PLUGIN(HGCalSortingTruncationWrapperBaseFactory, HGCalSortingTruncationWrapper, "HGCalSortingTruncationWrapper");
