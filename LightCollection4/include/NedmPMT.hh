#ifndef NedmPMT_h
#define NedmPMT_h 1

#include "globals.hh"
#include "G4PVPlacement.hh"

#include "NedmDetectorComponent.hh"

class NedmDetectorConstruction;

class NedmPMT : public NedmDetectorComponent, public G4PVPlacement {

  public:
    NedmPMT(G4RotationMatrix* pRot, const G4ThreeVector& tlate, G4LogicalVolume* pMother, G4int copy_no);

    static void CreateLogicalVolumes();

  private:
    static G4LogicalVolume* the_pmt_log_;
};

#endif
