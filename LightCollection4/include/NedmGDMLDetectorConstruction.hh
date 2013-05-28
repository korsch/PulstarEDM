#ifndef _GDMLDetectorConstruction_H_
#define _GDMLDetectorConstruction_H_

#include <G4VUserDetectorConstruction.hh>
#include <G4GDMLParser.hh>

class G4VPhysicalVolume;

class NedmGDMLDetectorConstruction : public G4VUserDetectorConstruction {
public:
  NedmGDMLDetectorConstruction(char* fname) {
    G4GDMLParser parser;
    parser.Read(fname);
    fWorld = parser.GetWorldVolume();
  }

  G4VPhysicalVolume* Construct() {
    return fWorld;
  }

private:
  G4VPhysicalVolume* fWorld;

};

#endif
