#ifndef _NedmTrajectory_H_
#define _NedmTrajectory_H_

#include "G4Trajectory.hh"
#include "G4String.hh"
#include "G4Allocator.hh"
#include "globals.hh"

#include <vector>

/*! @brief Keeps track of properties of each photon's history so that they can be used in visualization.

  The attributes defined within this class can be used to define the visualzation properties of photon trajectories interactively, via Geant4 macros, or within heprep. */
class NedmTrajectory : public G4Trajectory {
public:
  NedmTrajectory();
  NedmTrajectory(const G4Track* aTrack);
  NedmTrajectory(NedmTrajectory& right);

  virtual ~NedmTrajectory() {;}

  //! Mark this photon as having been detected
  inline
  void set_detected(G4bool det) { detected_ = det; }

  /*! @brief Set the final fate of this photon

    Options are:
    \li \c "detected" Photon was detected
    \li \c "bulk" Photon was killed by bulk absorption
    \li \c "cast" Photon was absorbed by a cast surface
    \li \c "machined" Photon was absorbed by a machined surface
    \li \c "escaped" Photon escaped the detector
    \li \c "plug" Photon absorbed by torlon plug
    \li \c "other" Other
  */
  inline
  void set_fate(G4String fate) { fate_ = fate; }

  //! Set the number of times the photon bounced on cast and machined surfaces
  inline
  void set_bounces(G4int, G4int, G4int, G4int);

  inline
  void set_lightguide_number(G4int lg) { lightguide_number_ = lg; }

  inline 
  void set_pmt_number(G4int copy_no) { pmt_number_ = copy_no; }

  inline
  void set_converted(G4bool converted) { converted_ = converted;}

  //! Define the attributes of this trajectory
  const std::map<G4String, G4AttDef>* GetAttDefs() const;
  //! Return the values of the trajectorie's attributes
  std::vector<G4AttValue>* CreateAttValues() const;

  inline void* operator new(size_t);
  inline void operator delete(void* aTrajectory);
  
private:
  G4bool detected_;
  G4int cast_bounces_;
  G4int machined_bounces_;
  G4int tpb_bounces_;
  G4int mirror_bounces_;
  G4String fate_;
  G4int lightguide_number_;
  G4int pmt_number_;
  G4bool converted_;
};

extern G4Allocator<NedmTrajectory> NedmTrajectoryAllocator;

 
inline void* NedmTrajectory::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)NedmTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}
 
inline void NedmTrajectory::operator delete(void* aTrajectory)
{
  NedmTrajectoryAllocator.FreeSingle((NedmTrajectory*)aTrajectory);
}

inline
void NedmTrajectory::set_bounces(G4int cast, G4int mach, G4int tpb, G4int mirror) {
  cast_bounces_ = cast;
  machined_bounces_ = mach;
  tpb_bounces_ = tpb;
  mirror_bounces_ = mirror;
}
#endif
