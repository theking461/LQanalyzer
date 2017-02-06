#!/bin/sh
1;2c
########################
### SAMPLE LIST ########## 
#######################

declare -a example=("" "WW" "" "WZ")

declare -a tmplist=('WpWp_qcd_madgraph' 'ZG_llG_MCatNLO' 'ZZ_llnunu_powheg' 'ZZ_llqq_MCatNLO' 'ZZ_llll_MCatNLO' 'ZZ_llll_powheg' 'ZZ_pythia8' 'ttHnobb_Powheg' 'ttHtobb_Powheg')

declare -a hn=('DYJets_10to50'  'DYJets' 'WW' 'ZZ' 'WZ' 'TTJets_MG')

declare -a hnpair=('HNpair_mumu_WR5000_Zp400_HN100' 'HNpair_mumu_WR5000_Zp750_HN100' 'HNpair_mumu_WR5000_Zp750_HN300' 'HNpair_mumu_WR5000_Zp1500_HN100' 'HNpair_mumu_WR5000_Zp1500_HN300' 'HNpair_mumu_WR5000_Zp1500_HN700' 'HNpair_mumu_WR5000_Zp3000_HN100' 'HNpair_mumu_WR5000_Zp3000_HN300' 'HNpair_mumu_WR5000_Zp3000_HN700')

declare -a hnpair_mm_private=('HNpair_MM_WR5000_Zp500_HN100_private' 'HNpair_MM_WR5000_Zp500_HN200_private' 'HNpair_MM_WR5000_Zp750_HN100_private' 'HNpair_MM_WR5000_Zp750_HN200_private' 'HNpair_MM_WR5000_Zp750_HN300_private' 'HNpair_MM_WR5000_Zp1000_HN100_private' 'HNpair_MM_WR5000_Zp1000_HN300_private' 'HNpair_MM_WR5000_Zp2000_HN100_private' 'HNpair_MM_WR5000_Zp2000_HN300_private' 'HNpair_MM_WR5000_Zp2000_HN500_private' 'HNpair_MM_WR5000_Zp2000_HN700_private' 'HNpair_MM_WR5000_Zp2000_HN900_private' 'HNpair_MM_WR5000_Zp3000_HN100_private' 'HNpair_MM_WR5000_Zp3000_HN400_private' 'HNpair_MM_WR5000_Zp3000_HN700_private' 'HNpair_MM_WR5000_Zp3000_HN1000_private' 'HNpair_MM_WR5000_Zp3000_HN1300_private' 'HNpair_MM_WR5000_Zp4000_HN100_private' 'HNpair_MM_WR5000_Zp4000_HN500_private' 'HNpair_MM_WR5000_Zp4000_HN900_private' 'HNpair_MM_WR5000_Zp4000_HN1300_private' 'HNpair_MM_WR5000_Zp4000_HN1700_private')

declare -a hnpair_ee_private=('HNpair_EE_WR5000_Zp500_HN100_private' 'HNpair_EE_WR5000_Zp500_HN200_private' 'HNpair_EE_WR5000_Zp750_HN100_private' 'HNpair_EE_WR5000_Zp750_HN200_private' 'HNpair_EE_WR5000_Zp750_HN300_private' 'HNpair_EE_WR5000_Zp1000_HN100_private' 'HNpair_EE_WR5000_Zp1000_HN300_private' 'HNpair_EE_WR50\00_Zp2000_HN100_private' 'HNpair_EE_WR5000_Zp2000_HN300_private' 'HNpair_EE_WR5000_Zp2000_HN500_private' 'HNpair_EE_WR5000_Zp2000_HN700_private' 'HNpair_EE_WR5000_Zp2000_HN900_private' 'HNpair_EE_WR5000_Zp3000_HN100_private' 'HNpair_EE_WR5000_Zp3000_HN400_private' 'HNpair_EE_WR5000_Zp3000_HN700_private' 'HNpair_EE_WR5000_Zp3000_HN1000_private' 'HNpair_EE_WR5000_Zp3000_HN1300_private' 'HNpair_EE_WR5000_Zp4000_HN100_private' 'HNpair_EE_WR5000_Zp4000_HN500_private' 'HNpair_EE_WR5000_Zp4000_HN900_private' 'HNpair_EE_WR5000_Zp4000_HN1300_private' 'HNpair_EE_WR5000_Zp4000_HN1700_private')


declare -a crash=('DYJets' 'TTJets_aMC' 'TT_powheg')