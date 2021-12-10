gshare(){
echo $1
echo "------------------------------------" 
echo "int 1"             
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare:$1
echo "int 2"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare:$1
echo "mm 1"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare:$1
echo "mm 2"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare:$1
echo "fp 1"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare:$1
echo "fp 2"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare:$1

echo "------------------------------------"
}

gshare 1
gshare 2
gshare 3
gshare 4
gshare 5
gshare 6
gshare 7
gshare 8
gshare 9
gshare 10
gshare 11
gshare 12
gshare 13
gshare 13
