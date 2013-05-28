//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*! @mainpage nEDM Lightguide Simulations

Code for simulation of the nEDM light collection system.
*/


#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "G4ios.hh"
#include <stdlib.h>

#include "NedmDetectorConstruction.hh"
//#include "NedmGDMLDetectorConstruction.hh"
#include "NedmPhysicsList.hh"
#include "NedmPrimaryGeneratorAction.hh"
#include "NedmRunAction.hh"
#include "NedmEventAction.hh"
#include "NedmSteppingAction.hh"
#include "NedmAnalysisManager.hh"
#include "NedmTrackingAction.hh"

#include "Randomize.hh"
#include "time.h"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {

  // Seed the random number generator manually
  //G4long myseed = 6827978;
  G4long myseed = time(0);
  CLHEP::HepRandom::setTheSeed(myseed);
  
  // Run manager
  G4RunManager* runManager = new G4RunManager;

  // UserInitialization classes - mandatory

  G4VUserDetectorConstruction* det;
  det = new NedmDetectorConstruction;

  runManager-> SetUserInitialization(det);

  runManager-> SetUserInitialization(new NedmPhysicsList);
  
#ifdef G4VIS_USE
  // visualization manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // UserAction classes 
  runManager->SetUserAction(new NedmPrimaryGeneratorAction);

  NedmRunAction *runact = new NedmRunAction;
  runManager->SetUserAction(runact);
  
  NedmEventAction *evtact = new NedmEventAction;  
  runManager->SetUserAction(evtact);

  NedmSteppingAction *stepact = new NedmSteppingAction;
  runManager->SetUserAction(stepact);

  NedmTrackingAction *trackact = new NedmTrackingAction;
  runManager->SetUserAction(trackact);


  //Initialize G4 kernel
  runManager->Initialize();
  
  //Make sure that the analysis manager has been created so that
  //we can configure it before we do a run
  NedmAnalysisManager* analysis = NedmAnalysisManager::GetInstance();
    
  // get the pointer to the User Interface manager 
  G4UImanager* UI = G4UImanager::GetUIpointer(); 
   
  if (argc==1)   //define UI session for interactive mode
    {
      G4UIsession* session = 0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);      
#else
      session = new G4UIterminal();
#endif    
      UI->ApplyCommand("/control/execute vis.mac"); 
      session->SessionStart();
      delete session;
   }
   
  else         // Batch mode
   {
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     UI->ApplyCommand(command+fileName);
   }
   
  dynamic_cast<NedmDetectorConstruction*>(det)->PrintParameters();
  // job termination
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
  

  // Manually delete analysis class while ROOT is still loaded so that
  // it doesn't complain
  delete analysis;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
