#ifndef NedmLightGuide_h
#define NedmLightGuide_h 1

#include "globals.hh"
#include "NedmDetectorComponent.hh"
#include "G4PVPlacement.hh"

class NedmDetectorConstruction;
class G4LogicalVolume;

class NedmLightGuide : public NedmDetectorComponent, public G4PVPlacement {

  public:
    NedmLightGuide(G4RotationMatrix* pRot, const G4ThreeVector& tlate, G4LogicalVolume* pLog, const G4String& pName, G4LogicalVolume* pMother, G4int copy_no); 

    static void CreateLogicalVolumes();
    
    static void ConstructLightGuides(G4RotationMatrix* rotation, const G4ThreeVector& center, G4LogicalVolume* pMother, G4int copy_no);
    static void ConstructLightGuides(G4LogicalVolume* pMother);

    static const G4ThreeVector& lightguide_end_offset();

  private:
    static G4LogicalVolume* lightGuide_log[3];
    static G4LogicalVolume* lightguide_log[2000];

    static void ConstructStraightSurfaceVolumes(NedmLightGuide**);
  //    static void ConstructMirrorVolumes(NedmLightGuide**);
    static void ConstructCustomSurfaceVolumes(NedmLightGuide**);


};


#endif
