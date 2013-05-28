#include "NedmAnalysisMessenger.hh"
#include "NedmAnalysisManager.hh"

#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

NedmAnalysisMessenger::NedmAnalysisMessenger(NedmAnalysisManager* man) {
  analysis = man;
  
  analysisDir = new G4UIdirectory("/nedm/analysis/");
  analysisDir->SetGuidance("Adjust preperation of data for analysis.");

  rootFileCmd = new G4UIcmdWithAString("/nedm/analysis/rootfile", this);
  rootFileCmd->SetGuidance("Set the name of the rootfile to write data to.");
  rootFileCmd->SetParameterName("filename", false);
  
}

NedmAnalysisMessenger::~NedmAnalysisMessenger() {
  delete rootFileCmd;
  delete analysisDir;
}

void NedmAnalysisMessenger::SetNewValue(G4UIcommand* cmd, G4String str) {
  if(cmd == rootFileCmd) {
    analysis->set_root_file_name(str);
  }
}
