#ifndef NedmACRYLICWINDOW_h
#define NedmACRYLICWINDOW_h 1

#include "globals.hh"
#include "G4PVPlacement.hh"

#include "NedmDetectorComponent.hh"

class NedmDetectorConstruction;

class NedmAcrylicWindow : public NedmDetectorComponent, public G4PVPlacement {

  public:
    NedmAcrylicWindow(G4RotationMatrix* pRot, const G4ThreeVector& tlate, G4LogicalVolume* pMother, G4int copy_no);

  static void CreateLogicalVolumes();
  void ConstructSurfaceVolumes();

  private:
    static G4LogicalVolume* the_acrylicwindow_log_;
};

#endif
