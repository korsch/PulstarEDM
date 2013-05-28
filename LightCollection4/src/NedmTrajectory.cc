#include "NedmTrajectory.hh"

#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"

G4Allocator<NedmTrajectory> NedmTrajectoryAllocator;

NedmTrajectory::NedmTrajectory() :
  G4Trajectory(), detected_(false),
  cast_bounces_(0), machined_bounces_(0), tpb_bounces_(0), mirror_bounces_(0),
  fate_("other"), lightguide_number_(0), pmt_number_(0), converted_(false) {;}

NedmTrajectory::NedmTrajectory(const G4Track* aTrack) :
  G4Trajectory(aTrack), detected_(false),
  cast_bounces_(0), machined_bounces_(0), tpb_bounces_(0), mirror_bounces_(0),
  fate_("other"), lightguide_number_(0), pmt_number_(0), converted_(false) {;}

NedmTrajectory::NedmTrajectory(NedmTrajectory& right) :
  G4Trajectory(right), detected_(right.detected_),
  cast_bounces_(right.cast_bounces_), machined_bounces_(right.machined_bounces_),
  tpb_bounces_(right.tpb_bounces_), mirror_bounces_(right.mirror_bounces_),
  fate_(right.fate_), lightguide_number_(right.lightguide_number_),
  pmt_number_(right.pmt_number_), converted_(right.converted_) {;}

const std::map<G4String, G4AttDef>* NedmTrajectory::GetAttDefs() const {
  G4bool isNew;
  std::map<G4String,G4AttDef>* store
    = G4AttDefStore::GetInstance("NedmTrajectory",isNew);
  if (isNew) {
    
    G4String Det("Det");
    (*store)[Det] = G4AttDef(Det, "Whether particle was detected by the PMT",
			     "Bookkeeping", "", "G4bool");

    G4String Cast("Cast");
    (*store)[Cast] = G4AttDef(Cast, "Number of bounces on a cast surface",
			      "Bookkeeping", "", "G4int");

    G4String Mach("Mach");
    (*store)[Mach] = G4AttDef(Mach, "Number of bounces on a machined surface",
			      "Bookkeeping", "", "G4int");

    G4String TPB("TPB");
    (*store)[TPB] = G4AttDef(TPB, "Number of bounces on a tpb surface",
			      "Bookkeeping", "", "G4int");

    G4String Mirror("Mirror");
    (*store)[Mirror] = G4AttDef(Mirror, "Number of bounces on a mirror surface",
			      "Bookkeeping", "", "G4int");

    G4String Fate("Fate");
    (*store)[Fate] = G4AttDef(Fate, "Final fate of photon.",
			      "Bookkeeping", "", "G4String");

    G4String LG("LG");
    (*store)[LG] = G4AttDef(LG, "Whether photon went into light guide",
			      "Bookkeeping", "", "G4int");

    G4String PMTno("PMTno");
    (*store)[PMTno] = G4AttDef(PMTno, "Copy number of the PMT which detecte this photon",
        "Bookkeeping", "", "G4int");
    
    G4String Conv("Conv");
    (*store)[Conv] = G4AttDef(Conv, "Whether particle converted in the TPB",
			     "Bookkeeping", "", "G4bool");

  }
  return store;

}

std::vector<G4AttValue>* NedmTrajectory::CreateAttValues() const {
  std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

  values->push_back
    (G4AttValue("Det", G4UIcommand::ConvertToString(detected_), ""));
  values->push_back
    (G4AttValue("Cast", G4UIcommand::ConvertToString(cast_bounces_), ""));
  values->push_back
    (G4AttValue("Mach", G4UIcommand::ConvertToString(machined_bounces_), ""));
  values->push_back
    (G4AttValue("TPB", G4UIcommand::ConvertToString(tpb_bounces_), ""));
  values->push_back
    (G4AttValue("Mirror", G4UIcommand::ConvertToString(mirror_bounces_), ""));
  values->push_back
    (G4AttValue("Fate", fate_, ""));
  values->push_back
    (G4AttValue("LG", G4UIcommand::ConvertToString(lightguide_number_), ""));
  values->push_back
    (G4AttValue("PMTno", G4UIcommand::ConvertToString(pmt_number_), ""));
  values->push_back
    (G4AttValue("Conv", G4UIcommand::ConvertToString(converted_), ""));

  return values;
		    
}

