#ifndef NedmCellSide_h
#define NedmCellSide_h 1

#include "globals.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"

#include "NedmDetectorComponent.hh"

class G4LogicalVolume;
class G4VSolid;
class NedmDetectorConstruction;

class NedmCellSide : public NedmDetectorComponent, public G4PVPlacement {

  public:
    NedmCellSide(G4RotationMatrix* pRot,
		 const G4ThreeVector& tlate,
		 const G4String whichside,	 
		 G4LogicalVolume* pMotherLogical,
		 G4int copyNo,
		 G4bool lrtbfb=false,
		 G4bool lrfb=false,
		 G4bool hole=false);

  static void CreateLogicalVolumes();
  inline int sign(double x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
  }

  private:
  static G4LogicalVolume* ConstructLogicalVolume(G4bool side1=false);
  static G4LogicalVolume* ConstructLogicalVolume(G4bool side1=false, G4String whichside="none");
  static G4LogicalVolume* ConstructMirrorLogicalVolume();
  static G4VSolid* MakeHole(G4VSolid*, G4String, G4String, G4VSolid**);
  
  static G4LogicalVolume* the_tb_side_withhole_;
  static G4LogicalVolume* the_tb_side_;
  static G4LogicalVolume* the_fb_side_;
  static G4LogicalVolume* the_lr_side_;
  static G4LogicalVolume* the_plug_;
  static G4LogicalVolume* the_mirror_;
  
  G4VPhysicalVolume* ConstructTPBInterface(G4bool side1);
  G4VPhysicalVolume* ConstructTPBInterface(G4bool side1=false, G4String whichside="none", const G4ThreeVector tlate=G4ThreeVector(0,0,0));
  void ConstructSurfaceVolumes(G4bool side1);
  void ConstructSurfaceVolumes(G4bool side1=false,G4String whichside="none", const G4ThreeVector tlate=G4ThreeVector(0,0,0));
  G4VPhysicalVolume* ConstructMirror();
  G4VPhysicalVolume* ConstructRearSurface();
  
};

#endif
