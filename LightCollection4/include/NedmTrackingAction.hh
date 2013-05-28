#ifndef _NedmTrackingAction_H_
#define _NedmTrackingAction_H_

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class G4Track;

//! Keep track of the properties of a photon's history and pass them to the Trajectory and Analysis classes.
class NedmTrackingAction : public G4UserTrackingAction {
public:
  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*);
private:
};
#endif
