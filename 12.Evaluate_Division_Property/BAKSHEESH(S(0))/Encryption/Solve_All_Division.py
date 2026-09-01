#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import gurobipy as gp
from gurobipy import GRB


TotalRound = 14
SboxConsNum = 5

INE = [[-2, -1, -1, -2, 1, 1, 2, 2, -1], [-1, -3, -3, -2, 2, 3, 1, 1, -4], [-2, -1, -1, -1, 1, 0, 1, 1, -2], [-1, -1, 0, -1, 1, 1, 1, 2, 0], [-1, 0, -1, -1, 1, 1, 2, 1, 0]]

BAKPermutation = [
    96, 65, 34, 3, 64, 33, 2, 99, 32, 1, 98, 67, 0, 97, 66, 35,
    100, 69, 38, 7, 68, 37, 6, 103, 36, 5, 102, 71, 4, 101, 70, 39,
    104, 73, 42, 11, 72, 41, 10, 107, 40, 9, 106, 75, 8, 105, 74, 43,
    108, 77, 46, 15, 76, 45, 14, 111, 44, 13, 110, 79, 12, 109, 78, 47,
    112, 81, 50, 19, 80, 49, 18, 115, 48, 17, 114, 83, 16, 113, 82, 51,
    116, 85, 54, 23, 84, 53, 22, 119, 52, 21, 118, 87, 20, 117, 86, 55,
    120, 89, 58, 27, 88, 57, 26, 123, 56, 25, 122, 91, 24, 121, 90, 59,
    124, 93, 62, 31, 92, 61, 30, 127, 60, 29, 126, 95, 28, 125, 94, 63
]


def build_base_model():
    model = gp.Model("BAKSHEESH_MILP")

    model.Params.OutputFlag = 0

    CountVariable = 0

    xin = [[None for _ in range(128)] for _ in range(TotalRound)]
    xout = [[None for _ in range(128)] for _ in range(TotalRound)]

    # xin variables
    for r in range(TotalRound):
        for i in range(128):
            xin[r][i] = CountVariable
            CountVariable += 1
            
    for r in range(TotalRound - 1):
        for i in range(128):
            xout[r][i] = xin[r + 1][i]

    for i in range(128):
        xout[TotalRound - 1][i] = CountVariable
        CountVariable += 1

    a = model.addVars(
        CountVariable,
        vtype=GRB.BINARY,
        name="a"
    )

    # Objective:
    # Minimize sum of final output division variables
    obj = gp.quicksum(a[xout[TotalRound - 1][i]] for i in range(128))
    model.setObjective(obj, GRB.MINIMIZE)

    # S-box constraints
    for r in range(TotalRound):
        temp = [None for _ in range(128)]

        for i in range(128):
            temp[i] = xout[r][BAKPermutation[i]]

        for sbox in range(32):
            X = [None for _ in range(8)]

            for i in range(4):
                X[i] = xin[r][4 * sbox + i]
                X[i + 4] = temp[4 * sbox + i]

            for ine in range(SboxConsNum):
                expr = gp.quicksum(INE[ine][i] * a[X[i]] for i in range(8))

                model.addConstr(expr >= INE[ine][8])

    input_constraints = []
    for i in range(128):
        c = model.addConstr(a[xin[0][i]] == 1)
        input_constraints.append(c)

    output_constraints = []
    for i in range(128):
        c = model.addConstr(a[xout[TotalRound - 1][i]] == 0)
        output_constraints.append(c)

    model.update()

    return model, input_constraints, output_constraints


def solve_all():
    model, input_constraints, output_constraints = build_base_model()

    result_matrix = [[None for _ in range(128)] for _ in range(128)]

    detail_file = "All_Result_Detail.txt"

    with open(detail_file, "w") as fout:
        fout.write("trainput traoutput status objective\n")

        for trainput in range(128):
            print(f"Processing trainput = {trainput}")

            for traoutput in range(128):
                for i in range(128):
                    input_constraints[i].RHS = 1
                input_constraints[trainput].RHS = 0

                for i in range(128):
                    output_constraints[i].RHS = 0
                output_constraints[traoutput].RHS = 1

                model.update()
                model.optimize()

                if model.Status == GRB.OPTIMAL:
                    val = model.ObjVal

                    val_int = int(round(val))
                    result_matrix[trainput][traoutput] = val_int

                    fout.write(f"{trainput} {traoutput} OPTIMAL {val_int}\n")

                elif model.Status == GRB.INFEASIBLE:
                    result_matrix[trainput][traoutput] = "INF"
                    fout.write(f"{trainput} {traoutput} INFEASIBLE INF\n")

                elif model.Status == GRB.UNBOUNDED:
                    result_matrix[trainput][traoutput] = "UNB"
                    fout.write(f"{trainput} {traoutput} UNBOUNDED UNB\n")

                else:
                    result_matrix[trainput][traoutput] = f"STATUS_{model.Status}"
                    fout.write(
                        f"{trainput} {traoutput} STATUS_{model.Status} STATUS_{model.Status}\n"
                    )

                fout.flush()

    matrix_file = "All_Result_Matrix.txt"

    with open(matrix_file, "w") as fout:
        for trainput in range(128):
            line = " ".join(str(result_matrix[trainput][traoutput]) for traoutput in range(128))
            fout.write(line + "\n")

    print(f"Detailed results have been written to {detail_file}")
    print(f"Matrix results have been written to {matrix_file}")


if __name__ == "__main__":
    solve_all()
