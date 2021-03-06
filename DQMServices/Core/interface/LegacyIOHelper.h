#ifndef DQMSERVICES_CORE_LEGACYIOHELPER_H
#define DQMSERVICES_CORE_LEGACYIOHELPER_H

#include "DQMServices/Core/interface/DQMStore.h"

// This class encapsulates the TDirectory based file format used for DQMGUI
// uploads and many other use cases.
// This should be part of `DQMFileSaver`, however since DQMServices/Components
// DQMFileSaver and DQMServices/FileIO DQMFileSaverOnline both write this
// format, the code is shared here (evnetually, these modules should become one
// again).
// This code is in DQMServices/Core to also allow the legacy DQMStore::save
// interface to use this without adding another dependency.
class LegacyIOHelper {
public:
  typedef dqm::legacy::DQMStore DQMStore;
  typedef dqm::legacy::MonitorElement MonitorElement;
  LegacyIOHelper(DQMStore* dqmstore) : dbe_(dqmstore){};

  // Replace or append to `filename`, a TDirectory ROOT file. If a run number
  // is passed, the paths are rewritten to the "Run Summary" format used by
  // DQMGUI. The run number does not affect which MEs are saved; this code only
  // supports non-threaded mode. `fileupdate` is passed to ROOT unchanged.
  // TODO: allow to select only RUN, LUMI or JOB histograms.
  void save(std::string const& filename, uint32_t const run = 0, std::string const& fileupdate = "RECREATE");

private:
  bool createDirectoryIfNeededAndCd(const std::string& path);
  DQMStore* dbe_;
};

#endif
