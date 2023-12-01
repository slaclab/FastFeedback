source $TOOLS/script/use_pydm.sh
export PYDM_DISPLAYS_PATH=$TOOLS/pydm/display/
pydm --hide-nav-bar -m "LOOP=FB16:TR01" $PYDM/fbck/fbck_details.py & 
