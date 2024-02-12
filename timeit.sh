rm testlogs/ttiming.txt testlogs/mtiming.txt testlogs/atiming.txt
touch testlogs/ttiming.txt testlogs/mtiming.txt testlogs/atiming.txt

for i in 10000 13000 16000 19000 22000;
do
    echo
    echo Running sample size $i...
    for j in 1 2 3 4 5 6;
    do
	echo Running tabletest $j/6...
	./tabletest -t -n $i >> testlogs/ttiming.txt
    done

    for j in 1 2 3 4 5 6;
    do
	echo Running mtftabletest $j/6...
	./mtftabletest -t -n $i >> testlogs/mtiming.txt
    done

    for j in 1 2 3 4 5 6;
    do
	echo Running arraytabletest $j/6...
	./arraytabletest -t -n $i >> testlogs/atiming.txt
    done
done
