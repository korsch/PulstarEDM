#ifndef _NedmDetectorMessenger_H_
#define _NedmDetectorMessenger_H_

#include "G4UImessenger.hh"
#include "globals.hh"

class NedmDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcommand;

/*! @brief Allows the user to modify the geometry of the detector interactively.
*/
class NedmDetectorMessenger : public G4UImessenger {

public:
  NedmDetectorMessenger(NedmDetectorConstruction*);
  ~NedmDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  NedmDetectorConstruction* det;

  G4UIdirectory* detectorDir;
  //! /nedm/detector/halfLG_z
  G4UIcmdWithADoubleAndUnit* lgzCmd;
  //! /nedm/detector/halfLG_x
  G4UIcmdWithADoubleAndUnit* lgxCmd;
  //! /nedm/detector/update
  G4UIcommand* updateCmd;
  //! /nedm/detector/castLPB
  G4UIcmdWithADouble* castLpbCmd;
  //! /nedm/detector/machinedLPB
  G4UIcmdWithADouble* machinedLpbCmd;
  //! /nedm/detector/tpbLPB
  G4UIcmdWithADouble* tpbLpbCmd;
  //! /nedm/detector/bulkAbsorption
  G4UIcmdWithADoubleAndUnit* bulkCmd;
  //! /nedm/detector/torlonLPB
  G4UIcmdWithADouble* torlonLpbCmd;
  //! /nedm/detector/acrylicRIndex
  G4UIcmdWithADouble* indexCmd;
  //! /nedm/detector/mirror
  G4UIcmdWithABool* mirrorCmd;
  //! /nedm/detector/hole
  G4UIcmdWithABool* holeCmd;
  //! /nedm/detector/holeRadius
  G4UIcmdWithADoubleAndUnit* holeRadCmd;
  //! /nedm/detector/hole_x
  G4UIcmdWithADoubleAndUnit* holeXcmd;
  //! /nedm/detector/hole_z
  G4UIcmdWithADoubleAndUnit* holeZcmd;
  //! /nedm/detector/plug
  G4UIcmdWithABool* plugCmd;
  //! /nedm/detector/window
  G4UIcmdWithABool* windowCmd;
  //! /nedm/detector/fullPMTArea
  G4UIcmdWithABool* fullPMTCmd;
  //! /nedm/detector/PMTRadius
  G4UIcmdWithADoubleAndUnit* PMTRadiusCmd;
  //! /nedm/detector/mirrorReflectivity
  G4UIcmdWithADouble* mirrorReflCmd;
  //! /nedm/detector/mirrorType
  G4UIcmdWithAString* mirrorTypeCmd;
  //! /nedm/detector/torlonType
  G4UIcmdWithAString* torlonTypeCmd;
  //! /nedm/detector/singleAirgapWidth
  G4UIcmdWithADoubleAndUnit* sAirgapWidthCmd;
  //! /nedm/detector/mirrorAirgapWidth
  G4UIcmdWithADoubleAndUnit* mAirgapWidthCmd;
  //! /nedm/detector/lgGeometryFiles
  G4UIcommand* lightguideFilesCmd;
  //! /nedm/detector/PMTEfficiency
  G4UIcmdWithADouble* pmtEfficiencyCmd;
  //! /nedm/detector/lgLossToggle
  G4UIcmdWithABool* lgLossCmd;
};

#endif
