for((a=0;a<44;a++));do
    mkdir $a.Candidate
    scp Search.py $a.Candidate/Search.py
    scp LATOut$a.out $a.Candidate/LATOut.out
    rm LATOut$a.out
done;
