//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Make this appear first!
#include "G4Timer.hh"

#include "NedmRunAction.hh"
#include "NedmAnalysisManager.hh"
#include "G4Run.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NedmRunAction::NedmRunAction()
{
  timer = new G4Timer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

NedmRunAction::~NedmRunAction()
{
  delete timer;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*!
  Begins timing the execution of the current run and prepares the NedmAnalysisManager to record data.
*/
void NedmRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4int runID = aRun->GetRunID();
  G4cout << "### Run " << runID << " start." << G4endl; 
  timer->Start();
  NedmAnalysisManager::GetInstance()->BeginOfRun(runID);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*!
  Summarizes event and finalizes data stored to root file.
*/
void NedmRunAction::EndOfRunAction(const G4Run* aRun)
{   
  timer->Stop();
  G4cout << "number of event = " << aRun->GetNumberOfEvent() 
         << " " << *timer << G4endl;
  NedmAnalysisManager::GetInstance()->EndOfRun();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
