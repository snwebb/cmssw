#ifndef DataFormats_ForwardDetId_HGCalModuleDetId_H
#define DataFormats_ForwardDetId_HGCalModuleDetId_H 1

#include <iosfwd>
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/ForwardDetId/interface/ForwardSubdetector.h"

/* \brief description of the bit assigment
   [0:3]  u-coordinate of the silicon module (u-axis points along -x axis)
          or eta-coordinate of the scintillator module
   [4:7]  v-coordinate of the silicon module (v-axis points 60-degree wrt x-axis)
          or phi-coordinate of the scintillator module
   [8:9] sector (0,1,2 counter-clockwise from u-axis)

   [10:17] reserved for future use

   [18:22] layer number 
   [23:24] Type (0 fine divisions of wafer with 120 mum thick silicon
                 1 coarse divisions of wafer with 200 mum thick silicon
                 2 coarse divisions of wafer with 300 mum thick silicon
                 0 fine divisions of scintillators
                 1 coarse divisions of scintillators)

   [25:26] Subdetector Type (HFNoseTrigger/HGCalEETrigger/HGCalHSiTrigger/HGCalHScTrigger)
   [27:27] z-side (0 for +z; 1 for -z)
   [28:31] Detector type (HGCalTrigger)
*/


class HGCalModuleDetId : public DetId {
public:
  /** Create a null cellid*/
  HGCalModuleDetId();
  /** Create cellid from raw id (0=invalid tower id) */
  HGCalModuleDetId(uint32_t rawid);
  /** Constructor from subdetector, zplus, type, layer, sector, module numbers */
  HGCalModuleDetId(int subdet, int zp, int type, int layer, int sector, int moduleU, int moduleV);
  /** Constructor from a generic cell id */
  HGCalModuleDetId(const DetId& id);
  /** Assignment from a generic cell id */
  HGCalModuleDetId& operator=(const DetId& id);

  /// get the subdetector
  HGCalTriggerSubdetector subdet() const {
    return (HGCalTriggerSubdetector)((id_ >> kHGCalSubdetOffset) & kHGCalSubdetMask);
  }

  /// get the type
  int type() const { return (id_ >> kHGCalTypeOffset) & kHGCalTypeMask; }

  /// get the z-side of the cell (1/-1)
  int zside() const { return ((id_ >> kHGCalZsideOffset) & kHGCalZsideMask ? 1 : -1); }

  /// get the layer #
  int layer() const { return (id_ >> kHGCalLayerOffset) & kHGCalLayerMask; }

  /// get the sector #
  int sector() const { return (id_ >> kHGCalSectorOffset) & kHGCalSectorMask; }

  /// get the module U
  int moduleU() const { return (id_ >> kHGCalModuleUOffset) & kHGCalModuleUMask; }

  /// get the module V
  int moduleV() const { return (id_ >> kHGCalModuleVOffset) & kHGCalModuleVMask; }

  /// get the scintillator panel eta
  int eta() const { return moduleU(); }

  /// get the scintillator panel phi
  int phi() const { return moduleV(); }

  /* /\** Converter for a geometry cell id *\/ */
  /* HGCalModuleDetId geometryCell() const { return id_ & kHGCalMaskCell; } */

  /// consistency check : no bits left => no overhead
  bool isHFNose() const { return (subdet() == HFNoseTrigger); }
  bool isEE() const { return (subdet() == HGCalEETrigger); }
  bool isHSilicon() const { return (subdet() == HGCalHSiTrigger); }
  bool isHScintillator() const { return (subdet() == HGCalHScTrigger); }
  bool isForward() const { return true; }
  
  //get u and v rotated to whole detector coordinates 
  int wholeDetectorU() const;
  int wholeDetectorV() const;

  static const HGCalModuleDetId Undefined;

  static const int kHGCalModuleUOffset = 0;
  static const int kHGCalModuleUMask = 0xF;
  static const int kHGCalModuleVOffset = 4;
  static const int kHGCalModuleVMask = 0xF;
  static const int kHGCalSectorOffset = 8;
  static const int kHGCalSectorMask = 0x3;
  static const int kHGCalLayerOffset = 18;
  static const int kHGCalLayerMask = 0x1F;
  static const int kHGCalTypeOffset = 23;
  static const int kHGCalTypeMask = 0x3;
  static const int kHGCalSubdetOffset = 25;
  static const int kHGCalSubdetMask = 0x3;
  static const int kHGCalZsideOffset = 27;
  static const int kHGCalZsideMask = 0x1;

};

std::ostream& operator<<(std::ostream&, const HGCalModuleDetId& id);

#endif
