//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: F02RunAction.cc,v 1.1.1.1 2004/10/25 12:36:47 kuzniak Exp $
// GEANT4 tag $Name:  $
//
// 

#include "F02RunAction.hh"
#include "F02RunMessenger.hh"

#include "G4Run.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include <iomanip>

#include "Randomize.hh"

#include "UCNI.hh"
#include "UCNF.hh"
#include "UCNHists.hh"
#include "TFile.h"

//const char* OutputFile = "Results.dat";
ofstream  runsummary;

UCNI* initial= new UCNI();
UCNF* final= new UCNF();


long int count_tube01(0);
long int count_tube01a(0);
long int count_cons1(0);
long int count_cons2(0);
long int count_bend1(0);
long int count_bend1a(0);
long int count_bend2a(0);
long int count_bend2b(0);
long int count_bend3a(0);
long int count_bend3b(0);
long int count_tube3(0);
long int count_tube3a(0);
long int count_reflector(0);
long int count_tube4(0);
long int count_tube4a(0);
long int count_tube4b(0);
long int count_tube5(0);
long int count_v1valve(0);
long int count_v1rod(0);
long int countneutrons(0);
G4double totaltime(0);


//////////////////////////////////////////////////////////////////////////////

F02RunAction::F02RunAction()
  : saveRndm(0)
{
  runMessenger = new F02RunMessenger(this);
}

////////////////////////////////////////////////////////////////////////////

F02RunAction::~F02RunAction()
{
  delete runMessenger;
}

/////////////////////////////////////////////////////////////////////////////

void F02RunAction::BeginOfRunAction(const G4Run* aRun)
{  

  extern UCNHists* ucn;
  extern UCNI* initial;
  extern UCNF* final;
  extern ofstream  runsummary;
  runsummary.open ("RunSummary.txt");

  ucn->Construct("initial",initial);
  ucn->Construct("final",final);

  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
  
  // save Rndm status
  if (saveRndm > 0)
  { 
    CLHEP::HepRandom::showEngineStatus();
    CLHEP::HepRandom::saveEngineStatus("beginOfRun.rndm");
  }  
  G4UImanager* UI = G4UImanager::GetUIpointer();
   
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();

  if(pVVisManager)    UI->ApplyCommand("/vis/scene/notifyHandlers");
  
  G4cout<< "+++++++++++++++++++++++Run action: begin" << G4endl;

}

/////////////////////////////////////////////////////////////////////////////

void F02RunAction::EndOfRunAction(const G4Run* aRun)
{

  extern ofstream  runsummary;
  extern TFile* rootfile;

  rootfile->Write();

  G4double totalneutrons = aRun->GetNumberOfEventToBeProcessed();

  extern long int count_tube01;
  extern long int count_tube01a;
  extern long int count_cons1;
  extern long int count_cons2;
  extern long int count_bend1;
  extern long int count_bend1a;
  extern long int count_bend2a;
  extern long int count_bend2b;
  extern long int count_bend3a;
  extern long int count_bend3b;
  extern long int count_tube3;
  extern long int count_tube3a;
  extern long int count_reflector;
  extern long int count_tube4;
  extern long int count_tube4a;
  extern long int count_tube4b;
  extern long int count_tube5;
  extern long int count_v1valve;
  extern long int count_v1rod;

  extern  long int countneutrons;
  extern G4double totaltime;

  G4cout<< G4endl; 
  G4cout<< G4endl; 
  G4cout<< "-----------------------------------------------------------" << G4endl; 

  G4cout << "total # of neutrons in this run: " << totalneutrons << G4endl;
  G4cout << "total # of neutrons transmitted: " << countneutrons << G4endl;
  G4cout << "total time for transport to cell: " << totaltime << " s (for: " << countneutrons << " UCNs)" << G4endl;
  if(countneutrons)
    G4cout << "average time for transport to cell: " << totaltime/double(countneutrons) << " s " << G4endl;
  double eff = double(countneutrons)/double(totalneutrons);
  G4cout << "transport efficiency: " << eff << G4endl;
  G4cout << "Figure of Merit (<t>/eff): " << totaltime/double(countneutrons)/eff << G4endl;
  

  G4cout<< "-----------------------------------------------------------" << G4endl; 
  G4cout<< G4endl; 
  G4cout<< G4endl; 

  G4cout << "gray tube:                 " <<  double(count_tube01)/double(countneutrons) << G4endl;
  G4cout << "cone section:              " <<  double(count_cons1)/double(countneutrons) << G4endl;
  G4cout << "yellow section 1:          " <<  double(count_bend1)/double(countneutrons) << G4endl;
  G4cout << "yellow section 2:          " <<  double(count_bend2a)/double(countneutrons) + double(count_bend2b)/double(countneutrons)  << G4endl; 
  G4cout << "infection cell (cylinder): " <<  double(count_tube3)/double(countneutrons) << G4endl; 
  G4cout << "reflector:                 " <<  double(count_reflector)/double(countneutrons) << G4endl;
  G4cout << "Endcap (w/ hole):          " <<  double(count_tube4)/double(countneutrons) << G4endl; 
  G4cout << "Endcap (no hole):          " <<  double(count_tube5)/double(countneutrons) << G4endl; 
  G4cout << "Channel to target cell:    " <<  double(count_tube4a)/double(countneutrons) << G4endl;
  G4cout << "v1valve:                   " <<  double(count_v1valve)/double(countneutrons) << G4endl;
  G4cout << "v1rod:                     " <<  double(count_v1rod)/double(countneutrons) << G4endl; 
  G4cout << "UCN stop                   " << countneutrons << G4endl;


  G4cout<< G4endl; 
  G4cout<< G4endl; 
  G4cout<< "-----------------------------------------------------------" << G4endl; 


  runsummary<< G4endl; 
  runsummary<< G4endl; 
  runsummary<< "-----------------------------------------------------------" << G4endl; 

  runsummary << "total # of neutrons in this run: " << totalneutrons << G4endl;
  runsummary << "total # of neutrons transmitted: " << countneutrons << G4endl;
  runsummary << "total time for transport to cell: " << totaltime << " s (for: " << countneutrons << " UCNs)" << G4endl;
  if(countneutrons)
    runsummary << "average time for transport to cell: " << totaltime/double(countneutrons) << " s " << G4endl;
  runsummary << "transport efficiency: " << eff << G4endl;
  runsummary << "Figure of Merit (<t>/eff): " << totaltime/double(countneutrons)/eff << G4endl;
  

  runsummary<< "-----------------------------------------------------------" << G4endl; 
  runsummary<< G4endl; 
  runsummary<< G4endl; 

  runsummary << "gray tube:                 " <<  double(count_tube01)/double(countneutrons) << G4endl;
  runsummary << "cone section:              " <<  double(count_cons1)/double(countneutrons) << G4endl;
  runsummary << "yellow section 1:          " <<  double(count_bend1)/double(countneutrons) << G4endl;
  runsummary << "yellow section 2:          " <<  double(count_bend2a)/double(countneutrons) + double(count_bend2b)/double(countneutrons)  << G4endl; 
  runsummary << "infection cell (cylinder): " <<  double(count_tube3)/double(countneutrons) << G4endl; 
  runsummary << "reflector:                 " <<  double(count_reflector)/double(countneutrons) << G4endl;
  runsummary << "Endcap (w/ hole):          " <<  double(count_tube4)/double(countneutrons) << G4endl; 
  runsummary << "Endcap (no hole):          " <<  double(count_tube5)/double(countneutrons) << G4endl; 
  runsummary << "Channel to target cell:    " <<  double(count_tube4a)/double(countneutrons) << G4endl;
  runsummary << "v1valve:                   " <<  double(count_v1valve)/double(countneutrons) << G4endl;
  runsummary << "v1rod:                     " <<  double(count_v1rod)/double(countneutrons) << G4endl; 
  runsummary << "UCN stop                   " << countneutrons << G4endl;


  runsummary<< G4endl; 
  runsummary<< G4endl; 
  runsummary<< "-----------------------------------------------------------" << G4endl; 

  runsummary.close();

  if (G4VVisManager::GetConcreteInstance())
  {
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");
  }

  // save Rndm status

  if (saveRndm == 1)
  { 
    CLHEP::HepRandom::showEngineStatus();
    CLHEP::HepRandom::saveEngineStatus("endOfRun.rndm");
  }     
  G4cout<< "+++++++++++++++++++++++++++ Run action: end" << G4endl;


}

//
//
////////////////////////////////////////////////////////////////////////
