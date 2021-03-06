/*
 *  $Id: DmpSimAlg.h, 2015-01-27 11:57:14 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 10/06/2014
*/

#ifndef DmpSimAlg_H
#define DmpSimAlg_H

#include "DmpVAlg.h"

class DmpSimRunManager;
class DmpSimPrimaryGeneratorAction;
class DmpSimDetector;
class DmpSimTrackingAction;
class G4PhysListFactory;

class DmpSimAlg : public DmpVAlg{
/*
 *  DmpSimAlg
 *
 *
 */
public:
  DmpSimAlg();
  ~DmpSimAlg();

  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

public:
  void Set(const std::string &option,const std::string &value);
  /*
   * Options:
   *    |--Mode
   *    |       value:
   *    |           "batch"     // default
   *    |           "Vis" or "vis" or "VIS" or "visualization" or "Visualization"
   *    +--Physics
   *    |       value:
   *    |           "QGSP_BIC"  // default
   *    |           "QGSP_BERT"
   *    |           "QGSP_BERT_HP"
   *    |           many values...
   *    +--Gdml
   *    |       value:
   *    |          sub-directory       // default: "FM" (Will read $DMPSWSYS/share/Geometry/FM/DAMPE.gdml)
   *    |          "Path/Filename.gdml"
   *    +--Seed             // default: current time
   *    +--Nud
   *    |  +--DeltaTime     // default: "100"
   *    +--BT
   *    |  +--Magnetic      // default: "0 0 0"
   *    |  +--DAMPE
   *    |  |  +--Translation    // deault: "0 0 0"
   *    |  |  +--Rotation       // deault: "0 0 0"
   */

private:
  DmpSimRunManager      *fSimRunMgr;    // run manager of simulation
  G4PhysListFactory     *fPhyFactory;
  DmpSimPrimaryGeneratorAction  *fSource;   // particle source
  DmpSimDetector        *fDetector;     // DAMPE detector
  DmpSimTrackingAction  *fTracking;     // tracking action

};

#endif

