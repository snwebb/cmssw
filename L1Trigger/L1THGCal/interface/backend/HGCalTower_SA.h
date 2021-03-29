#ifndef L1Trigger_L1THGCal_HGCalTower_SA_h
#define L1Trigger_L1THGCal_HGCalTower_SA_h

#include <unordered_map>

namespace l1t {

  class HGCalTower_SA {
    public:
      HGCalTower_SA() {}
      HGCalTower_SA( double etEm, double etHad ) :
        etEm_(etEm),
        etHad_(etHad)
      {}

      ~HGCalTower_SA(){};

      double etEm() const { return etEm_; };
      double etHad() const { return etHad_; };
  
      HGCalTower_SA& operator+=(const HGCalTower_SA& tower);

    private:
      double etEm_;
      double etHad_;
  };
}  // namespace l1t

#endif
