#ifndef _NedmOpTPB_H_
#define _NedmOpTPB_H_

#include "globals.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4VParticleChange.hh"
#include "G4VDiscreteProcess.hh"

enum NedmOpProcessSubType {
  fOpTPB = 99
};

class NedmOpTPB : public G4VDiscreteProcess {

  public:
    NedmOpTPB(const G4String& processName = "OpTPB",
        G4ProcessType type = fUserDefined);

    ~NedmOpTPB();

  public:
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);

    G4double GetMeanFreePath(const G4Track&,
        G4double,
        G4ForceCondition* force);

    G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
        const G4Step& aStep);

  private:
    static const G4double the_tpb_photon_energy_;
    static const G4double the_euv_cutoff_;
    static const G4double the_penetration_depth_;

};


inline
G4bool NedmOpTPB::IsApplicable(const G4ParticleDefinition& aParticleType) {
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

#endif
