#ifndef _NedmTrackInformation_H_
#define _NedmTrackInformation_H_

#include "G4VUserTrackInformation.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4LogicalSurface;

/*! @brief Keeps track of important parameters of a photon's history

  Data of interest about each photon is accumulated here and then passed to the analysis manager at the end of each event.
 */
class NedmTrackInformation : public G4VUserTrackInformation {
public:
  NedmTrackInformation() : G4VUserTrackInformation(), detected_(false),
			     cast_bounces_(0), machined_bounces_(0), tpb_bounces_(0),
                             mirror_bounces_(0),
			     tpb_(false), absorber_("other"), lg_number_(0),
			   pmt_number_(-1), time_detected_(0) , energy_detected_(0), position_detected_(0) {;}

  //! Set whether or not this photon has been detected
  void set_detected(G4bool det) { detected_ = det; }

  //! Predicate to check if photon as detected
  G4bool detected() { return detected_; }

  //! For each time a photon is reflected attempt to register a bounce
  void RegisterBounce(G4LogicalSurface*);

  //! Fetch the number of times this photon bounced off cast,
  //! machined, and tpb surfaces
  void GetBounces(G4int&, G4int&, G4int&, G4int&);

  //! Set the surface which absorbed this surface
  void WasAbsorbed(G4LogicalSurface*);

  //! Mark the photon as having escaped the detector
  void set_escaped() { absorber_ = "escaped"; }

  inline void set_pmt_number(G4int pmt_number) { pmt_number_ = pmt_number; }
  inline G4int pmt_number() { return pmt_number_; }

  //! Set the photon as having a possibility to interact w/ TPB
  inline void set_tpb() { tpb_ = true; }

  inline G4bool tpb() { return tpb_; }

  //! Mark this photon as having been killed by bulk attenuation
  inline void set_bulk() { absorber_ = "bulk"; }

  //! Get the fate of the photon
  G4String absorber();

  inline void set_lg_number(G4int lg_number) { lg_number_ = lg_number; }

  inline G4int lg_number() { return lg_number_; }

  inline
  void set_time_detected(G4double time) { time_detected_ = time; }

  inline
  void set_energy_detected(G4double energy) { energy_detected_ = energy; }

  inline
  void set_position_detected(G4ThreeVector position) { position_detected_ = position; }


  inline
  G4double time_detected() { return time_detected_; }

  inline
  G4double energy_detected() { return energy_detected_; }

  inline
  G4ThreeVector position_detected() { return position_detected_; }

  inline void Print() const {;}
private:
  G4bool detected_;
  G4int cast_bounces_;
  G4int machined_bounces_;
  G4int tpb_bounces_;
  G4int mirror_bounces_;
  G4bool tpb_;
  G4String absorber_;
  G4int lg_number_;
  G4int pmt_number_;
  G4double time_detected_;
  G4double energy_detected_;
  G4ThreeVector position_detected_;
  

};

#endif
