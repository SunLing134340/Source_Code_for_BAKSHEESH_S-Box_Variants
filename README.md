# Source Code for the Generation, Classification, and Evaluation of 4-Bit S-Boxes for BAKSHEESH Variants

This repository contains the source code and auxiliary files used to generate, classify, and evaluate the 4-bit S-boxes employed in the BAKSHEESH variants studied in our work.

## Requirements

Some experiments require the following external compilers, solvers, or libraries:

- `g++`
- Python 3
- Gurobi
- A SAT solver, e.g., Kissat or CaDiCaL
- CryptoMiniSat

---

## Directory Structure

### `./1.Generate_277120_S-boxes`

This directory contains the code used to generate the 277,120 S-boxes considered in our analysis.

Run:

```bash
g++ -o test main.cpp -O3 && ./test
```

Output:

```text
Result.out
```

The file `Result.out` contains the Boolean representations of the 277,120 generated S-boxes.

---

### `./2.Classify_539_Classes`

This directory contains the code used to classify the 277,120 generated S-boxes into 539 equivalence classes.

Run:

```bash
g++ -o test main.cpp -O3 && ./test && ./Analyze.sh
```

Output:

```text
PatternDistribution.out
```

The file `PatternDistribution.out` contains information about the 539 equivalence classes. For each class, it records:

- the number of S-boxes belonging to the class;
- the sets of indices of their nonlinear coordinate functions with respect to the 25 affine-equivalence classes listed in Table 1 of the paper.

---

### `./3.Output_49984_S-boxes`

This directory contains the code and auxiliary files used to extract the 49,984 S-boxes satisfying `LBN = DBN = 3`.

Run:

```bash
g++ -o test main.cpp -O3 && ./test
```

Output:

```text
DBN3Sbox.out
```

The file `DBN3Sbox.out` contains the 49,984 S-boxes with `LBN = DBN = 3`.

---

### `./4.Classify_44_Classes`

This directory contains the code and files for classifying the 4,864 S-boxes from AE Class #295 into 44 equivalence classes according to their DDT entries and the absolute values of their LAT entries.

Run:

```bash
g++ -o test main.cpp -O3 && ./test && ./Analyze.sh
```

Output:

```text
PatternDistribution.out
```

The file `PatternDistribution.out` contains information about the 44 equivalence classes. For each class, it records:

- the number of S-boxes belonging to the class;
- the DDT entries of the S-boxes in the class;
- the absolute values of the LAT entries of the S-boxes in the class.

---

### `./5.Search_Differential_Bound`

This directory contains the code and auxiliary files used to evaluate the differential bounds of the 44 BAKSHEESH variants.

We tested both the pure SAT-based method of [SWW21] and the SAT+MILP-based method of [WFH+23]. In our experiments, the method of [WFH+23] was slightly faster than that of [SWW21]. However, for some BAKSHEESH variants, the search is still very time-consuming. This directory provides the implementation based on the method of [WFH+23].

Requirements:

- Gurobi
- A SAT solver

To set up the test directories, run:

```bash
./SetUpTest.sh
```

This script creates 44 subdirectories, each containing the search program for one variant.

To search for the differential bound of a given variant, enter the corresponding subdirectory and run:

```bash
python3 Search.py
```

The corresponding output files are generated within that variant's subdirectory.

**Note.** This process may be extremely slow. In some of our experiments, the search ran for several months without obtaining the complete bound.

**Important.** Before running the program, line 300 of `Search.py` must be modified to specify the local path to the SAT solver.

---

### `./6.Search_Linear_Bound`

This directory contains the code and auxiliary files used to evaluate the linear bounds of the 44 BAKSHEESH variants.

As in the differential-bound search, this implementation is based on the method of [WFH+23].

Requirements:

- Gurobi
- A SAT solver

To set up the test directories, run:

```bash
./SetUpTest.sh
```

This script creates 44 subdirectories, each containing the search program for one variant.

To search for the linear bound of a given variant, enter the corresponding subdirectory and run:

```bash
python3 Search.py
```

The corresponding output files are generated within that variant's subdirectory.

**Note.** The search can also be extremely time-consuming for some variants.

---

### `./7.Evaluate_Differential_Clustering_Effect`

This directory contains the code used to evaluate the clustering effect of differential trails associated with the optimal differential trails of the original BAKSHEESH and the following four variants:

- `BAKSHEESH(S(0))`
- `BAKSHEESH(S(27))`
- `BAKSHEESH(S(29))`
- `BAKSHEESH(S(35))`

The directory contains five subdirectories, one for each of the above cipher instances.

For each cipher instance, first enter the corresponding subdirectory and run:

```bash
./SetUpTest.sh
```

This script creates 22 subdirectories:

```text
1.Round
2.Round
...
22.Round
```

These directories contain the search programs for differential trails over 1 to 22 rounds.

To perform the experiment for a specified number of rounds, enter the corresponding `*.Round` directory and run:

```bash
g++ -o test -std=c++11 main.cpp -lcryptominisat5 -O3 && ./test
```

The resulting output records the number of differential trails found for different differential weights. These results are used to evaluate the clustering effect associated with the corresponding optimal differential trail.

---

### `./8.Evaluate_Linear_Clustering_Effect`

This directory contains the code used to evaluate the clustering effect of linear trails associated with the optimal linear trails of the original BAKSHEESH and the following four variants:

- `BAKSHEESH(S(0))`
- `BAKSHEESH(S(27))`
- `BAKSHEESH(S(29))`
- `BAKSHEESH(S(35))`

The directory contains five subdirectories, one for each of the above cipher instances.

For each cipher instance, first enter the corresponding subdirectory and run:

```bash
./SetUpTest.sh
```

This script creates 22 subdirectories:

```text
1.Round
2.Round
...
22.Round
```

These directories contain the search programs for linear trails over 1 to 22 rounds.

To perform the experiment for a specified number of rounds, enter the corresponding `*.Round` directory and run:

```bash
g++ -o test -std=c++11 main.cpp -lcryptominisat5 -O3 && ./test
```

The resulting output records the number of linear trails found for different linear weights. These results are used to evaluate the clustering effect contributing to the corresponding linear hull.

---

### `./9.Search_Impossible_Differential`

This directory contains the code and auxiliary files used to evaluate impossible differential distinguishers for the 44 variants.

Our evaluation follows the methodology of [ST17], with the search implemented using SAT. CryptoMiniSat is used as the SAT solver.

Run:

```bash
g++ -o test -std=c++11 main.cpp -lcryptominisat5 -O3 && ./test
```

Output:

```text
ImpossibleDifferentialTrail.out
```

The file `ImpossibleDifferentialTrail.out` records the number of impossible differential distinguishers found for the different variants under the specified numbers of rounds.

---

### `./10.Search_Zero_Correlation`

This directory contains the code and auxiliary files used to evaluate zero-correlation linear distinguishers for the 44 variants.

The evaluation is implemented using SAT, with CryptoMiniSat as the SAT solver.

Run:

```bash
g++ -o test -std=c++11 main.cpp -lcryptominisat5 -O3 && ./test
```

Output:

```text
ZeroCorrelationTrail.out
```

The file `ZeroCorrelationTrail.out` records the number of zero-correlation linear distinguishers found for the different variants under the specified numbers of rounds.

---

### `./11.Evaluate_Implementation_Cost`

This directory contains the files used to evaluate the implementation costs of the 256 newly identified S-boxes under the lookup-based implementation setting considered in the paper.

---

### `./12.Evaluate_Division_Property`

This directory contains the code and auxiliary files used to evaluate the division-property-based integral resistance of the following four BAKSHEESH variants:

- `BAKSHEESH(S(0))`
- `BAKSHEESH(S(27))`
- `BAKSHEESH(S(29))`
- `BAKSHEESH(S(35))`

The directory contains four subdirectories, one for each variant.

Within each variant directory, there are two subdirectories:

```text
Encryption
Decryption
```

These directories are used to evaluate the division property in the encryption and decryption directions, respectively.

To perform the evaluation, enter either the `Encryption` or `Decryption` directory and run:

```bash
python3 Solve_All_Division.py
```

The program outputs the corresponding division-property evaluation results, which are used to determine whether balanced output bits can be identified for the specified number of rounds.

---

## References

[ST17] Yu Sasaki and Yosuke Todo.  
New Impossible Differential Search Tool from Design and Cryptanalysis Aspects: Revealing Structural Properties of Several Ciphers.  
*EUROCRYPT 2017*, Part III, pp. 185--215, 2017.

[SWW21] Ling Sun, Wei Wang, and Meiqin Wang.  
Accelerating the Search of Differential and Linear Characteristics with the SAT Method.  
*IACR Transactions on Symmetric Cryptology*, 2021(1):269--315, 2021.

[WFH+23] Senpeng Wang, Dengguo Feng, Bin Hu, Jie Guan, Kai Zhang, and Tairong Shi.  
New Method for Combining Matsui's Bounding Conditions with Sequential Encoding Method.  
*Designs, Codes and Cryptography*, 91(11):3603--3642, 2023.
