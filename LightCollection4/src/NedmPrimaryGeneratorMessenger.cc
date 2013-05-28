#include "NedmPrimaryGeneratorMessenger.hh"
#include "NedmPrimaryGeneratorAction.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIdirectory.hh"


NedmPrimaryGeneratorMessenger::NedmPrimaryGeneratorMessenger(NedmPrimaryGeneratorAction* gen) {

  generator = gen;
  generatorDir = new G4UIdirectory("/nedm/generator/");

  randomCmd = new G4UIcmdWithABool("/nedm/generator/random", this);
  randomCmd->SetGuidance("Switch on or off random particle position");
  randomCmd->SetParameterName("on_off", false);

  randomDirectionCmd = new G4UIcmdWithABool("/nedm/generator/randomDirection", this);
  randomDirectionCmd->SetGuidance("Switch on or off random particle momentum direction");
  randomDirectionCmd->SetParameterName("on_off", false);

  minCmd = new G4UIcmdWith3VectorAndUnit("/nedm/generator/minVector",
      this);
  minCmd->SetGuidance("Set the minimum corner of the random generation area.");
  minCmd->SetParameterName("min_x", "min_y", "min_z", false);
  minCmd->SetDefaultUnit("cm");

  maxCmd = new G4UIcmdWith3VectorAndUnit("/nedm/generator/maxVector",
      this);
  maxCmd->SetGuidance("Set the maximum corner of the random generation area.");
  maxCmd->SetParameterName("max_x", "max_y", "max_z", false);
  maxCmd->SetDefaultUnit("cm");

}


NedmPrimaryGeneratorMessenger::~NedmPrimaryGeneratorMessenger() {
  delete generatorDir;
  delete randomCmd;
  delete randomDirectionCmd;
  delete minCmd;
  delete maxCmd;
}

void NedmPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* cmd,
                                                  G4String str) {

  if(cmd == randomCmd)
    generator->set_random_position(randomCmd->GetNewBoolValue(str));
  else if(cmd == randomDirectionCmd)
    generator->set_random_direction(randomDirectionCmd->GetNewBoolValue(str));
  else if (cmd == minCmd)
    generator->set_min_random_corner(minCmd->GetNew3VectorValue(str));
  else if (cmd == maxCmd)
    generator->set_max_random_corner(maxCmd->GetNew3VectorValue(str));
}
