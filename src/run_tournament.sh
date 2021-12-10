tournament(){
echo $1 $2 $3
echo "------------------------------------" 
echo "int 1"             
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament:$1:$2:$3
echo "int 2"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament:$1:$2:$3
echo "mm 1"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament:$1:$2:$3
echo "mm 2"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament:$1:$2:$3
echo "fp 1"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:$1:$2:$3
echo "fp 2"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament:$1:$2:$3

echo "------------------------------------"
}

tournament 1 2 1
tournament 1 2 2
tournament 1 3 3
tournament 2 2 2
tournament 3 1 2
tournament 3 3 3
tournament 4 2 1
tournament 4 3 3
tournament 4 4 4
tournament 5 3 1
tournament 6 4 1
tournament 7 8 8
tournament 9 8 8
tournament 9 10 10
