for((a=0;a<44;a++));do
    mkdir $a.Candidate
    scp Search.py $a.Candidate/Search.py
    scp DDTOut$a.out $a.Candidate/DDTOut.out
    rm DDTOut$a.out
done;
