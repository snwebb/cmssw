#ifndef __L1Trigger_L1THGCal_HGCalSortingTruncationConfig_SA_h__
#define __L1Trigger_L1THGCal_HGCalSortingTruncationConfig_SA_h__

#include <vector>

namespace l1thgcfirmware {

  class sortingTruncationAlgoConfig {
  public:
    sortingTruncationAlgoConfig(std::vector<double> truncation_byRoverZbin)
        : truncation_byRoverZbin_(truncation_byRoverZbin) {}

    void setParameters(std::vector<double> truncation_byRoverZbin) { truncation_byRoverZbin = truncation_byRoverZbin; }

    void setParameters(const sortingTruncationAlgoConfig& newConfig) {
      setParameters(newConfig.truncation_byRoverZbin());
    }

    std::vector<double> truncation_byRoverZbin() const { return truncation_byRoverZbin_; }

  private:
    std::vector<double> truncation_byRoverZbin_;
  };

}  // namespace l1thgcfirmware

#endif
