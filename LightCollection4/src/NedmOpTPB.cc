#include "NedmOpTPB.hh"

#include "NedmTrackInformation.hh"

#include "G4RandomTools.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

const G4double NedmOpTPB::the_euv_cutoff_ = 10*eV;
const G4double NedmOpTPB::the_tpb_photon_energy_ = 2.82*eV;
const G4double NedmOpTPB::the_penetration_depth_ = 0.01 * mm;


NedmOpTPB::NedmOpTPB(const G4String& processName, G4ProcessType type) :
  G4VDiscreteProcess(processName, type) {
    SetProcessSubType(fOpTPB);

}

NedmOpTPB::~NedmOpTPB() {}

/*! Determines the distance scale on which to check this process. We want to always check,
 * much like is done for the Optical Processes.
 */
G4double NedmOpTPB::GetMeanFreePath(const G4Track& aTrack, G4double previousStepSize, G4ForceCondition* force) {
  *force = Forced;

  return DBL_MAX;
}

G4VParticleChange* NedmOpTPB::PostStepDoIt(const G4Track& aTrack, const G4Step& aStep) {

  pParticleChange->Initialize(aTrack);

  //If this particle has a parent ID, then it must be a secondary and thus we don't
  //need to consider it here.
  if (aTrack.GetParentID())
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

  G4StepPoint* preStepPoint = aStep.GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep.GetPostStepPoint();

  if (postStepPoint->GetStepStatus() != fGeomBoundary) {
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  //Only EUV photons should be converted by the TPB.
  if (aTrack.GetTotalEnergy() < the_euv_cutoff_) {
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }



  //Search the Material and Surface property tables for a conversion efficiency
  const G4Material* aMaterial = preStepPoint->GetMaterial();

  G4double conversionEfficiency;

  G4MaterialPropertiesTable* aMaterialPropertiesTable = aMaterial->GetMaterialPropertiesTable();


  //Check our material first
  if(aMaterialPropertiesTable &&
      aMaterialPropertiesTable->ConstPropertyExists("TPBCONVERSIONEFFICIENCY")) {

    conversionEfficiency = aMaterialPropertiesTable->GetConstProperty("TPBCONVERSIONEFFICIENCY");

  } else {
    //Now we check any optical surfaces associated with this boundary
    G4LogicalSurface* Surface = NULL;
    G4OpticalSurface* OpticalSurface = NULL;

    Surface = G4LogicalBorderSurface::GetSurface(preStepPoint->GetPhysicalVolume(),
        postStepPoint->GetPhysicalVolume());

    if(Surface) 
      OpticalSurface = dynamic_cast<G4OpticalSurface*>(Surface->GetSurfaceProperty());


    if(OpticalSurface)
      aMaterialPropertiesTable = OpticalSurface->GetMaterialPropertiesTable();

    if(aMaterialPropertiesTable &&
        aMaterialPropertiesTable->ConstPropertyExists("TPBCONVERSIONEFFICIENCY")) {

      conversionEfficiency =
        aMaterialPropertiesTable->GetConstProperty("TPBCONVERSIONEFFICIENCY");

    } else {

      //We have no conversion efficiency, so we are done.
      return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
    }
  }

  //Our original particle is absorbed by the tpb
  pParticleChange->ProposeTrackStatus(fStopAndKill);

  if (G4UniformRand() > conversionEfficiency) {
    return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
  }

  //Create our secondary photons
  pParticleChange->SetNumberOfSecondaries(1);

  //The generated photons are emitted isotropically with a fixed
  //kinetic energy
  G4ParticleMomentum momentumDirection = G4RandomDirection();
  G4ThreeVector polar = G4PlaneVectorRand(momentumDirection);

  G4DynamicParticle* aTPBPhoton = new G4DynamicParticle(G4OpticalPhoton::OpticalPhoton(),
      momentumDirection);


  aTPBPhoton->SetPolarization(polar.x(),
      polar.y(),
      polar.z());

  aTPBPhoton->SetKineticEnergy(the_tpb_photon_energy_);


  G4double aSecondaryTime = postStepPoint->GetGlobalTime();
  G4ThreeVector aSecondaryPosition =
    the_penetration_depth_ * aTrack.GetMomentumDirection() +
    postStepPoint->GetPosition();

  G4Track* aSecondaryTrack = new G4Track(aTPBPhoton, aSecondaryTime, aSecondaryPosition);

  aSecondaryTrack->SetTouchableHandle(aTrack.GetTouchableHandle());

  aSecondaryTrack->SetParentID(aTrack.GetTrackID());

  pParticleChange->AddSecondary(aSecondaryTrack);

  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);

}
