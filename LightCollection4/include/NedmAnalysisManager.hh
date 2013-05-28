#ifndef NedmAnalysisManager_h
#define NedmAnalysisManager_h 1

#include "string.h"

#include "TString.h"

#include "globals.hh"

class TFile;
class TTree;

class NedmAnalysisMessenger;

/*! @brief Saves simulation data to root files.
  
  This class is hooked into a number of user actions such that it can keep tracking the the number of photons detected when produced at varied positions.

@note  Note that NedmAnalysisManager is a singleton.
 */
class NedmAnalysisManager {

public:
  //! Returns the instance creating it if it doesn't exist
  static NedmAnalysisManager* GetInstance();

private:

  NedmAnalysisManager();

public:

  ~NedmAnalysisManager();

  //! Open file and create TTree to record data
  void BookTree(G4int);

  //! Prepare for a Run
  void BeginOfRun(G4int);

  //! Cleanup after a run.
  void EndOfRun();

  //! Simply calls the Fill method of the current TTree.
  void FillTree();

  //! Create the ROOT file for this program execution
  void BookFile();

  //! Save and close the ROOT file
  void FinalizeFile();
  
  //! Register propreties of particle generation
  inline
  void set_vertex_data(double, double, double, double, double);
  
  //! Set the number of bounces for the current photon
  inline
  void set_bounces(G4int, G4int, G4int, G4int);

  //! Sets the final fate of the current photon
  void set_fate(G4String);

  //! Set the number of detected photons for this event
  inline
  void set_detected(G4int detected) { detected_ = detected; }

  //! Record the copy number of the PMT which detected this photon
  inline
  void set_pmt_number(G4int pmt_number) { pmt_number_ = pmt_number; }

  //! Record whether this photon hit the TPB
  inline
  void set_tpb(G4bool tpb) { tpb_ = tpb; }

  //! Set the name of the ROOT file to write data to.
  //! Closes the currently open ROOT file and opens one named \a{fname}
  inline
  void set_root_file_name(TString root_file_name);

  //! Set the copy number of the lightguide which this photons first enters
  inline
  void set_lightguide_number(G4int lightguide_number);

  //! Set the time at which the photon was detected if any
  inline
  void set_time_detected(G4double time) { time_detected_ = time; }

  //! Set the energy at which the photon was detected if any
  inline
  void set_energy_detected(G4double energy) { energy_detected_ = energy; }

  inline
  void set_x_detected(G4double x) { x_detected_ = x; }

  inline
  void set_y_detected(G4double y) { y_detected_ = y; }

  inline
  void set_z_detected(G4double z) { z_detected_ = z; }



  //! Set the parent id of the detected particle
  inline
  void set_parent_id(G4int id) { parent_id_ = id; }

private:
  NedmAnalysisMessenger* messenger_;

  static NedmAnalysisManager* the_manager_;

  TString root_file_name_;
  TFile *root_file_;
  TTree *data_tree_;

  //! @name Vertex information
  //! Position and momementum direction information of primary event
  //@{
  G4double vertex_x_;
  G4double vertex_y_;
  G4double vertex_z_;
  G4double vertex_theta_;
  G4double vertex_phi_;
  //@}

  G4int detected_;
  G4int cast_bounces_;
  G4int machined_bounces_;
  G4int tpb_bounces_;
  G4int mirror_bounces_;
  G4int fate_;
  G4bool tpb_;
  G4int lightguide_number_;
  G4int pmt_number_;
  G4double time_detected_;
  G4double energy_detected_;
  G4double x_detected_;
  G4double y_detected_;
  G4double z_detected_;
  G4int parent_id_;
  
};

//Inline method definitions below here
//
inline
void NedmAnalysisManager::set_root_file_name(TString root_file_name) {
  if(root_file_) {
    FinalizeFile();
  }
  root_file_name_ = root_file_name;
  BookFile();
}

inline
void NedmAnalysisManager::set_lightguide_number(G4int lightguide_number) {
  lightguide_number_ = lightguide_number;
}

/*!
  @param x1 x position of particle generation
  @param y1 y position
  @param z1 z position
  @param theta1 Polar angle of particle momentum
  @param phi1 Azimuthal angle of particle momentum
*/
inline
void NedmAnalysisManager::set_vertex_data(double x1, double y1, double z1,
					   double theta1, double phi1)
{
  vertex_x_ = x1;
  vertex_y_ = y1;
  vertex_z_ = z1;
  vertex_theta_ = theta1;
  vertex_phi_ = phi1;
}

/*!
  @arg cast The number of bounces on cast surfaces
  @arg mach The number of bounces on machined surfaces
*/
inline
void NedmAnalysisManager::set_bounces(G4int cast, G4int mach, G4int tpb, G4int mirror) {
  cast_bounces_ = cast;
  machined_bounces_ = mach;
  tpb_bounces_ = tpb;
  mirror_bounces_ = mirror;
}
#endif

