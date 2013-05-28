#include "NedmEventAction.hh"

#include "NedmRunAction.hh"

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include <iostream>

#include "NedmAnalysisManager.hh"


/*!
  Resets the detected photon count and prints out a status update every 1000 events.
*/
void NedmEventAction::BeginOfEventAction(const G4Event* evt)
{  
  if( (evt->GetEventID()%10000)==0 ) {
    std::cout << "\r <<< " << evt->GetEventID() << " events simulated. " << std::flush;
  }

  NedmAnalysisManager *analysis = NedmAnalysisManager::GetInstance();
  analysis->set_detected(0);
  analysis->set_time_detected(0);
}


void NedmEventAction::EndOfEventAction(const G4Event* evt)
{
  NedmAnalysisManager *analysis = NedmAnalysisManager::GetInstance();
  
  analysis->FillTree();
}  

