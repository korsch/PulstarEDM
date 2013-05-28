#ifndef NedmSteppingAction_h
#define NedmSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class NedmDetectorConstruction;
class NedmEventAction;

//! User defined actions to be taken for each simulation step.
class NedmSteppingAction : public G4UserSteppingAction
{
  public:
    void UserSteppingAction(const G4Step*);
    
  private:

    NedmDetectorConstruction* detector;

};

#endif
