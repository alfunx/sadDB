#/bin/bash
./random ex_A3_B1 1 R -p 1000 -t 1200 -r 1 1200
sleep 1
./random ex_A3_B1 1 S -p 1000 -t 1200 -r 1 1200
sleep 1
./random ex_A3_B2 1 R -p 1500 -t 1200 -r 1 1200
sleep 1
./random ex_A3_B2 1 S -p 500 -t 1200 -r 1 1200
sleep 1
./random ex_A3_B3 1 R -p 500 -t 1200 -r 1 1200
sleep 1
./random ex_A3_B3 1 S -p 1500 -t 1200 -r 1 1200
sleep 1
for i in {2..6}; do
    ./random ex_A3_B1 "$i" R
    ./random ex_A3_B1 "$i" S
done
for i in {2..6}; do
    ./random ex_A3_B2 "$i" R
    ./random ex_A3_B2 "$i" S
done
for i in {2..6}; do
    ./random ex_A3_B3 "$i" R
    ./random ex_A3_B3 "$i" S
done
