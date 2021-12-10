custom(){
echo $1 $2 $3
echo "------------------------------------" 
echo "int 1"             
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom
echo "int 2"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom
echo "mm 1"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom 
echo "mm 2"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom
echo "fp 1"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom 
echo "fp 2"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom

echo "------------------------------------"
}

custom
