#include "NedmCompoundSolid.hh"

#include <iostream>
#include <fstream>
#include <sstream>

#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4GeometryTolerance.hh"

#include "NedmDetectorParameters.hh"
#include "NedmDetectorConstruction.hh"

namespace NedmCompoundSolid {
  Slice::Slice(const G4ThreeVector& center, 
      const G4ThreeVector& axis,
      const G4TwoVector& dimensions, 
      G4double twist)
    : center_(center),
    axis_(axis.unit()),
    dimensions_(dimensions),
    twist_(twist) {}

  Slice::Slice(const Slice& slice) 
    : center_(slice.center_),
    axis_(slice.axis_),
    dimensions_(slice.dimensions_),
    twist_(slice.twist_) {}

  //! This builds a rotation which will align our axis in the correct
  //! direction while leaving vertical orientation alone. The quantities
  //! used were obtained by creating a generic matrix rotating z-hat into
  //! z-hat' and giving y-hat' no x component and then applying the constraints
  //! of a rotation to solve for the remaining components.
  const G4RotationMatrix Slice::ComputeAxisRotation() const {
    G4double z1 = axis_.x();
    G4double z2 = axis_.y();
    G4double z3 = axis_.z();

    G4double norm_z2_z3 = std::sqrt(std::pow(z2, 2.) + std::pow(z3, 2.));

    G4ThreeVector new_x(norm_z2_z3, -z1 * z2 / norm_z2_z3, -z1 * z3 / norm_z2_z3);
    G4ThreeVector new_y(0, z3 / norm_z2_z3, -z2 / norm_z2_z3);

    return G4RotationMatrix(new_x, new_y, axis_); 

  }

  void Slice::ComputeVertices(G4ThreeVector& bottom_right,
      G4ThreeVector& top_right,
      G4ThreeVector& top_left,
      G4ThreeVector& bottom_left,
      G4double surface) const {
    G4double x = dimensions_.x();
    G4double y = dimensions_.y();

    x -= surface;
    y -= surface;

    //Lay out corners of rectangle in x-y plane.
    bottom_right.set(x, -y, 0);
    top_right.set(x, y, 0);
    top_left.set(-x, y, 0);
    bottom_left.set(-x, -y, 0);

    //Twist the slice in the plane by the requested amount
    bottom_right.rotateZ(twist_);
    top_right.rotateZ(twist_);
    top_left.rotateZ(twist_);
    bottom_left.rotateZ(twist_);

    //Rotate plane so that its axis aligns with the specified axis
    //We assume that the x-axis is up, and that axis changes do not change
    //up-down orientation.

    const G4RotationMatrix rotation = ComputeAxisRotation();

    bottom_right *= rotation;
    top_right *= rotation;
    top_left *= rotation;
    bottom_left *= rotation;

    bottom_right += center_;
    top_right += center_;
    top_left += center_;
    bottom_left += center_;

  }

  std::istream& operator>>(std::istream& in, Slice& slice) {
    in >> slice.center_ >> slice.axis_ >> slice.dimensions_ >> slice.twist_;

    slice.center_ *= cm;
    slice.axis_ = slice.axis_.unit();
    slice.dimensions_ *= cm;
    slice.twist_ *= deg;

    return in;
  }

  std::ostream& operator<<(std::ostream& out, Slice& slice) {
    out << slice.center_ / cm << " " << slice.axis_ << " "
      << slice.dimensions_ / cm << " " << slice.twist_ / deg << std::endl;

    return out;
  }



  void BentSolid::AddSlice(const Slice& slice) {
    slices_.push_back(slice);
  }

  void BentSolid::Build() {
    Slice previous = slices_.front();

    //BR - Bottom Right
    //TR - Top Right
    //etc.
    //p - previous, c - current
    G4ThreeVector pBR, pTR, pTL, pBL,
                  cBR, cTR, cTL, cBL;
    
    //Create the first endplate for the solid.
    previous.ComputeVertices(pBR, pTR, pTL, pBL, surface_thickness_);

    G4QuadrangularFacet* end1 = new G4QuadrangularFacet(pBL, pTL, pTR, pBR, ABSOLUTE);
    AddFacet((G4VFacet*) end1);

    for(SliceIterator it = ++(slices_.begin());
        it < slices_.end();
        it++) {

      const Slice& current = *it;
      current.ComputeVertices(cBR, cTR, cTL, cBL, surface_thickness_);

      ConnectVertices(this,
          pBR, pTR, pTL, pBL,
          cBR, cTR, cTL, cBL);


      pBR = cBR;
      pTR = cTR;
      pTL = cTL;
      pBL = cBL;
    }

    //Create closing endcap
    G4QuadrangularFacet* end2 = new G4QuadrangularFacet(pBL, pBR, pTR, pTL, ABSOLUTE);
    AddFacet((G4VFacet*) end2);

    SetSolidClosed(true);
  }

  BentSolid* BentSolid::FromFile(const G4String& name,
      const G4String& fname) {
    std::ifstream data_file;

    BentSolid* solid = NULL;
    data_file.open(fname.data());
    if(data_file.is_open()) {
      
      solid = new BentSolid(name);
      Slice slice;
      std::string line;


      while(getline(data_file, line)) {
        std::stringstream line_stream(line);
        line_stream >> slice;

        solid->AddSlice(slice);
      }

      data_file.close();

    }

    return solid;

  }

  //Determines whether any vertices of surface volumes are inside the solid they are
  //associated with.
  G4bool BentSolid::CheckIntersections(const G4TessellatedSolid* solid2, G4bool verbose) const {
    G4double tolerance = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance();
    G4bool intersection = false;

    G4int n = solid2->GetNumberOfFacets();
    G4int n_this = GetNumberOfFacets();

    //Check every vertex of the given solid
    for(int i = 0; i < n; i++) {

      G4VFacet* facet = solid2->GetFacet(i);
      G4int m = facet->GetNumberOfVertices();

      for(int j = 0; j < m; j++) {

        G4ThreeVector p = facet->GetVertex(j);

        //Check that no vertex is inside our solid
        if(Inside(p)) {
          //If this point is "inside" p make sure that it doesn't
          //just share a vertex with with it.
          G4bool common_point = false;

          for(int k = 0; k < n_this; k++) {
            G4VFacet* this_facet = GetFacet(k);
            G4int m_this = this_facet->GetNumberOfVertices();

            int l;
            for(l = 0; l < m_this; l++) {
              G4ThreeVector q = this_facet->GetVertex(l);

              if((p - q).mag() <= tolerance) {
                //Our "intersection" is actually just our two volumes abutting
                //ignore this.
                common_point = true;
                break;
              }
            }

            if(l < m_this) break;
          }

          if(common_point) continue;
              

          if(!intersection) intersection = true;

          if(verbose) {
            G4cout << "#######WARNING#######" << G4endl;
            G4cout << "Intersection! BentSolid: " << name_ << G4endl;
            G4cout << "Intersection with: " << solid2->GetName() << G4endl;
            G4cout << "Facet: " << i << " Vertex: " << j << G4endl;
            G4cout << "Coordinates: " << p << G4endl;
          }

          continue;
        }

      }
    }

    return intersection;

  }

  void BentSolid::ConstructSurfaceVolumes(G4VPhysicalVolume* phys) {
    G4double safety_gap = G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / 2.;
    G4TessellatedSolid *bottom = new BentSolid(name_ + "_bottom_surface");
    G4TessellatedSolid *top = new BentSolid(name_ + "_top_surface");
    G4TessellatedSolid *left = new BentSolid(name_ + "_left_surface");
    G4TessellatedSolid *right = new BentSolid(name_ + "_right_surface");

    Slice previous = slices_.front();

    //BR - Bottom Right
    //TR - Top Right
    //etc.
    //p - previous, c - current, t - trimmed, r - raw
    G4ThreeVector rpBR, rpTR, rpTL, rpBL,
                  rcBR, rcTR, rcTL, rcBL,
                  tpBR, tpTR, tpTL, tpBL,
                  tcBR, tcTR, tcTL, tcBL;

    previous.ComputeVertices(tpBR, tpTR, tpTL, tpBL, surface_thickness_);
    previous.ComputeVertices(rpBR, rpTR, rpTL, rpBL);
    tpBR += (rpBR - tpBR).unit()*safety_gap;
    tpTR += (rpTR - tpTR).unit()*safety_gap;
    tpTL += (rpTL - tpTL).unit()*safety_gap;
    tpBL += (rpBL - tpBL).unit()*safety_gap;

    G4double left_right_mag = (rpBL - rpBR).mag();
    G4double top_bottom_mag = (rpBL - rpTL).mag();

    G4QuadrangularFacet* end;

    end = new G4QuadrangularFacet(tpBL, tpBR, rpBR, rpBL, ABSOLUTE);
    bottom->AddFacet((G4VFacet*) end);

    end = new G4QuadrangularFacet(tpTL, tpBL, rpBL, rpTL, ABSOLUTE);
    left->AddFacet((G4VFacet*) end);

    end = new G4QuadrangularFacet(tpTR, tpTL, rpTL, rpTR, ABSOLUTE);
    top->AddFacet((G4VFacet*) end);

    end = new G4QuadrangularFacet(tpBR, tpTR, rpTR, rpBR, ABSOLUTE);
    right->AddFacet((G4VFacet*) end);



    for(SliceIterator it = ++(slices_.begin());
        it < slices_.end();
        it++) {

      const Slice& current = *it;
      current.ComputeVertices(tcBR, tcTR, tcTL, tcBL, surface_thickness_);
      current.ComputeVertices(rcBR, rcTR, rcTL, rcBL);
      tcBR += (rcBR - tcBR).unit()*safety_gap;
      tcTR += (rcTR - tcTR).unit()*safety_gap;
      tcTL += (rcTL - tcTL).unit()*safety_gap;
      tcBL += (rcBL - tcBL).unit()*safety_gap;

      ConnectVertices(bottom,
          rpBR, rpBL, tpBL, tpBR,
          rcBR, rcBL, tcBL, tcBR,
          -1);

      ConnectVertices(left,
          rpBL, rpTL, tpTL, tpBL, 
          rcBL, rcTL, tcTL, tcBL,
          -1);

      ConnectVertices(top,
          rpTL, rpTR, tpTR, tpTL,
          rcTL, rcTR, tcTR, tcTL,
          -1);

      ConnectVertices(right,
          rpTR, rpBR, tpBR, tpTR,
          rcTR, rcBR, tcBR, tcTR,
          -1);

      tpBR = tcBR;
      tpTR = tcTR;
      tpTL = tcTL;
      tpBL = tcBL;

      rpBR = rcBR;
      rpTR = rcTR;
      rpTL = rcTL;
      rpBL = rcBL;

    }

    end = new G4QuadrangularFacet(tpBR, tpBL, rpBL, rpBR, ABSOLUTE);
    bottom->AddFacet((G4VFacet*) end);
    bottom->SetSolidClosed(true);

    end = new G4QuadrangularFacet(tpBL, tpTL, rpTL, rpBL, ABSOLUTE);
    left->AddFacet((G4VFacet*) end);
    left->SetSolidClosed(true);

    end = new G4QuadrangularFacet(tpTL, tpTR, rpTR, rpTL, ABSOLUTE);
    top->AddFacet((G4VFacet*) end);
    top->SetSolidClosed(true);

    end = new G4QuadrangularFacet(tpTR, tpBR, rpBR, rpTR, ABSOLUTE);
    right->AddFacet((G4VFacet*) end);
    right->SetSolidClosed(true);


    G4TessellatedSolid* solids[4] = {bottom, top, left, right};
    G4bool intersected = false;
    for(int i = 0; i < 4; i++) intersected = intersected
      or CheckIntersections(solids[i], true);

    if(intersected)
      G4cout << "WARNING: Intersection of border surfaces." << G4endl;

    G4LogicalVolume* bottom_log = new G4LogicalVolume(bottom, the_detector_->air(),
       name_ + "_bottom_surface_log");

    G4LogicalVolume* left_log = new G4LogicalVolume(left, the_detector_->air(),
       name_ + "_left_surface_log");

    G4LogicalVolume* top_log = new G4LogicalVolume(top, the_detector_->air(),
       name_ + "_top_surface_log");

    G4LogicalVolume* right_log = new G4LogicalVolume(right, the_detector_->air(),
       name_ + "_right_surface_log");

    G4RotationMatrix* rot = phys->GetRotation();
    G4ThreeVector trans = phys->GetTranslation();
    G4int copy_no = phys->GetCopyNo();


    G4OpticalSurface *bottom_surface, *left_surface;
    G4String bottom_type, left_type;

    if(left_right_mag < top_bottom_mag) {
      bottom_surface = the_detector_->machined_surface();
      bottom_type = "machined";
      left_surface = the_detector_->cast_surface();
      left_type = "cast";
    } else {
      bottom_surface = the_detector_->cast_surface();
      bottom_type = "cast";
      left_surface = the_detector_->machined_surface();
      left_type = "machined";
    }

    G4LogicalBorderSurface* border_surface;
    G4VPhysicalVolume* border_phys;

    G4LogicalVolume* border_log[4] = {bottom_log, left_log, top_log, right_log};
    G4String names[4] = { "bottom", "left", "top", "right" };
    G4OpticalSurface* border_op_surface;
    G4String border_type;

    for(int i = 0; i < 4; i++ ) {
      border_phys =
        new G4PVPlacement(rot,
            trans,
            border_log[i],
            name_ + "_" + names[i] + "_surface",
            the_detector_->get_logical_world(),
            false,
            copy_no);

      border_op_surface = i % 2 == 0 ? bottom_surface : left_surface;
      border_type = i % 2 == 0 ? bottom_type : left_type;

      border_surface = new G4LogicalBorderSurface(name_ + "_" + names[i] + "_border_1",
          border_phys,
          phys,
          border_op_surface);

      NedmDetectorConstruction::RegisterSurface(border_surface, border_type);

      border_surface = new G4LogicalBorderSurface(name_ + "_" + names[i] + "_border_2",
          phys,
          border_phys,
          border_op_surface); 

      NedmDetectorConstruction::RegisterSurface(border_surface, border_type);

    }

  }

  void BentSolid::ConnectVertices(G4TessellatedSolid* solid,
      const G4ThreeVector& pBR, const G4ThreeVector& pTR,
      const G4ThreeVector& pTL, const G4ThreeVector& pBL,
      const G4ThreeVector& cBR, const G4ThreeVector& cTR,
      const G4ThreeVector& cTL, const G4ThreeVector& cBL,
      G4int direction) {
    
    //This breaks the solid connecting the two slices into faces
    //with each faces consisting of two triangular solids each. The
    //vertices of each triangle are arranged per Geant4's convention,
    //i.e. they are in counter-clockwise order when view from the outside
    //of the solid.
    G4ThreeVector facet_vertices[8][3] = {
      {pBR, pTR, cBR},
      {cBR, pTR, cTR},
      {pTR, pTL, cTR},
      {cTR, pTL, cTL},
      {pTL, pBL, cTL},
      {cTL, pBL, cBL},
      {pBL, pBR, cBL},
      {cBL, pBR, cBR}
    };

    G4int start = 1 - direction;
    for(int i=0; i < 8; i++) {
      G4TriangularFacet* facet = new
        G4TriangularFacet(facet_vertices[i][start],
            facet_vertices[i][start + direction],
            facet_vertices[i][start + 2*direction],
            ABSOLUTE);

      solid->AddFacet((G4VFacet*) facet);
    }

  }
}
