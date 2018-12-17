#/bin/bash
for i in {1..6}; do
    ./random ex_A2_B1 "$i" R -p 1000 -t 1200 -r $(( $i * 200 - 200 + 1 )) $(( $i * 200 ))
    sleep 1
    ./random ex_A2_B1 "$i" S -p 1000 -t 1200 -r $(( $i * 200 - 200 + 1 )) $(( $i * 200 ))
    sleep 1
done
for i in {1..6}; do
    ./random ex_A2_B2 "$i" R -p 1500 -t 1200 -r $(( $i * 200 - 200 + 1 )) $(( $i * 200 ))
    sleep 1
    ./random ex_A2_B2 "$i" S -p 500 -t 1200 -r $(( $i * 200 - 200 + 1 )) $(( $i * 200 ))
    sleep 1
done
for i in {1..6}; do
    ./random ex_A2_B3 "$i" R -p 500 -t 1200 -r $(( $i * 200 - 200 + 1 )) $(( $i * 200 ))
    sleep 1
    ./random ex_A2_B3 "$i" S -p 1500 -t 1200 -r $(( $i * 200 - 200 + 1 )) $(( $i * 200 ))
    sleep 1
done
