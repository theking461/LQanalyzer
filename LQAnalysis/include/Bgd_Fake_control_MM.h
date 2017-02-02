#ifndef Bgd_Fake_control_MM_h
#define Bgd_Fake_control_MM_h

#include "AnalyzerCore.h"
class Bgd_Fake_control_MM : public AnalyzerCore {

 public:
  //// constructors                                                                                                                                                             
  Bgd_Fake_control_MM();
  ~Bgd_Fake_control_MM();

  /// Functions from core
  virtual void BeginCycle() throw( LQError );
  virtual void BeginEvent()throw( LQError );
  virtual void ExecuteEvents()throw( LQError );
  virtual void EndCycle()throw( LQError );
  virtual void ClearOutputVectors()throw( LQError );
  
  void InitialiseAnalysis() throw( LQError );
  void MakeHistograms();
  void FillCutFlow(TString cut, float w);
 private:
  
  //
  // The output variables 
  //
  /// Vectors for output objetcs
  std::vector<snu::KMuon> out_muons;
  std::vector<snu::KElectron> out_electrons;


  ClassDef ( Bgd_Fake_control_MM, 1);
};
#endif
