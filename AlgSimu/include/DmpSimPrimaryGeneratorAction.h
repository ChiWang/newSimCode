/*
 *  $Id: DmpSimPrimaryGeneratorAction.h, 2014-10-05 00:06:18 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 03/03/2014
*/

#ifndef DmpSimPrimaryGeneratorAction_H
#define DmpSimPrimaryGeneratorAction_H

#include "G4VUserPrimaryGeneratorAction.hh"

class G4GeneralParticleSource;
class DmpEvtMCPrimaryParticle;
//class DmpMetadata;

class DmpSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction{
public:
  DmpSimPrimaryGeneratorAction();    
  ~DmpSimPrimaryGeneratorAction();
  
  void ApplyGPSCommand();
  void GeneratePrimaries(G4Event*);

public:
  void AdjustmentRotation(const double &rad);
  void AdjustmentTranslation(const G4ThreeVector &v);

private:
  G4GeneralParticleSource   *fGPS;
  DmpEvtMCPrimaryParticle   *fPrimaryParticle;

private:
  G4ThreeVector fDirection;     // direction of particle source
  G4ThreeVector fCentre;        // centre of particle source, unit mm
  //DmpMetadata   *fMetadata;     // 

};

#endif

