#include "NedmDetectorParameters.hh"

#include "G4UnitsTable.hh"

NedmDetectorParameters* NedmDetectorParameters::instance_;

NedmDetectorParameters::NedmDetectorParameters() {
  SetDefaults();
}

void NedmDetectorParameters::SetDefaults() {

  surface_thickness_ = 0.01*cm;
  //  surface_thickness_ = 0.5*cm;
  
  //  lightguide_length_ =   3.5*2.54*cm;
  lightguide_length_ =   4.5*2.54*cm;
  lightguide_cell_gap_ = 1.0*mm;
  acrylicwindow_pmt_gap_ =  0.0*mm;
  lightguide_acrylicwindow_gap_ =  0.0*mm;
  cell_wall_thickness_ = 0.5*2.54*cm;


  //Lightguide extension
  acrylicwindow_radius_ = 2.55 * cm;
  acrylicwindow_z_ = 2.0*2.54*cm; //inner_lightguide_size_.y();


  //PMT Defaults
  pmt_radius_ = 2.54 * cm;
  pmt_z_ = 1.0*cm; //inner_lightguide_size_.y();


  //Geometry Defaults
  mirror_airgap_ = single_airgap_ = 0.02*cm;
  //  use_mirror_ = true;
  use_mirror_ = false;
  use_hole_ = true;
  //  use_hole_ = false;
  use_plug_ = true;
  //  use_plug_ = false;
  //  use_window_ = false;
  use_window_ = true;
  use_acrylicwindow_ = true;
  use_full_acrylicwindow_ = true;
  use_full_pmt_ = true;

}

void NedmDetectorParameters::Compute() {
  ComputeCellParameters();
  ComputeMirrorParameters();
  ComputeEndcapParameters();
  ComputeLightguideParameters();
  ComputeWindowParameters();
  ComputeAcrylicWindowParameters();
  ComputePmtParameters();
}

void NedmDetectorParameters::ComputeCellParameters() {


  cell_inside_x_ = 4.0*2.54*cm; 
  cell_inside_y_ = 3.0*2.54*cm;
  cell_inside_z_ = 40.0*cm;


  //  G4double surface_thickness = 0.01*cm;

  G4double cell_tb_x = 0.5*(cell_inside_x_ + 2.0*surface_thickness_);
  G4double cell_tb_y = 0.5*(cell_wall_thickness_);
  G4double cell_tb_z = 0.5*(cell_inside_z_ + 2.0*cell_wall_thickness_);

  cell_tb_size_ = G4ThreeVector(cell_tb_x, cell_tb_y, cell_tb_z);

  G4double cell_lr_x = 0.5*(cell_wall_thickness_);
  G4double cell_lr_y = 0.5*(cell_inside_y_  + 2.0*cell_wall_thickness_ + 2.0*surface_thickness_);
  G4double cell_lr_z = cell_tb_z;

  cell_lr_size_ = G4ThreeVector(cell_lr_x, cell_lr_y, cell_lr_z);

  G4double cell_fb_x = 0.5*cell_inside_x_;
  G4double cell_fb_y = 0.5*cell_inside_y_;
  G4double cell_fb_z = 0.5*cell_wall_thickness_;

  cell_fb_size_ = G4ThreeVector(cell_fb_x, cell_fb_y, cell_fb_z);

  //The 4 here comes from the fact that there are two full surfaces
  //of thickness 2*surface_thickness between the plates. These are the 
  //surface volumes
  G4double cell_tb_distx_ = 0.0;
  G4double cell_tb_disty_ = cell_lr_y - cell_tb_y; 
  G4double cell_tb_distz_ = 0.0;
 
  cell_wall_tb_position_ = G4ThreeVector(cell_tb_distx_, cell_tb_disty_, cell_tb_distz_);

  G4double cell_lr_distx_ = cell_tb_x + cell_lr_x;
  G4double cell_lr_disty_ = 0.0;
  G4double cell_lr_distz_ = 0.0;

  cell_wall_lr_position_ = G4ThreeVector(cell_lr_distx_, cell_lr_disty_, cell_lr_distz_);

  G4double cell_fb_distx_ = 0.0;
  G4double cell_fb_disty_ = 0.0;
  G4double cell_fb_distz_ = 0.5*cell_inside_z_ + 0.5*cell_wall_thickness_  -  surface_thickness_;

  cell_wall_fb_position_ = G4ThreeVector(cell_fb_distx_, cell_fb_disty_, cell_fb_distz_);


  G4double cell_tb_x_surface_size_ = cell_tb_x;
  G4double cell_tb_y_surface_size_ = 0.5*surface_thickness_;
  G4double cell_tb_z_surface_size_ = cell_tb_z;

  cell_tb_surface_size_ = G4ThreeVector(cell_tb_x_surface_size_, cell_tb_y_surface_size_, cell_tb_z_surface_size_);

  G4double cell_tb_x_surface_position_ = cell_tb_distx_;
  G4double cell_tb_y_surface_position_ = cell_tb_disty_ - cell_tb_y - cell_tb_y_surface_size_;
  G4double cell_tb_z_surface_position_ = cell_tb_distz_;

  cell_tb_surface_position_ = G4ThreeVector(cell_tb_x_surface_position_, cell_tb_y_surface_position_, cell_tb_z_surface_position_);

  G4double cell_lr_x_surface_size_ = 0.5*surface_thickness_;
  G4double cell_lr_y_surface_size_ = cell_lr_y - 2.0*cell_tb_y - surface_thickness_; 
  G4double cell_lr_z_surface_size_ = cell_lr_z;

  cell_lr_surface_size_ = G4ThreeVector(cell_lr_x_surface_size_, cell_lr_y_surface_size_, cell_lr_z_surface_size_);

  G4double cell_lr_x_surface_position_ = cell_tb_x  - cell_lr_x - cell_lr_x_surface_size_;
  G4double cell_lr_y_surface_position_ = 0.0;
  G4double cell_lr_z_surface_position_ = cell_lr_z;


  cell_lr_surface_position_ = G4ThreeVector(cell_lr_x_surface_position_, cell_lr_y_surface_position_, cell_lr_z_surface_position_);

  // outside surfaces
  G4double cell_tb_x_outside_surface_size_ = cell_tb_x + 2.0*cell_lr_x;
  G4double cell_tb_y_outside_surface_size_ = 0.5*surface_thickness_;
  G4double cell_tb_z_outside_surface_size_ = cell_tb_z;

  cell_tb_outside_surface_size_ = G4ThreeVector(cell_tb_x_outside_surface_size_, cell_tb_y_outside_surface_size_, cell_tb_z_outside_surface_size_);

  G4double cell_tb_x_outside_surface_position_ = cell_tb_distx_;
  G4double cell_tb_y_outside_surface_position_ = cell_tb_disty_ + cell_tb_y + cell_tb_y_surface_size_ +cell_tb_y_outside_surface_size_;
  G4double cell_tb_z_outside_surface_position_ = cell_tb_distz_;

  cell_tb_outside_surface_position_ = G4ThreeVector(cell_tb_x_surface_position_, cell_tb_y_surface_position_, cell_tb_z_surface_position_);

  G4double cell_lr_x_outside_surface_size_ = 0.5*surface_thickness_;
  G4double cell_lr_y_outside_surface_size_ = cell_lr_y + surface_thickness_; 
  G4double cell_lr_z_outside_surface_size_ = cell_lr_z;

  cell_lr_outside_surface_size_ = G4ThreeVector(cell_lr_x_outside_surface_size_, cell_lr_y_outside_surface_size_, cell_lr_z_outside_surface_size_);

  G4double cell_lr_x_outside_surface_position_ = cell_tb_x  + cell_lr_x + cell_lr_x_outside_surface_size_;
  G4double cell_lr_y_outside_surface_position_ = 0.0;
  G4double cell_lr_z_outside_surface_position_ = cell_lr_z;


  cell_lr_outside_surface_position_ = G4ThreeVector(cell_lr_x_outside_surface_position_, cell_lr_y_outside_surface_position_, cell_lr_z_outside_surface_position_);


  G4double cell_fb_x_surface_size_ = 0.5*cell_inside_x_;
  G4double cell_fb_y_surface_size_ = 0.5*cell_inside_y_;
  G4double cell_fb_z_surface_size_ = 0.5*surface_thickness_;

  cell_fb_surface_size_ = G4ThreeVector(cell_fb_x_surface_size_, cell_fb_y_surface_size_, cell_fb_z_surface_size_);

  G4double cell_fb_x_surface_position_ = 0.0;
  G4double cell_fb_y_surface_position_ = 0.0;
  G4double cell_fb_z_surface_position_ = 0.5*cell_inside_z_ - 0.5*surface_thickness_;

  cell_fb_surface_position_ = G4ThreeVector(cell_fb_x_surface_position_, cell_fb_y_surface_position_, cell_fb_z_surface_position_);

  G4double cell_fb_x_outside_surface_size_ = cell_fb_x_surface_size_;
  G4double cell_fb_y_outside_surface_size_ = cell_fb_y_surface_size_;
  G4double cell_fb_z_outside_surface_size_ = cell_fb_z_surface_size_;

  cell_fb_outside_surface_size_ = G4ThreeVector(cell_fb_x_outside_surface_size_, cell_fb_y_outside_surface_size_, cell_fb_z_outside_surface_size_);

  G4double cell_fb_x_outside_surface_position_ = 0.0;
  G4double cell_fb_y_outside_surface_position_ = 0.0;
  G4double cell_fb_z_outside_surface_position_ = cell_inside_z_ + cell_wall_thickness_ + surface_thickness_;

  cell_fb_outside_surface_position_ = G4ThreeVector(cell_fb_x_outside_surface_position_, cell_fb_y_outside_surface_position_, cell_fb_z_outside_surface_position_);

  



  //Cell hole defaults
  hole_position_ = G4ThreeVector(0, -(cell_tb_surface_size().y()-cell_tb_outside_surface_size().y() ), 0);
  hole_radius_ = 1.00*cm;

}

void NedmDetectorParameters::ComputeLightguideParameters() 
{

  std::string filename = "LightGuideValues.txt";
  std::ifstream ifs ( filename.c_str(), std::ifstream::in );

  std::vector<G4double> lightguide_dx;
  std::vector<G4double> lightguide_dy;
  std::vector<G4double> lightguide_dz;
  std::vector<G4double> lightguide_angleX;
  std::vector<G4double> lightguide_angleY;
  
  size_t nlines(0);

  GetLightGuideGeometry(lightguide_dx, lightguide_dy, lightguide_dz, lightguide_angleX, lightguide_angleY);
  
  //  for(int i=0; i<lightguide_dx.size(); i++){
  //    std::cout << i << ' ' << lightguide_angleX[i] << ' ' << lightguide_angleY[i] << std::endl;
  //  }


  nsegments = lightguide_dx.size();

  lightguide_size_.resize(nsegments);

  for(size_t k=0; k< nsegments; k++){ 
    lightguide_size_[k].setX(lightguide_dx[k]);
    lightguide_size_[k].setY(lightguide_dy[k]);
    lightguide_size_[k].setZ(lightguide_dz[k]);
    //    std::cout<<  k << ' ' << lightguide_size(k) << std::endl;

  }

  //LG Defaults
  lightguide_gap_ = 5.*um;
  lightguide_files_[0] = lightguide_files_[1] = lightguide_files_[2] = "";
  lightguide_loss_ = true;


  lightguide_topbottom_surface_size_.resize(nsegments);

  for(size_t k=0; k< nsegments; k++){ 
    lightguide_topbottom_surface_size_[k].setX(lightguide_dx[k]);
    lightguide_topbottom_surface_size_[k].setY(surface_thickness_);
    lightguide_topbottom_surface_size_[k].setZ(lightguide_dz[k]/(cos(lightguide_angleX[k]*deg)));
  }

  lightguide_leftright_surface_size_.resize(nsegments);

  for(size_t k=0; k< nsegments; k++){    
    lightguide_leftright_surface_size_[k].setX(surface_thickness_);
    lightguide_leftright_surface_size_[k].setY(lightguide_dy[k]+2.0*surface_thickness_);
    lightguide_leftright_surface_size_[k].setZ(lightguide_dz[k]/cos(lightguide_angleY[k]*deg));
  }

  lightguide_frontback_surface_size_.resize(2);

  lightguide_frontback_surface_size_[0].setX(lightguide_size_[0].getX()+2*surface_thickness_);
  lightguide_frontback_surface_size_[0].setY(lightguide_size_[0].getY()+2*surface_thickness_);
  lightguide_frontback_surface_size_[0].setZ(surface_thickness_);

  lightguide_frontback_surface_size_[1].setX(lightguide_size_[nsegments-1].getX()+2.0*surface_thickness_);
  lightguide_frontback_surface_size_[1].setY(lightguide_size_[nsegments-1].getY()+2.0*surface_thickness_);
  lightguide_frontback_surface_size_[1].setZ(surface_thickness_);


  lightguide_position_.resize(nsegments);

  for(size_t k=0; k< nsegments; k++){
    if(nsegments){
      lightguide_position_[k].setX(0);
      lightguide_position_[k].setY(0);
      lightguide_position_[k].setZ(cell_lr_size_.getZ()+(2.0*k+1)*lightguide_size_[k].getZ()+2.0*surface_thickness_ + lightguide_cell_gap_);
    }
  }

  lightguide_topbottom_surface_position_.resize(nsegments);
  lightguide_leftright_surface_position_.resize(nsegments);

  lightguide_topbottom_surface_rotation_angle_.resize(nsegments);
  lightguide_leftright_surface_rotation_angle_.resize(nsegments);


  for(size_t k=0; k< (nsegments); k++){
    if(nsegments){
      lightguide_topbottom_surface_position_[k] = lightguide_position_[k];
      lightguide_topbottom_surface_position_[k].setY(0.5*(lightguide_size_[k].getY() + lightguide_size_[k+1].getY())  + surface_thickness_/(cos(lightguide_angleX[k]*deg)) - surface_thickness_*tan(fabs(lightguide_angleX[k])*deg)*sin(fabs(lightguide_angleX[k])*deg));
     lightguide_topbottom_surface_position_[k].setZ(lightguide_position_[k].getZ() + surface_thickness_*tan(fabs(lightguide_angleX[k])*deg)*cos(fabs(lightguide_angleX[k])*deg));

      lightguide_topbottom_surface_rotation_angle_[k] = -lightguide_angleX[k]*deg; 

      lightguide_leftright_surface_position_[k] = lightguide_position_[k];
      lightguide_leftright_surface_position_[k].setX(0.5*(lightguide_size_[k].getX() + lightguide_size_[k+1].getX())  + surface_thickness_/(cos(lightguide_angleY[k]*deg)) - surface_thickness_*tan(fabs(lightguide_angleY[k])*deg)*sin(fabs(lightguide_angleY[k])*deg));
     lightguide_leftright_surface_position_[k].setZ(lightguide_position_[k].getZ() + surface_thickness_*tan(fabs(lightguide_angleY[k])*deg)*cos(fabs(lightguide_angleY[k])*deg));



      lightguide_leftright_surface_rotation_angle_[k] = lightguide_angleY[k]*deg; 
    }
  }


  lightguide_frontback_surface_position_.resize(2);
  
  G4ThreeVector Center = G4ThreeVector(0,0,0);

  lightguide_frontback_surface_position_[0] = Center;
  lightguide_frontback_surface_position_[0].setZ(cell_lr_size_.getZ() + surface_thickness_ + lightguide_cell_gap_);
  lightguide_frontback_surface_position_[1] = Center;
  lightguide_frontback_surface_position_[1].setZ(cell_lr_size_.getZ() + 2.0*(nsegments-1)*lightguide_size_[0].getZ() + 3.0*surface_thickness_ + lightguide_cell_gap_); //assuming all lg blocks are of same length

  //  G4double outer_x_position = outer_x + inner_lightguide_size_.x() + lightguide_gap_;
}

void NedmDetectorParameters::ComputePmtParameters() {

  //  G4double cell_z = cell_lr_size_.z();
  //  G4double lg_z = 1.0*cm;//inner_lightguide_size_.z();
  G4double pmt_posz;

  if(use_window_)
    pmt_posz = lightguide_frontback_surface_position_[1].getZ() + 3*surface_thickness_ + acrylicwindow_z_ + lightguide_acrylicwindow_gap_ + 0.5*pmt_z_ + acrylicwindow_pmt_gap_;
  else
    pmt_posz = lightguide_frontback_surface_position_[1].getZ() + 3*surface_thickness_ + lightguide_acrylicwindow_gap_ + 0.5*pmt_z_ + acrylicwindow_pmt_gap_;

  //  if(!use_lightguide_files())
  //    pmt_posz += 2*lg_z;

  pmt_position_.setZ(pmt_posz);
}

void NedmDetectorParameters::ComputeAcrylicWindowParameters() {

  G4double acrylicwindow_posz;

  if(use_acrylicwindow_)
    acrylicwindow_posz = lightguide_frontback_surface_position_[1].getZ() + 2*surface_thickness_ + 0.5*acrylicwindow_z_ + lightguide_acrylicwindow_gap_;
  else
    acrylicwindow_posz = lightguide_frontback_surface_position_[1].getZ()  + 2*surface_thickness_  + 0.5*acrylicwindow_z_ + lightguide_acrylicwindow_gap_;

  acrylicwindow_position_.setZ(acrylicwindow_posz);


  acrylicwindow_frontback_surface_size_ = G4ThreeVector(pmt_radius_, acrylicwindow_radius_, surface_thickness_);

  G4ThreeVector Center = G4ThreeVector(0,0,0);

  acrylicwindow_frontback_surface_position_.resize(2);

  acrylicwindow_frontback_surface_position_[0] = Center;
  acrylicwindow_frontback_surface_position_[0].setZ(lightguide_frontback_surface_position_[1].getZ() + 1.5*surface_thickness_ + lightguide_acrylicwindow_gap_);
  acrylicwindow_frontback_surface_position_[1] = Center;
  acrylicwindow_frontback_surface_position_[1].setZ(lightguide_frontback_surface_position_[1].getZ() + 2.5*surface_thickness_ + acrylicwindow_z_ + lightguide_acrylicwindow_gap_);

  acrylicwindow_center_surface_size_ = G4ThreeVector(acrylicwindow_radius_, acrylicwindow_radius_ + surface_thickness_, acrylicwindow_z_ + 2.0*surface_thickness_);


  acrylicwindow_center_surface_position_ = acrylicwindow_position_;


}



void NedmDetectorParameters::ComputeMirrorParameters() {
  mirror_size_ = G4ThreeVector(5.08, 0.635, 0.5) * cm;
  mirror_z_position_ = -(cell_lr_size_.z() + mirror_size_.z() + mirror_airgap_);
}

void NedmDetectorParameters::ComputeEndcapParameters() {
  endcap_size_ = G4ThreeVector(cell_tb_size_.x(), cell_tb_size_.x() - 2*cell_tb_size_.y(), 0.635*cm);
  endcap_position_ = G4ThreeVector(0, 0, cell_lr_size_.z() - endcap_size_.z());
}

void NedmDetectorParameters::ComputeWindowParameters() {
  cold_gap_z_ = 0.03175*cm;          // half of cold gap
  window_to_pmt_z_ = 0.03175*cm;     // half of gap b/w quartz win and PMT
  window_z_ = 0.3175*cm;       // half of quartz win thickness
  if(use_lightguide_files())
    window_radius_ = 2.54*cm;
  else
    window_radius_ = 10. * cm;

  window_position_ = G4ThreeVector(0, 0,cell_lr_size_.z()  + 2*cold_gap_z_ + window_z_);
                      
}

void NedmDetectorParameters::Print() {
  G4cout << "Hole enabled?: " << use_hole_ << G4endl;

  G4cout << "Mirror Enabled?: " << use_mirror_ << G4endl;

  G4cout << "Width of Single Airgap: " <<
    G4BestUnit(single_airgap_, "Length") << G4endl;

  G4cout << "Width of Mirror Airgap: " <<
    G4BestUnit(mirror_airgap_, "Length") << G4endl;


}

void NedmDetectorParameters::GetLightGuideGeometry(std::vector<G4double>& dx, std::vector<G4double>& dy, std::vector<G4double>& dz, std::vector<G4double>& angleX, std::vector<G4double>& angleY)
{

  SetDefaults();

  double L1 = lightguide_length_; ///m;

  size_t nSlices = 10;
  double dL1(L1/double(nSlices)); //m


  double X11(cell_inside_x_ + 2*(cell_wall_thickness_+ surface_thickness_));
  double Y11(cell_inside_y_ + 2*(cell_wall_thickness_+ surface_thickness_));
  //  double X11(cell_inside_x_ + 2*(cell_wall_thickness_+ surface_thickness_) + 2.0*mm);
  //  double Y11(cell_inside_y_ + 2*(cell_wall_thickness_+ surface_thickness_) + 2.0*mm);

  double X21(0.05*m); 
  double Y21(X21);

  double aX1(1.1);  // note: negative values for aX or aY change curvature from "+" to "-"
  double aY1(aX1);


  std::vector<double> x1;
  std::vector<double> y1;
  std::vector<double> z1;
  std::vector<double> angleX1;
  std::vector<double> angleY1;

  GetGeometry(dL1, aX1, aY1, L1, X11, Y11, X21, Y21, x1,y1,z1, angleX1, angleY1);

  size_t n = nSlices;
 
  for(size_t i=0; i< (n+1); i++){
      dx.push_back(0.5*x1[i]);
      dy.push_back(0.5*y1[i]); 
      dz.push_back(0.5*dL1);
      angleX.push_back(angleX1[i]);
      angleY.push_back(angleY1[i]);
 }


  return;
}

void GetGeometry(double dL, double aX, double aY, double L1, double X1, double Y1, double X2, double Y2, std::vector<double>& x, std::vector<double>& y, std::vector<double>& z, std::vector<double>& angleX, std::vector<double>& angleY)
{

  size_t nSegments = size_t(L1/dL);

  double xt(0.0);
  double yt(0.0);
  double zt(0.0);
  


  //  X1 /= m;
  //  Y1 /= m;

  x.push_back(X1);
  y.push_back(Y1);
  z.push_back(zt);
  
  zt += dL;  

  double epsilon(1.e-6);

  while(zt <= (L1+epsilon)){
  // while(fabs(zt-L1) > 1.0e-7){

    //    std::cout << zt << ' ' << L1 << std::endl;

    xt = (X2 - X1)/L1 * zt - aX* (zt-L1)/L1 * zt/L1 * fabs(X2-X1) + X1;
    x.push_back(xt);

    yt = (Y2 - Y1)/L1 * zt - aY* (zt-L1)/L1 * zt/L1 * fabs(Y2-Y1) + Y1;
    y.push_back(yt);
    
    z.push_back(zt);    
      
    zt += dL;
  }


  //  for(size_t i=0; i< (x.size()-1); i++){
  //    std::cout << i << ' ' << x[i] << ' ' << y[i] << ' ' << z[i] << std::endl;
  //  }

  double degree(180.0/M_PI);

  for(size_t i=0; i< nSegments; i++){
    double dx = 0.5*(x[i+1] - x[i]);
    double dy = 0.5*(y[i+1] - y[i]);
    

    double Xangle = atan(dy/(dL))*degree;
    double Yangle = atan(dx/(dL))*degree;

    if(fabs(Xangle) < 1.0e-9) Xangle = 0.0;
    if(fabs(Yangle) < 1.0e-9) Yangle = 0.0;


    angleX.push_back(Xangle);
    angleY.push_back(Yangle);
    

  }

  return;
}
