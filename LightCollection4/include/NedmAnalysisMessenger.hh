#ifndef _NedmAnalysisMessenger_H_
#define _NedmAnalysisMessenger_H_

#include "G4UImessenger.hh"

#include "globals.hh"

class G4UIcmdWithAString;
class G4UIdirectory;
class G4UIcommand;

class NedmAnalysisManager;


/*! @brief Allows the user to adjust preperation of data for analysis.

  Currently this is only here to allow the name of the data root file to be changed interactively.
*/
class NedmAnalysisMessenger : public G4UImessenger {

public:
  NedmAnalysisMessenger(NedmAnalysisManager*);
  ~NedmAnalysisMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  NedmAnalysisManager* analysis;
  G4UIdirectory* analysisDir;
  //! /nedm/analysis/rootfile
  G4UIcmdWithAString* rootFileCmd;

};

#endif
