////////////////////////////////////////////////////////////////////////
//
//  UCNsimpleabsorption, 7.9.04, peter fierlinger
//			 21.4.06, marcin kuzniak
//
////////////////////////////////////////////////////////////////////////

#ifndef UCNSimpleAbsorption_h
#define UCNSimpleAbsorption_h 1


#include "globals.hh"
#include "templates.hh"
#include "Randomize.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4UCN.hh"

// simple discrete process,
// absorbs UCN due to absorption cross section of the material

class UCNSimpleAbsorption : public G4VDiscreteProcess
{

public: // Without description

        UCNSimpleAbsorption(const G4String& processName = "UCNSimpleAbsorption");

	~UCNSimpleAbsorption();

public: // With description

        G4bool IsApplicable(const G4ParticleDefinition& aParticleType);

	G4double GetMeanFreePath(const G4Track& aTrack,
				 G4double ,
				 G4ForceCondition* condition);

	G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
 				        const G4Step&  aStep);

	static UCNSimpleAbsorption* GetInstance();
	void setVerbose(G4int level);
private:

	G4double decaytime;
	static UCNSimpleAbsorption* theInstance;
	G4int verboseLevel;
};

////////////////////
// Inline methods
////////////////////

inline
G4bool UCNSimpleAbsorption::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return ( &aParticleType == G4UCN::UCN() );
}

#endif /* UCNSimpleAbsorption_h */
