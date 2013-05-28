#include "NedmAnalysisManager.hh"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "G4LogicalBorderSurface.hh"
#include "G4String.hh"
#include "G4ios.hh"
#include "G4ThreeVector.hh"

#include "NedmAnalysisMessenger.hh"

NedmAnalysisManager* NedmAnalysisManager::the_manager_ = 0;

NedmAnalysisManager* NedmAnalysisManager::GetInstance()
{
  if(!the_manager_) {
    the_manager_ = new NedmAnalysisManager();
  }
  return the_manager_;
}

NedmAnalysisManager::NedmAnalysisManager() : root_file_name_("hist.root")
{
  messenger_ = new NedmAnalysisMessenger(this);
  vertex_x_ = 0;
  vertex_y_ = 0;
  vertex_z_ = 0;
  vertex_theta_ = 0;
  vertex_phi_ = 0;
  detected_ = 0;
  root_file_ = 0;
  cast_bounces_ = 0;
  machined_bounces_ = 0;
  tpb_bounces_ = 0;
  mirror_bounces_ = 0;
  fate_ = 0;
  tpb_ = false;
  lightguide_number_ = 0;
  pmt_number_ = 0;
  time_detected_ = 0;
  energy_detected_ = 0;
  x_detected_ = 0;
  y_detected_ = 0;
  z_detected_ = 0;
  parent_id_ = -1;
}



NedmAnalysisManager::~NedmAnalysisManager()
{
  if(root_file_) {
    FinalizeFile();
  }
  delete messenger_;
}



/*! All ROOT objects created after this call we be created in the directory of the open ROOT file. */
void NedmAnalysisManager::BookFile() {
  root_file_ = new TFile(root_file_name_, "RECREATE", "light guide transmission");
  root_file_->cd();
}

/*! @note This the rootfile will no longer be available after this call */
void NedmAnalysisManager::FinalizeFile() {
  root_file_->Close();
  delete root_file_;
}

/*! @arg run the run number */
void NedmAnalysisManager::BookTree(G4int run)
{
  data_tree_ = new TTree(TString::Format("t_lg%d",run),"lightguide transmission");
  data_tree_->Branch("vertex_x",&vertex_x_,"vertex_x/D");
  data_tree_->Branch("vertex_y",&vertex_y_,"vertex_y/D");
  data_tree_->Branch("vertex_z",&vertex_z_,"vertex_z/D");
  data_tree_->Branch("theta",&vertex_theta_,"theta/D");
  data_tree_->Branch("phi",&vertex_phi_,"phi/D");
  data_tree_->Branch("det",&detected_,"det/I");
  data_tree_->Branch("castBounces", &cast_bounces_, "castBounces/I");
  data_tree_->Branch("machinedBounces", &machined_bounces_, "machinedBounces/I");
  data_tree_->Branch("tpbBounces", &tpb_bounces_, "tpbBounces/I");
  data_tree_->Branch("mirrorBounces", &mirror_bounces_, "mirrorBounces/I");
  data_tree_->Branch("fate", &fate_, "fate/I");
  data_tree_->Branch("tpb", &tpb_, "tpb/I");
  data_tree_->Branch("lg", &lightguide_number_, "lg/I");
  data_tree_->Branch("pmtNo", &pmt_number_, "pmtNo/I");
  data_tree_->Branch("time", &time_detected_, "time/D");
  data_tree_->Branch("energy", &energy_detected_, "energy/D");
  data_tree_->Branch("final_x",&x_detected_,"final_x/D");
  data_tree_->Branch("final_y",&y_detected_,"final_y/D");
  data_tree_->Branch("final_z",&z_detected_,"final_z/D");
  data_tree_->Branch("parent_id", &parent_id_, "parent_id/I");
}

void NedmAnalysisManager::FillTree()
{
  data_tree_->Fill();
}


/*!
  Make sure root file is open and create tree by calling NedmAnalysisManager::BookTree()
*/
void NedmAnalysisManager::BeginOfRun(G4int run)
{
  if(!root_file_) BookFile();
  BookTree(run);
}

/*! Writes the data file to disk */
void NedmAnalysisManager::EndOfRun()
{
  data_tree_->Write();
}

/*! Encodes the final fate of the photon as an integer. The current scheme is as follows:
  @li 1 The photon was detected
  @li 2 The photon was killed by Bulk Absorption process
  @li 3 The photon was absorbed by a cast surface
  @li 4 The photon was absorbed by a machined face
  @li 5 The photon was absorbed by the plug
  @li 0 Other
 */
void NedmAnalysisManager::set_fate(G4String fate_arg) {
  if(fate_arg == "detected")
    fate_ = 1;
  else if (fate_arg == "bulk")
    fate_ = 2;
  else if (fate_arg == "cast")
    fate_ = 3;
  else if (fate_arg == "machined")
    fate_ = 4;
  else if (fate_arg == "plug" )
    fate_ = 5;
  else
    fate_ = 0;
}
