#include "NedmSteppingAction.hh"

#include "NedmAnalysisManager.hh"
#include "NedmDetectorConstruction.hh"
#include "NedmEventAction.hh"
#include "NedmTrackInformation.hh"

#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

#include "G4DynamicParticle.hh"
#include "G4Material.hh"
#include "G4PhysicsTable.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4ParticleDefinition.hh"

#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"

#include "G4OpBoundaryProcess.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpProcessSubType.hh"

#include "G4TrackStatus.hh"

#include <iostream>
using namespace std;


/*! Called when the simulation steps forward. A lot is happening in this method and it is worth taking a look at. This handles detection of photons as well as bounce and absorption tracking.
 */
void NedmSteppingAction::UserSteppingAction(const G4Step* aStep)
{

  // get point of entry and exit
  const G4StepPoint *p_in  = aStep->GetPreStepPoint();
  const G4StepPoint *p_out = aStep->GetPostStepPoint();
  //const G4VProcess *p1 = p_in->GetProcessDefinedStep();
  const G4VProcess *p2 = p_out->GetProcessDefinedStep();


  // Find boundary status

  G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  static G4OpBoundaryProcess* boundary=NULL;

  G4ParticleDefinition* particleType = aStep->GetTrack()->GetDefinition();
  
  if(particleType==G4OpticalPhoton::OpticalPhotonDefinition()){

    if(!boundary){
      G4ProcessManager* pm
	= aStep->GetTrack()->GetDefinition()->GetProcessManager();
      G4int nprocesses = pm->GetProcessListLength();
      G4ProcessVector* pv = pm->GetProcessList();
      G4int i;
      for( i=0;i<nprocesses;i++){
	if((*pv)[i]->GetProcessName()=="OpBoundary"){
	  boundary = (G4OpBoundaryProcess*)(*pv)[i];
	  break;
	}
      }
    }

    boundaryStatus = boundary->GetStatus();

    NedmTrackInformation* trackInfo;
    G4LogicalVolume* aVol;
    G4LogicalSurface* surface = NULL;
    G4StepStatus stepStatus = p_out->GetStepStatus();
    
    switch( stepStatus )
      {
      
      case fGeomBoundary:

	switch( boundaryStatus )
	  {
	  case Absorption:
            if( p_out->GetMaterial() ) {
              aVol = p_out->GetTouchableHandle()
                ->GetVolume()->GetLogicalVolume();
              trackInfo =
                (NedmTrackInformation *) aStep->GetTrack()->GetUserInformation();


              G4LogicalSurface* skin = G4LogicalSkinSurface::GetSurface(aVol);

              if(skin) {

                trackInfo->WasAbsorbed(skin);

              } else {

                surface =
                  G4LogicalBorderSurface::GetSurface(p_in->GetPhysicalVolume(),
                      p_out->GetPhysicalVolume());
                if(surface) {
                  trackInfo->WasAbsorbed(surface);
                }

              }
            }
	    break;
          case Detection:
            if( p_out->GetMaterial() ) {
              aVol = p_out->GetTouchableHandle()
                ->GetVolume()->GetLogicalVolume();
              trackInfo =
                (NedmTrackInformation *) aStep->GetTrack()->GetUserInformation();

              if( aVol->GetName() == "PMT" ) {
                G4int copy_no = p_out->GetPhysicalVolume()->GetCopyNo();

                trackInfo->set_detected(true);
                trackInfo->set_pmt_number(copy_no);
                trackInfo->set_time_detected(aStep->GetTrack()->GetGlobalTime());
		trackInfo->set_energy_detected((aStep->GetTrack()->GetKineticEnergy()));
		trackInfo->set_position_detected((aStep->GetTrack()->GetPosition()));
              }
            }
            break;

	    
	  case TotalInternalReflection:
	  case FresnelReflection:
          case LobeReflection:
          case SpikeReflection:
          case LambertianReflection:
	    if( p_out->GetMaterial() ) {
              surface =
		G4LogicalBorderSurface::GetSurface(p_in->GetPhysicalVolume(),
						   p_out->GetPhysicalVolume());
            }

            if(!surface) {
              surface = G4LogicalSkinSurface::GetSurface(
                  p_in->GetPhysicalVolume()->GetLogicalVolume());
            }

            if(!surface) {
              surface = G4LogicalSkinSurface::GetSurface(
                  p_out->GetPhysicalVolume()->GetLogicalVolume());
            }


            if(surface) {

              trackInfo =
                (NedmTrackInformation *) aStep->GetTrack()->GetUserInformation();
              trackInfo->RegisterBounce(surface);
            }

	    break;


          case FresnelRefraction:
          case SameMaterial:

            if( p_out->GetMaterial() ) {
            
	      G4String iName = p_in->GetPhysicalVolume()->GetName();

	      trackInfo =
		(NedmTrackInformation *) aStep->GetTrack()->GetUserInformation();
	    
	      if( iName == "CellSide") {

                G4String oName = p_out->GetPhysicalVolume()->GetName();
                if( oName == "LG") {
                  G4int copy_no = p_out->GetPhysicalVolume()->GetCopyNo();

                  copy_no = copy_no % 3;
                    
                  trackInfo->set_lg_number(copy_no + 1);
                }
	      }
            }
            break;
	  
	  default:
	    break;
	  }
	break;
      case fPostStepDoItProc:

        if(p2->GetProcessSubType() == fOpAbsorption) {
          trackInfo =
            (NedmTrackInformation *) aStep->GetTrack()->GetUserInformation();

          trackInfo->set_bulk();
        }
      default:
	break;
      }
  }

}

