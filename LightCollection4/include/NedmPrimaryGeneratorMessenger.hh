#ifndef _NedmPrimaryGeneratorMessenger_H_
#define _NedmPrimaryGeneratorMessenger_H_

#include "G4UImessenger.hh"

#include "globals.hh"

class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;
class G4UIdirectory;
class G4UIcommand;

class NedmPrimaryGeneratorAction;

/*! @brief Allows control of event generation at a more granular level
*/

class NedmPrimaryGeneratorMessenger : public G4UImessenger {

public:
  NedmPrimaryGeneratorMessenger(NedmPrimaryGeneratorAction*);
  ~NedmPrimaryGeneratorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  NedmPrimaryGeneratorAction* generator;

  G4UIdirectory* generatorDir;
  //! /nedm/generator/random
  G4UIcmdWithABool* randomCmd;
  //! /nedm/generator/randomDirection
  G4UIcmdWithABool* randomDirectionCmd;
  //! /nedm/generator/minVector
  G4UIcmdWith3VectorAndUnit* minCmd;
  //! /nedm/generator/maxVector
  G4UIcmdWith3VectorAndUnit* maxCmd;

};

#endif
