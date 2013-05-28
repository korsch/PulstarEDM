#ifndef NedmDetectorComponent_h
#define NedmDetectorComponent_h 1

#include "globals.hh"

class NedmDetectorConstruction;

class NedmDetectorComponent {

  public:
    static void Initialize(NedmDetectorConstruction*);

  protected:
    static NedmDetectorConstruction* the_detector_;
};

#endif
