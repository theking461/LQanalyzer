#!/bin/sh

########################
### SAMPLE LIST ########## 
#######################


source ${LQANALYZER_DIR}/LQRun/txt/list_user_mc.sh


declare -a input_samples=('WZ')

declare -a all_mc=('HN_Tchannel_emem_M-100' 'HN_Tchannel_emem_M-1100' 'HN_Tchannel_emem_M-200' 'HN_Tchannel_emem_M-500' 'HN_Tchannel_epep_M-100' 'HN_Tchannel_epep_M-1100' 'HN_Tchannel_epep_M-200' 'HN_Tchannel_epep_M-500' 'HN_Tchannel_mummum_M-100' 'HN_Tchannel_mummum_M-1100' 'HN_Tchannel_mummum_M-200' 'HN_Tchannel_mummum_M-500' 'HN_Tchannel_mupmup_M-100' 'HN_Tchannel_mupmup_M-1100' 'HN_Tchannel_mupmup_M-200' 'HN_Tchannel_mupmup_M-500' 'DYJets_10to50' 'DYJets_MG_10to50' 'DYJets' 'DYJets_MG' 'DYJets_MG_5to50' 'GG_HToMuMu' 'QCD_Pt-1000toInf_MuEnriched' 'QCD_Pt-120to170_EMEnriched' 'QCD_Pt-120to170_MuEnriched' 'QCD_Pt-15to20_MuEnriched' 'QCD_Pt-170to300_EMEnriched' 'QCD_Pt-170to300_MuEnriched' 'QCD_Pt-20to30_EMEnriched' 'QCD_Pt-20to30_MuEnriched' 'QCD_Pt-300to470_MuEnriched' 'QCD_Pt-300toInf_EMEnriched' 'QCD_Pt-30to50_EMEnriched' 'QCD_Pt-30to50_MuEnriched' 'QCD_Pt-470to600_MuEnriched' 'QCD_Pt-50to80_EMEnriched' 'QCD_Pt-50to80_MuEnriched' 'QCD_Pt-600to800_MuEnriched' 'QCD_Pt-800to1000_MuEnriched' 'QCD_Pt-80to120_EMEnriched' 'QCD_Pt-80to120_MuEnriched' 'SingleTop_s' 'SingleTbar_t' 'SingleTop_t' 'SingleTbar_tW_noHadron' 'SingleTbar_tW' 'SingleTop_tW_noHadron' 'SingleTop_tW' 'TTJets_aMC' 'TTJets_MG' 'TTLL_powheg' 'TTLJ_powheg' 'TT_powheg' 'VBF_HToMuMu' 'WGtoLNuG' 'WGtoLNuEE' 'WGtoLNuMM' 'WJets' 'WJets_MG' 'WWW' 'WWZ' 'WW' 'WZTo3LNu_powheg' 'WZZ' 'WZ' 'WpWpEWK' 'WpWpQCD' 'ZGto2LG' 'ZZTo4L_powheg' 'ZZZ' 'ZZ' 'ttH_nonbb' 'ttH_bb' 'ttW' 'ttZ' 'ttbb') 

declare -a mc_noqcd=('HN_Tchannel_emem_M-100' 'HN_Tchannel_emem_M-1100' 'HN_Tchannel_emem_M-200' 'HN_Tchannel_emem_M-500' 'HN_Tchannel_epep_M-100' 'HN_Tchannel_epep_M-1100' 'HN_Tchannel_epep_M-200' 'HN_Tchannel_epep_M-500' 'HN_Tchannel_mummum_M-100' 'HN_Tchannel_mummum_M-1100' 'HN_Tchannel_mummum_M-200' 'HN_Tchannel_mummum_M-500' 'HN_Tchannel_mupmup_M-100' 'HN_Tchannel_mupmup_M-1100' 'HN_Tchannel_mupmup_M-200' 'HN_Tchannel_mupmup_M-500' 'DYJets_10to50' 'DYJets_MG_10to50' 'DYJets' 'DYJets_MG' 'DYJets_MG_5to50' 'GG_HToMuMu' 'SingleTop_s' 'SingleTbar_t' 'SingleTop_t' 'SingleTbar_tW_noHadron' 'SingleTbar_tW' 'SingleTop_tW_noHadron' 'SingleTop_tW' 'TTJets_aMC' 'TTJets_MG' 'TTLL_powheg' 'TTLJ_powheg' 'TT_powheg' 'VBF_HToMuMu' 'WGtoLNuG' 'WGtoLNuEE' 'WGtoLNuMM' 'WJets' 'WJets_MG' 'WWW' 'WWZ' 'WW' 'WZTo3LNu_powheg' 'WZZ' 'WZ' 'WpWpEWK' 'ZGto2LG' 'ZZTo4L_powheg' 'ZZZ' 'ZZ' 'ttH_nonbb' 'ttH_bb' 'ttW' 'ttZ' 'ttbb') 

declare -a ch_wa=('') 


declare -a ch_wz=('') 

declare -a hn_ll_ee=('') 
declare -a hn_ll_mm=('') 

declare -a hn_ll_em=('') 

declare -a hn_mmm=('') 

declare -a hn_ll_tchann_ee=('HN_Tchannel_emem_M-100' 'HN_Tchannel_emem_M-1100' 'HN_Tchannel_emem_M-200' 'HN_Tchannel_emem_M-500' 'HN_Tchannel_epep_M-100' 'HN_Tchannel_epep_M-1100' 'HN_Tchannel_epep_M-200' 'HN_Tchannel_epep_M-500' '') 

declare -a hn_ll_tchann_mm=('HN_Tchannel_mummum_M-100' HN_Tchannel_mummum_M-1100' HN_Tchannel_mummum_M-200' HN_Tchannel_mummum_M-500' HN_Tchannel_mupmup_M-100' HN_Tchannel_mupmup_M-1100' HN_Tchannel_mupmup_M-200' HN_Tchannel_mupmup_M-500' ') 


declare -a diboson_pythia=('WZ' 'ZZ' 'WW' 'WZTo3LNu_powheg' 'ZZTo4L_powheg' )

declare -a dy_mcatnlo=('DYJets_10to50' 'DYJets') 

declare -a dilepton_list=('DYJets_10to50' 'DYJets' 'WJets' 'WpWpEWK' 'WpWpQCD'  'WZ' 'ZZ' 'WW'  'TTJets_MG'  'SingleTop_s' 'SingleTbar_t' 'SingleTop_t' 'SingleTbar_tW_noHadron'  'SingleTbar_tW' 'SingleTop_tW_noHadron' 'SingleTop_tW' 'WWW' 'ttW' 'ttZ' 'ttH_nonbb' 'ttH_bb' 'ttbb' 'ZZZ' 'WZZ'  'VBF_HToMuMu' 'WGtoLNuG' 'WGtoLNuEE' 'WGtoLNuMM' 'ZGto2LG' )
declare -a trilepton_list=('DYJets_10to50' 'DYJets' 'WJets' 'WZ' 'ZZ' 'WW'  'TTJets_MG' 'SingleTop_s' 'SingleTbar_t' 'SingleTop_t' 'SingleTbar_tW_noHadron'  'SingleTbar_tW' 'SingleTop_tW_noHadron' 'SingleTop_tW' 'WZZ' 'ZZZ' 'WWW'  'WZTo3LNu_powheg' 'ZZTo4L_powheg' )

declare -a qcd=('QCD_Pt-1000toInf_MuEnriched' 'QCD_Pt-120to170_EMEnriched' 'QCD_Pt-120to170_MuEnriched' 'QCD_Pt-15to20_MuEnriched' 'QCD_Pt-170to300_EMEnriched' 'QCD_Pt-170to300_MuEnriched' 'QCD_Pt-20to30_EMEnriched' 'QCD_Pt-20to30_MuEnriched' 'QCD_Pt-300to470_MuEnriched' 'QCD_Pt-300toInf_EMEnriched' 'QCD_Pt-30to50_EMEnriched' 'QCD_Pt-30to50_MuEnriched' 'QCD_Pt-470to600_MuEnriched' 'QCD_Pt-50to80_EMEnriched' 'QCD_Pt-50to80_MuEnriched' 'QCD_Pt-600to800_MuEnriched' 'QCD_Pt-800to1000_MuEnriched' 'QCD_Pt-80to120_EMEnriched' 'QCD_Pt-80to120_MuEnriched' 'WpWpQCD') 

declare -a qcd_mu=('QCD_Pt-1000toInf_MuEnriched' 'QCD_Pt-120to170_MuEnriched' 'QCD_Pt-15to20_MuEnriched' 'QCD_Pt-170to300_MuEnriched' 'QCD_Pt-20to30_MuEnriched' 'QCD_Pt-300to470_MuEnriched' 'QCD_Pt-30to50_MuEnriched' 'QCD_Pt-470to600_MuEnriched' 'QCD_Pt-50to80_MuEnriched' 'QCD_Pt-600to800_MuEnriched' 'QCD_Pt-800to1000_MuEnriched' 'QCD_Pt-80to120_MuEnriched') 

declare -a qcd_eg=('QCD_Pt-120to170_EMEnriched' 'QCD_Pt-170to300_EMEnriched' 'QCD_Pt-20to30_EMEnriched' 'QCD_Pt-300toInf_EMEnriched' 'QCD_Pt-30to50_EMEnriched' 'QCD_Pt-50to80_EMEnriched' 'QCD_Pt-80to120_EMEnriched') 


declare -a hn_mm=('WJets' 'WZ' 'ZZ' 'WW'  'TTJets_MG' 'WpWpEWK' 'WpWpQCD'  'WZZ' 'ZZZ' 'WWW'  'WZTo3LNu_powheg' 'ZZTo4L_powheg' 'ttZ' 'ttW'  'ttH_nonbb' 'ttH_bb' 'ttbb' 'ZZZ' 'WZZ'  'VBF_HToMuMu' 'WGtoLNuG' 'WGtoLNuEE' 'WGtoLNuMM' 'ZGto2LG' )  

declare -a hn_ee=('DYJets_10to50' 'DYJets' 'WJets' 'WZ' 'ZZ' 'WW'  'TTJets_MG' 'WpWpEWK' 'WpWpQCD'  'WZZ' 'ZZZ' 'WWW'  'WZTo3LNu_powheg' 'ZZTo4L_powheg' 'ttZ' 'ttW'  'ttH_nonbb' 'ttH_bb' 'ttbb' 'ZZZ' 'WZZ'  'VBF_HToMuMu' 'WGtoLNuG' 'WGtoLNuEE' 'WGtoLNuMM' 'ZGto2LG' )

declare -a hn_fakeee=('DYJets_10to50' 'DYJets' 'WJets'  'TTJets_MG')


declare -a singletop=('SingleTop_s' 'SingleTop_t' 'SingleTop_tW_noHadron' 'SingleTop_tW') 

