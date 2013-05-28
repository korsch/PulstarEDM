#ifndef NedmEventAction_h
#define NedmEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"


/*! @brief Takes user defined action before and after each event.

  Contains code to keep track of whether a produced photon is detected during this event. The actual check of the above is done in NedmSteppingAction::UserStepAction
 */
class NedmEventAction : public G4UserEventAction
{

 public:
  //! Prepare for the simulation of an event
  void BeginOfEventAction(const G4Event*);
  //! Cleans up by writing event data to ROOT Histogram.
  void EndOfEventAction(const G4Event*);

};

#endif

    
