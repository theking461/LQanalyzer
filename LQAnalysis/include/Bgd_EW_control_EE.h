#ifndef Bgd_EW_control_EE_h
#define Bgd_EW_control_EE_h

#include "AnalyzerCore.h"
class Bgd_EW_control_EE : public AnalyzerCore {

 public:
  //// constructors                                                                                                                                                             
  Bgd_EW_control_EE();
  ~Bgd_EW_control_EE();

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


  ClassDef ( Bgd_EW_control_EE, 1);
};
#endif
