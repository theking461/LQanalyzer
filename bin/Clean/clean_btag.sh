if [[  $isSLC5 == "True" ]];
    then
    bash $LQANALYZER_DIR/bin/Clean/clean_btag_c98.sh 
else
    bash $LQANALYZER_DIR/bin/Clean/clean_btag_c11.sh
fi