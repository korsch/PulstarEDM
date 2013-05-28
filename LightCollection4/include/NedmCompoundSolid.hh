#ifndef NedmCompoundSolid_h
#define NedmCompoundSolid_h

#include <vector>

#include "globals.hh"

#include "G4TessellatedSolid.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "NedmDetectorComponent.hh"

class G4VPhysicalVolume;

//! Contains code to build up complex solids from slices.
namespace NedmCompoundSolid {

  //! Encapsulates information of a single slice of a solid.
  class Slice {

    public:
      Slice(const G4ThreeVector& center, //vector pointing to the center of the slice
          const G4ThreeVector& axis, //vector perpendicular to the face of the slice
          const G4TwoVector& dimensions, //x and y dimensions of the slice
          G4double twist=0); //absolute magnitude twist with respect to default orientation

      Slice(const Slice& slice);

      Slice() : center_(0), axis_(0), dimensions_(0), twist_(0) {}

      void ComputeVertices(G4ThreeVector& bottom_right, G4ThreeVector& top_right,
          G4ThreeVector& top_left, G4ThreeVector& bottom_left, G4double surface=0) const;

      const G4RotationMatrix ComputeAxisRotation() const;

      //Accessors
      inline const G4ThreeVector& center() const { return center_; }
      inline const G4ThreeVector& axis() const { return axis_; }
      inline const G4TwoVector& dimensions() const { return dimensions_; }
      inline const G4double twist() const { return twist_; }

    private:
      //! A vector pointing to the center of this slice
      G4ThreeVector center_;
      //! This normal to the surface of this slice
      G4ThreeVector axis_;
      //! The two dimensional size of this slice
      G4TwoVector dimensions_;
      //! How much this slice should be rotated about its axis.
      G4double twist_;

    friend std::istream& operator>>(std::istream& in, Slice& slice);
    friend std::ostream& operator<<(std::ostream& out, Slice& slice);
  };

  typedef std::vector<Slice> SliceVector;
  typedef SliceVector::const_iterator SliceIterator;

  //! The class which sould be used to build complex solids.
  class BentSolid : public G4TessellatedSolid, NedmDetectorComponent {

    public:
      BentSolid(const G4String& name) : G4TessellatedSolid(name),
        name_(name),
        surface_thickness_(0) {}
      
      //! Adds a slice to the list which define this solid.
      void AddSlice(const Slice& slice);

      //! Builds the solid object from the slice data and closes it.
      void Build();

      G4bool CheckIntersections(const G4TessellatedSolid* solid2,
          G4bool verbose=false) const;
      
      //! Creates a BentSolid object from slice data in the named file.
      static BentSolid* FromFile(const G4String& name,
          const G4String& fname);

      inline G4double surface_thickness() const { return surface_thickness_; }
      inline const Slice& final_slice() const { return slices_.back(); }

      inline void set_surface_thickness(G4double thick) { surface_thickness_ = thick; }

      void ConstructSurfaceVolumes(G4VPhysicalVolume*);

    private:
      SliceVector slices_;
      G4String name_;
      G4double surface_thickness_;

      void ConnectVertices(G4TessellatedSolid* solid,
          const G4ThreeVector& pBR, const G4ThreeVector& pTR,
          const G4ThreeVector& pTL, const G4ThreeVector& pBL,
          const G4ThreeVector& cBR, const G4ThreeVector& cTR,
          const G4ThreeVector& cTL, const G4ThreeVector& cBL,
          G4int direction=1);
  };
}

#endif
