/* 
 *  $Id: DmpSimDetector.h, 2014-10-26 00:16:45 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 26/02/2014
 *    Yifeng Wei (weiyf@mail.ustc.edu.cn) 28/09/2014
*/

#ifndef DmpSimDetector_H
#define DmpSimDetector_H

#include "G4VUserDetectorConstruction.hh"

class G4GDMLParser;
//class DmpSimPsdSD;
//class DmpSimStkSD;
class DmpSimBgoSD;
//class DmpSimNudSD;
//class DmpMetadata;

class DmpSimDetector : public G4VUserDetectorConstruction{
/*
 *
 *
 */
public:
  DmpSimDetector();
  ~DmpSimDetector();
  G4VPhysicalVolume* Construct();

private:
  void SetAncillarySD();
  void Adjustment()const;
  //void AddPhotonGenerator()const;
  void ResetMagnetic(const double &x,const double &y,const double &z)const;    // for BT

private:
  G4GDMLParser          *fParser;
  G4VPhysicalVolume     *fPhyVolume;

private:
//  DmpSimPsdSD       *fPsdSD;
//  DmpSimStkSD       *fStkSD;
  DmpSimBgoSD       *fBgoSD;
//  DmpSimNudSD       *fNudSD;

//private:
//  DmpMetadata       *fMetadata;

};

#endif


