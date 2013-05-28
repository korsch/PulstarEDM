#include "NedmTrackingAction.hh"
#include "NedmTrackInformation.hh"
#include "NedmTrajectory.hh"
#include "NedmAnalysisManager.hh"

#include "G4TrackingManager.hh"
#include "G4Track.hh"

void NedmTrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
  fpTrackingManager->SetTrajectory(new NedmTrajectory(aTrack));
  fpTrackingManager->SetUserTrackInformation(new NedmTrackInformation);
}

void NedmTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  NedmTrajectory* trajectory = (NedmTrajectory*) fpTrackingManager->GimmeTrajectory();
  NedmTrackInformation* trackInfo = (NedmTrackInformation*) aTrack->GetUserInformation();
  NedmAnalysisManager* analysis = NedmAnalysisManager::GetInstance();
  
  if(trackInfo->detected()) {
    G4int pmtCopyNo = trackInfo->pmt_number();
    trajectory->set_detected(true);
    analysis->set_detected(1);
    analysis->set_pmt_number(pmtCopyNo);
    trajectory->set_pmt_number(pmtCopyNo);
    //The detected time is in nano-second
    analysis->set_time_detected(trackInfo->time_detected()/ns);
    analysis->set_energy_detected(trackInfo->energy_detected()/eV);
    analysis->set_x_detected(trackInfo->position_detected().x()/mm);
    analysis->set_y_detected(trackInfo->position_detected().y()/mm);
    analysis->set_z_detected(trackInfo->position_detected().z()/mm);
  }

  G4int PID = aTrack->GetParentID();
  trajectory->set_converted(PID != 0);
  analysis->set_parent_id(PID);

  G4int castBounces, machinedBounces, tpbBounces, mirrorBounces;
  G4int lg;
  G4String absorber;
  G4bool tpb;


  absorber = trackInfo->absorber();
  tpb = trackInfo->tpb();
  lg = trackInfo->lg_number();
  
  trackInfo->GetBounces(castBounces, machinedBounces, tpbBounces, mirrorBounces);
  trajectory->set_bounces(castBounces, machinedBounces, tpbBounces, mirrorBounces);
  analysis->set_bounces(castBounces, machinedBounces, tpbBounces, mirrorBounces);

  trajectory->set_fate(absorber);
  analysis->set_tpb(tpb); 
  analysis->set_fate(absorber);

  trajectory->set_lightguide_number(lg);
  analysis->set_lightguide_number(lg);

}
