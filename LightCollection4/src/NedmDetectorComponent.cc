#include "NedmDetectorComponent.hh"
#include "NedmDetectorConstruction.hh"

NedmDetectorConstruction* NedmDetectorComponent::the_detector_;

void NedmDetectorComponent::Initialize(NedmDetectorConstruction* det) {
  the_detector_ = det;
}
