custom(){
echo $1 $2 $3
echo "------------------------------------" 
echo "int 1"             
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom:$1:$2:$3
echo "int 2"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom:$1:$2:$3
echo "mm 1"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom:$1:$2:$3
echo "mm 2"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom:$1:$2:$3
echo "fp 1"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom:$1:$2:$3
echo "fp 2"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom:$1:$2:$3

echo "------------------------------------"
}

custom 32 256 31
custom 32 256 15
custom 32 128 31
custom 32 128 15
custom 0 256 31
custom 32 256 31
custom 64 256 31
custom 128 256 31