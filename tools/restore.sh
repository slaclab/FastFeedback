if [ "$#" != "3" ]; then
  echo "Usage: restore <gain1> <gain2> <gain3>"
  exit -1
fi

echo "----------------------------------------------------------"
echo -e "Will write the following gains: $1 $2 $3, are you sure? (y/n)"
read response

if [ "$response" != "y" ]; then
  echo "Cancelling operation"
  exit 0 
fi

#echo "Reloading FMatrix"
#echo "caput -a FBCK:FB04:LG01:FMATRIX 200 1.046 0 0 0 0 0 -1.2184 2.1986 0.011464 0 0 0 -0.080912 -0.72954 -0.026117 0 0 0 -0.13831 -0.60952 -0.01451 0.87485 0.037702 0 -9.5878 203.654 0.022006 -0.45644 -0.057178 0 -0.0005691 0.0041105 0 -1.5381 0 2.4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0"
echo "Reloading GMatrix"
caput -a FBCK:FB04:LG01:GMATRIX 200 1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 0 0 $1 0 0 0 0 0 0 $2 0 0 0 0 0 0 $3 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
echo "Turning FB04:LG01 OFF"
caput FBCK:FB04:LG01:STATE 0
sleep 3
echo "Turning FB04:LG01 ON"
caput FBCK:FB04:LG01:STATE 1
