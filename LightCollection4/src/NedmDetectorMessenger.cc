#include "NedmDetectorMessenger.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4Tokenizer.hh"

#include "NedmDetectorConstruction.hh"
#include "NedmDetectorParameters.hh"

NedmDetectorMessenger::NedmDetectorMessenger(NedmDetectorConstruction* gdc) : det(gdc) {
  detectorDir = new G4UIdirectory("/nedm/detector/");
  detectorDir->SetGuidance("Adjustment of detector geometry.");

  lgzCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/halfLG_z", this);
  lgzCmd->SetGuidance("Set 0.5 times the length of the light guide.");
  lgzCmd->SetParameterName("lg_z", false);
  lgzCmd->SetDefaultUnit("cm");

  lgxCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/halfLG_x", this);
  lgxCmd->SetGuidance("Set the half width of the light guide.");
  lgxCmd->SetParameterName("lg_x", false);
  lgxCmd->SetDefaultUnit("cm");

  updateCmd = new G4UIcommand("/nedm/detector/update", this);
  updateCmd->SetGuidance("Rebuilds the geometry after it has changed.");
  updateCmd->SetGuidance("Must be called after an update.");

  castLpbCmd = new G4UIcmdWithADouble("/nedm/detector/castLPB", this);
  castLpbCmd->SetGuidance("Set the loss per bounce on the cast surface");
  castLpbCmd->SetParameterName("LPB", false);

  machinedLpbCmd = new G4UIcmdWithADouble("/nedm/detector/machinedLPB", this);
  machinedLpbCmd->SetGuidance("Set the loss per bounce on the machined surface");
  machinedLpbCmd->SetParameterName("LPB", false);

  tpbLpbCmd = new G4UIcmdWithADouble("/nedm/detector/tpbLPB", this);
  tpbLpbCmd->SetGuidance("Set the loss per bounce on the tpb surface");
  tpbLpbCmd->SetParameterName("LPB", false);
  
  bulkCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/bulkAbsorption", this);
  bulkCmd->SetGuidance("Set the bulk attenuation length of the acrylic.");
  bulkCmd->SetParameterName("length", false);
  bulkCmd->SetDefaultUnit("cm");

  torlonLpbCmd = new G4UIcmdWithADouble("/nedm/detector/torlonLPB", this);
  torlonLpbCmd->SetGuidance("Set the loss per bounce on the torlon surface");
  torlonLpbCmd->SetParameterName("LPB", false);

  mirrorCmd = new G4UIcmdWithABool("/nedm/detector/mirror", this);
  mirrorCmd->SetGuidance("Turn the mirror on or off in the geometry");
  mirrorCmd->SetParameterName("on_off", false);

  indexCmd = new G4UIcmdWithADouble("/nedm/detector/acrylicRIndex", this);
  indexCmd->SetGuidance("Set the refractive index of the acrylic.");
  indexCmd->SetParameterName("index", false);

  holeCmd = new G4UIcmdWithABool("/nedm/detector/hole", this);
  holeCmd->SetGuidance("Turn the hole in the cell side on or off");
  holeCmd->SetParameterName("on_off", false);

  windowCmd = new G4UIcmdWithABool("/nedm/detector/window", this);
  windowCmd->SetGuidance("Turn the quartz window on or off in the geometry");
  windowCmd->SetParameterName("on_off", false);
  
  fullPMTCmd = new G4UIcmdWithABool("/nedm/detector/fullPMTArea", this);
  fullPMTCmd->SetGuidance("Set whether the full area of the PMT is avaible for detection or only the surface area of the light guide.");
  fullPMTCmd->SetParameterName("on_off", false);

  holeXcmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/hole_x", this);
  holeXcmd->SetGuidance("Set the x position of the hole in the cell side.");
  holeXcmd->SetParameterName("pos_x", false);
  holeXcmd->SetDefaultUnit("cm");

  PMTRadiusCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/PMTRadius", this);
  PMTRadiusCmd->SetGuidance("Set the radius of the PMT. This has no effect unless fullPMTArea is set.");
  PMTRadiusCmd->SetParameterName("radius", false);
  PMTRadiusCmd->SetDefaultUnit("cm");

  holeZcmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/hole_z", this);
  holeZcmd->SetGuidance("Set the z position of the hole in the cell side.");
  holeZcmd->SetParameterName("pos_z", false);
  holeZcmd->SetDefaultUnit("cm");

  holeRadCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/holeRadius", this);
  holeRadCmd->SetGuidance("Set the radius of the whole in the cell side.");
  holeRadCmd->SetParameterName("radius", false);
  holeRadCmd->SetDefaultUnit("cm");

  plugCmd = new G4UIcmdWithABool("/nedm/detector/plug", this);
  plugCmd->SetGuidance("Turn on or off the plug in the hole in the cell side.");
  plugCmd->SetParameterName("on_off", false);

  mirrorReflCmd = new G4UIcmdWithADouble("/nedm/detector/mirrorReflectivity", this);
  mirrorReflCmd->SetGuidance("Set the reflectivity of the rear mirror.");
  mirrorReflCmd->SetParameterName("reflectivity", false);

  mirrorTypeCmd = new G4UIcmdWithAString("/nedm/detector/mirrorType", this);
  mirrorTypeCmd->SetGuidance("Set the type of the rear mirror.");
  mirrorTypeCmd->SetParameterName("type", false);
  mirrorTypeCmd->SetCandidates("diffuse specular");

  torlonTypeCmd = new G4UIcmdWithAString("/nedm/detector/torlonType", this);
  torlonTypeCmd->SetGuidance("Set the surface type of the torlon.");
  torlonTypeCmd->SetParameterName("type", false);
  torlonTypeCmd->SetCandidates("diffuse specular");

  sAirgapWidthCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/singleAirgapWidth", this);
  sAirgapWidthCmd->SetGuidance("Set the width of the airgap between the lightguide and PMT when no window is present.");
  sAirgapWidthCmd->SetParameterName("width", false);
  sAirgapWidthCmd->SetDefaultUnit("cm");

  mAirgapWidthCmd = new G4UIcmdWithADoubleAndUnit("/nedm/detector/mirrorAirgapWidth", this);
  mAirgapWidthCmd->SetGuidance("Set the width of the airgap between the cell side and mirror.");
  mAirgapWidthCmd->SetParameterName("width", false);
  mAirgapWidthCmd->SetDefaultUnit("cm");

  lightguideFilesCmd = new G4UIcommand("/nedm/detector/lgGeometryFiles", this);
  lightguideFilesCmd->SetGuidance("Set the slice files to build the lightguides from. Setting a file to \"\" will cause a default straight guide to be used.");
  lightguideFilesCmd->SetGuidance("[usage] /nedm/detector/lgGeometryFiles left center right");
  G4UIparameter* param;
  param = new G4UIparameter("left", 's', false);
  lightguideFilesCmd->SetParameter(param);
  param = new G4UIparameter("center", 's', false);
  lightguideFilesCmd->SetParameter(param);
  param = new G4UIparameter("right", 's', false);
  lightguideFilesCmd->SetParameter(param);

  pmtEfficiencyCmd = new G4UIcmdWithADouble("/nedm/detector/PMTEfficiency", this);
  pmtEfficiencyCmd->SetGuidance("Set the detection efficiency of the PMT");
  pmtEfficiencyCmd->SetParameterName("efficiency", false);

  lgLossCmd = new G4UIcmdWithABool("/nedm/detector/lgLossToggle", this);
  lgLossCmd->SetGuidance("Turn on or off the surface losses on the lightguides");
  lgLossCmd->SetParameterName("on_off", false);
}

NedmDetectorMessenger::~NedmDetectorMessenger() {
  delete detectorDir;
  delete lgzCmd;
  delete lgxCmd;
  delete updateCmd;
  delete machinedLpbCmd;
  delete castLpbCmd;
  delete tpbLpbCmd;
  delete bulkCmd;
  delete torlonLpbCmd;
  delete mirrorCmd;
  delete indexCmd;
  delete holeCmd;
  delete holeXcmd;
  delete holeZcmd;
  delete holeRadCmd;
  delete plugCmd;
  delete windowCmd;
  delete fullPMTCmd;
  delete PMTRadiusCmd;
  delete mirrorReflCmd;
  delete mirrorTypeCmd;
  delete torlonTypeCmd;
  delete sAirgapWidthCmd;
  delete sAirgapWidthCmd;
  delete lightguideFilesCmd;
  delete pmtEfficiencyCmd;
  delete lgLossCmd;
}

void NedmDetectorMessenger::SetNewValue(G4UIcommand* cmd, G4String str) {
  NedmDetectorParameters* parameters = NedmDetectorParameters::instance();

  if(cmd == lgzCmd) {
    //    parameters->set_lightguide_z(lgzCmd->GetNewDoubleValue(str));
  } else if (cmd == lgxCmd) {
    //    parameters->set_lightguide_x(lgxCmd->GetNewDoubleValue(str));
  } else if (cmd == updateCmd) {
    det->UpdateGeometry();
  } else if (cmd == castLpbCmd) {
    det->set_cast_loss(castLpbCmd->GetNewDoubleValue(str));
  } else if (cmd == machinedLpbCmd) {
    det->set_machined_loss(machinedLpbCmd->GetNewDoubleValue(str));
  } else if (cmd == tpbLpbCmd) {
    det->set_tpb_loss(tpbLpbCmd->GetNewDoubleValue(str));
  } else if (cmd == bulkCmd) {
    det->set_bulk_attenuation(bulkCmd->GetNewDoubleValue(str));
  } else if (cmd == torlonLpbCmd) {
    det->set_torlon_loss(torlonLpbCmd->GetNewDoubleValue(str));
  } else if (cmd == mirrorCmd) {
    parameters->set_use_mirror(mirrorCmd->GetNewBoolValue(str));
  } else if (cmd == indexCmd) {
    det->set_acrylic_index(indexCmd->GetNewDoubleValue(str));
  } else if (cmd == holeCmd) {
    parameters->set_use_hole(holeCmd->GetNewBoolValue(str));
  } else if (cmd == holeXcmd) {
    parameters->set_hole_x_position(holeXcmd->GetNewDoubleValue(str));
  } else if (cmd == holeZcmd) {
    parameters->set_hole_z_position(holeZcmd->GetNewDoubleValue(str));
  } else if (cmd == holeRadCmd) {
    parameters->set_hole_radius(holeRadCmd->GetNewDoubleValue(str));
  } else if (cmd == plugCmd) {
    parameters->set_use_plug(plugCmd->GetNewBoolValue(str));
  } else if (cmd == windowCmd) {
    parameters->set_use_window(windowCmd->GetNewBoolValue(str));
  } else if (cmd == fullPMTCmd) {
    parameters->set_use_full_pmt(fullPMTCmd->GetNewBoolValue(str));
  } else if (cmd == PMTRadiusCmd) {
    parameters->set_pmt_radius(PMTRadiusCmd->GetNewDoubleValue(str));
  } else if (cmd == mirrorReflCmd) {
    det->set_mirror_reflectivity(mirrorReflCmd->GetNewDoubleValue(str));
  } else if (cmd == mirrorTypeCmd) {
    det->set_mirror_type(str);
  } else if (cmd == torlonTypeCmd) {
    det->set_torlon_type(str);
  } else if (cmd == sAirgapWidthCmd) {
    parameters->set_single_airgap(sAirgapWidthCmd->GetNewDoubleValue(str));
  } else if (cmd == mAirgapWidthCmd) {
    parameters->set_mirror_airgap(mAirgapWidthCmd->GetNewDoubleValue(str));
  } else if (cmd == lightguideFilesCmd) {
    G4Tokenizer next(str);

    G4String left = next();
    G4String center = next();
    G4String right = next();
    parameters->set_lightguide_files(left, center, right);
  } else if (cmd == pmtEfficiencyCmd) {
    det->set_pmt_efficiency(pmtEfficiencyCmd->GetNewDoubleValue(str)); 
  } else if (cmd == lgLossCmd) {
    parameters->set_lightguide_loss(lgLossCmd->GetNewBoolValue(str));
  }

}
