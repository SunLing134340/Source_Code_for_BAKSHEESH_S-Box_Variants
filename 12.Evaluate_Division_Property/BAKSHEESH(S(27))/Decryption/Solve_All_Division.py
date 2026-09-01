#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import gurobipy as gp
from gurobipy import GRB


TotalRound = 14
SboxConsNum = 5

INE = [[-1, -1, -2, -2, 2, 2, 1, 1, -1], [-3, -3, -2, -1, 1, 1, 2, 3, -4], [-1, -1, -1, -2, 1, 1, 1, 0, -2], [-3, -2, -1, -1, 1, 2, 3, 3, -2], [-2, -3, -1, -1, 2, 1, 3, 3, -2]]

BAKPermutation = [
    12, 9, 6, 3, 28, 25, 22, 19, 44, 41, 38, 35, 60, 57, 54, 51, 76, 73, 70, 67, 92, 89, 86, 83, 108, 105, 102, 99, 124, 121, 118, 115, 8, 5, 2, 15, 24, 21, 18, 31, 40, 37, 34, 47, 56, 53, 50, 63, 72, 69, 66, 79, 88, 85, 82, 95, 104, 101, 98, 111, 120, 117, 114, 127, 4, 1, 14, 11, 20, 17, 30, 27, 36, 33, 46, 43, 52, 49, 62, 59, 68, 65, 78, 75, 84, 81, 94, 91, 100, 97, 110, 107, 116, 113, 126, 123, 0, 13, 10, 7, 16, 29, 26, 23, 32, 45, 42, 39, 48, 61, 58, 55, 64, 77, 74, 71, 80, 93, 90, 87, 96, 109, 106, 103, 112, 125, 122, 119
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
