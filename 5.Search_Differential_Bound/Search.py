from gurobipy import *
from functools import partial
from itertools import repeat
from multiprocessing import Pool,freeze_support
import os
import time
import random
import math

FullRound = 36

SearchRoundStart = 1
SearchRoundEnd = 23
InitialLowerBound = 0

DiffActiveSbox = list([])
for i in range(FullRound):
    DiffActiveSbox += [0]
    
SymbolicCNFConstraintForSbox = open("./DDTOut.out").readlines()
TotalConsSbox = len(SymbolicCNFConstraintForSbox)
for i in range(TotalConsSbox):
    SymbolicCNFConstraintForSbox[i] = SymbolicCNFConstraintForSbox[i].strip().split(' ')
    TotalColumn = len(SymbolicCNFConstraintForSbox[0])
    for j in range(TotalColumn):
        SymbolicCNFConstraintForSbox[i][j] = int(SymbolicCNFConstraintForSbox[i][j])
    
def AccurateLowerBound(obj, TOTALROUND, MatsuiProbabilityBound, BlockWeightSize):
    M = Model()
    M.Params.LogToConsole = 0
    wvariables = []
    for i in range(TOTALROUND):
        wvariables.append([])
        for j in range(BlockWeightSize):
            wvariables[i].append(M.addVar(vtype = GRB.BINARY))
    for e1 in range(TOTALROUND):
        for e2 in range(e1,max(TOTALROUND,e1+1)):
            constrain = LinExpr()
            for i in range(e1,e2+1):
                for j in range(BlockWeightSize):
                    constrain += wvariables[i][j]
            M.addConstr(constrain >= MatsuiProbabilityBound[e2 - e1 + 1])
    constrain = LinExpr()
    for i in range(TOTALROUND):
        for j in range(BlockWeightSize):
            constrain += wvariables[i][j]
    M.addConstr(constrain <= MatsuiProbabilityBound[TOTALROUND])
    wvar = []
    for var in wvariables:
        wvar+= var
    objconstrain = LinExpr()
    for i in range(obj+1):
        objconstrain += wvar[i]
    M.setObjective(objconstrain,GRB.MINIMIZE)
    M.optimize ()
    if (M.Status == 2) :
        return(round(M.objVal))
    else:
        return("infeasible")
        
def AccurateUpperBound(obj, TOTALROUND, MatsuiProbabilityBound, BlockWeightSize):
    M = Model()
    M.Params.LogToConsole = 0
    wvariables = []
    for i in range(TOTALROUND):
        wvariables.append([])
        for j in range(BlockWeightSize):
            wvariables[i].append(M.addVar(vtype = GRB.BINARY))
    for e1 in range(TOTALROUND):
        for e2 in range(e1,max(TOTALROUND,e1+1)):
            constrain = LinExpr()
            for i in range(e1,e2+1):
                for j in range(BlockWeightSize):
                    constrain += wvariables[i][j]
            M.addConstr(constrain >= MatsuiProbabilityBound[e2 - e1 + 1])
    constrain = LinExpr()
    for i in range(TOTALROUND):
        for j in range(BlockWeightSize):
            constrain += wvariables[i][j]
    M.addConstr(constrain <= MatsuiProbabilityBound[TOTALROUND])
    wvar = []
    for var in wvariables:
        wvar+= var
    objconstrain = LinExpr()
    for i in range(obj+1):
        objconstrain += wvar[i]
    M.setObjective(objconstrain,GRB.MAXIMIZE)
    M.optimize ()
    if (M.Status == 2) :
        return(round(M.objVal))
    else:
        return("infeasible")
        
def GenerateAndCountVariables(Round, objLowerBound, objUpperBound):
    count_var_num = 0
    xin = []
    p = []
    xout = []
    for i in range(Round):
        xin.append([])
        p.append([])
        xout.append([])
        for j in range(128):
            xin[i].append(0)
        for j in range(32):
            p[i].append(0)
        for j in range(128):
            xout[i].append(0)
    for i in range(Round):
        for j in range(128):
            xin[i][j] = count_var_num
            count_var_num += 1
        for j in range(32):
            p[i][j] = count_var_num
            count_var_num += 1
    for i in range(Round - 1):
        for j in range(128):
            xout[i][j] = xin[i + 1][j]
    for i in range(128):
        xout[Round - 1][i] = count_var_num
        count_var_num += 1
    blockweightsize = 32
    auxiliary_var_u = []
    for r in range(0, Round):
        auxiliary_var_u.append([])
        for i in range(blockweightsize):
            if (r == (Round - 1)) and (i == blockweightsize - 1):
                continue
            auxiliary_var_u[r].append([])
            if(objLowerBound[(r)*(blockweightsize)+i] == objUpperBound[(r)*(blockweightsize)+i]):
                continue
            for j in range(objLowerBound[(r)*(blockweightsize)+i],objUpperBound[(r)*(blockweightsize)+i]):
                auxiliary_var_u[r][i].append(count_var_num)
                count_var_num += 1
    return(xin, p, xout, auxiliary_var_u, count_var_num)
    
def CountClausesInRoundFunction(Round, count_clause_num):
    count = count_clause_num
    # Nonzero input
    count += 1
    # Clauses for round function
    for r in range(Round):
        for i in range(32):
            for j in range(TotalConsSbox):
                count += 1
    return count
    
def CountClausesInSequentialEncoding(TotalRound,Wvars, Uvars, objLowerBound,objUpperBound, count_clause_num):
    if(len(Uvars[0]) > 0):
        count_clause_num += 1
    if (objLowerBound[0] == 0) and (objUpperBound[0] == 0):
        count_clause_num += 1
    if (objLowerBound[0] == 1) and (objUpperBound[0] == 1):
        count_clause_num += 1
    if len(Wvars) > 2:
        for i in range(1,len(Wvars)-1):
            l_i_1 = objLowerBound[i-1]
            m_i_1 = objUpperBound[i-1]
            l_i = objLowerBound[i]
            m_i = objUpperBound[i]
            if(m_i == 0):
                count_clause_num += 1
                continue
            if(l_i == 0):
                count_clause_num += 1
                if(l_i_1 < m_i_1):
                    count_clause_num += 1
            if m_i > max(1,l_i):
                for j in range(max(1,l_i),m_i):
                    if(j <= l_i_1):
                        count_clause_num += 1
                    if(  (j > l_i_1) and (j <= m_i_1)):
                        count_clause_num += 1
                    if( (j >= l_i_1) and (j <= m_i_1 - 1) ):
                        count_clause_num += 1
            if (l_i_1 == m_i):
                count_clause_num += 1
            if ((m_i_1 == m_i) and (l_i_1 < m_i)):
                count_clause_num += 1
    if len(Wvars) >= 2:
        i = len(Wvars) - 1
        l_i_1 = objLowerBound[i-1]
        m_i_1 = objUpperBound[i-1]
        l_i = objLowerBound[i]
        m_i = objUpperBound[i]
        if ( (l_i_1 == m_i)):
            count_clause_num += 1
        if ((m_i_1 == m_i) and (l_i_1 < m_i)):
            count_clause_num += 1
    return(count_clause_num)
    
def GenRoundConstrain(TotalRound, xin, p, xout, file):
    # Add constraints to claim nonzero input differences
    clauseseq = ""
    for i in range(128):
        clauseseq += str(xin[0][i] + 1) + " "
    clauseseq += "0" + "\n"
    file.write(clauseseq)
    for r in range(TotalRound):
        y = list([])
        P = [96, 65, 34, 3, 64, 33, 2, 99, 32, 1, 98, 67, 0, 97, 66, 35, 100, 69, 38, 7, 68, 37, 6, 103, 36, 5, 102, 71, 4, 101, 70, 39, 104, 73, 42, 11, 72, 41, 10, 107, 40, 9, 106, 75, 8, 105, 74, 43, 108, 77, 46, 15, 76, 45, 14, 111, 44, 13, 110, 79, 12, 109, 78, 47, 112, 81, 50, 19, 80, 49, 18, 115, 48, 17, 114, 83, 16, 113, 82, 51, 116, 85, 54, 23, 84, 53, 22, 119, 52, 21, 118, 87, 20, 117, 86, 55, 120, 89, 58, 27, 88, 57, 26, 123, 56, 25, 122, 91, 24, 121, 90, 59, 124, 93, 62, 31, 92, 61, 30, 127, 60, 29, 126, 95, 28, 125, 94, 63]
        for i in range(128):
            y += [xout[r][P[i]]]
        for i in range(32):
            X = list([])
            for j in range(4):
                X += [xin[r][4 * i + j]]
            for j in range(4):
                X += [y[4 * i + j]]
            X += [p[r][i]]
            for j in range(TotalConsSbox):
                clauseseq = ""
                for k in range(9):
                    if (SymbolicCNFConstraintForSbox[j][k] == 1):
                        clauseseq += "-" + str(X[k] + 1) + " "
                    if (SymbolicCNFConstraintForSbox[j][k] == 0):
                        clauseseq += str(X[k] + 1) + " "
                clauseseq += "0" + "\n"
                file.write(clauseseq)
                
def GenSequentialEncoding(TotalRound, Wvars, Uvars, objLowerBound, objUpperBound, file):
    if(len(Uvars[0]) > 0):
        clauseseq = "-" + str(Wvars[0] + 1) + " " + str(Uvars[0][0] + 1) + " 0" + "\n"
        file.write(clauseseq)
    if (objLowerBound[0] == 0) and (objUpperBound[0] == 0):
        clauseseq = "-" + str(Wvars[0] + 1) +  " 0" + "\n"
        file.write(clauseseq)
    if (objLowerBound[0] == 1) and (objUpperBound[0] == 1):
        clauseseq =  str(Wvars[0] + 1) +  " 0" + "\n"
        file.write(clauseseq)
    if len(Wvars) > 2:
        for i in range(1,len(Wvars)-1):
            l_i_1 = objLowerBound[i-1]
            m_i_1 = objUpperBound[i-1]
            l_i = objLowerBound[i]
            m_i = objUpperBound[i]
            if(m_i == 0):
                clauseseq = "-" + str(Wvars[i] + 1) + " 0" + "\n"
                file.write(clauseseq)
                continue
            if(l_i == 0):
                clauseseq = "-" + str(Wvars[i] + 1) + " " + str(Uvars[i][0] + 1) + " 0" + "\n"
                file.write(clauseseq)
                if(l_i_1 < m_i_1):
                    clauseseq = "-" + str(Uvars[i-1][0] + 1) + " " + str(Uvars[i][0] + 1) + " 0" + "\n"
                    file.write(clauseseq)
            if m_i > max(1,l_i):
                for j in range(max(1,l_i),m_i):
                    if(j <= l_i_1):
                        clauseseq = "-" + str(Wvars[i] + 1) + " " + str(Uvars[i][j - l_i] + 1) + " 0\n"
                        file.write(clauseseq)
                    if(  (j > l_i_1) and (j <= m_i_1)):
                        clauseseq = "-" + str(Wvars[i] + 1) + " " + "-" + str(Uvars[i-1][ j-1 - l_i_1] + 1) + " " + str(Uvars[i][j - l_i] + 1) + " 0\n"
                        file.write(clauseseq)
                    if( (j >= l_i_1) and (j <= m_i_1 - 1) ):
                        clauseseq =  "-" + str(Uvars[i-1][j - l_i_1] + 1) + " "  + str(Uvars[i][j - l_i] + 1) + " 0\n"
                        file.write(clauseseq)
            if (l_i_1 == m_i):
                clauseseq = "-" + str(Wvars[i] + 1) +  " 0\n"
                file.write(clauseseq)
            if ((m_i_1 == m_i) and (l_i_1 < m_i)):
                clauseseq = "-" + str(Wvars[i] + 1) + " " + "-" + str(Uvars[i-1][len(Uvars[i-1])-1] + 1) + " 0\n"
                file.write(clauseseq)
    if len(Wvars) >= 2:
        i = len(Wvars) - 1
        l_i_1 = objLowerBound[i-1]
        m_i_1 = objUpperBound[i-1]
        l_i = objLowerBound[i]
        m_i = objUpperBound[i]
        if ( (l_i_1 == m_i)):
            clauseseq = "-" + str(Wvars[i] + 1) +  " 0" + "\n"
            file.write(clauseseq)
        if ((m_i_1 == m_i) and (l_i_1 < m_i)):
            clauseseq = "-" + str(Wvars[i] + 1) + " " + "-" + str(Uvars[i-1][len(Uvars[i-1])-1] + 1) + " 0" + "\n"
            file.write(clauseseq)
        
def Decision(TotalRound, objLowerBound, objUpperBound):
    ActiveSbox = DiffActiveSbox[TotalRound]
    count_var_num = 0
    count_clause_num = 0
    (xin, p, xout, auxiliary_var_u, count_var_num) = GenerateAndCountVariables(TotalRound, objLowerBound, objUpperBound)
    Wvars = []
    for r in range(TotalRound):
        for i in range(32):
            Wvars.append(p[r][i])
    Uvars = []
    for uvar in auxiliary_var_u:
        Uvars += uvar
    count_clause_num = 0
    count_clause_num = CountClausesInRoundFunction(TotalRound, count_clause_num)
    count_clause_num = CountClausesInSequentialEncoding(TotalRound, Wvars, Uvars, objLowerBound,objUpperBound,count_clause_num)
    # Open file
    file = open("Problem-Round" + str(TotalRound) + "-Active" + str(ActiveSbox) + ".cnf", "w")
    file.write("p cnf " + str(count_var_num) + " " + str(count_clause_num) + "\n")
    # Add constraints
    GenRoundConstrain(TotalRound, xin, p, xout, file)
    GenSequentialEncoding(TotalRound, Wvars, Uvars, objLowerBound,objUpperBound, file)
    file.close()
    time_start = time.time()
    order = "PATH_TO_SAT_SOLVER " + "Problem-Round" + str(TotalRound) + "-Active" + str(ActiveSbox) + ".cnf > Round" + str(TotalRound) + "-Active" + str(ActiveSbox) + "-solution.out"
    os.system(order)
    time_end = time.time()
    # Extracting results
    order = "sed -n '/s SATISFIABLE/p' Round" + str(TotalRound) + "-Active" + str(ActiveSbox) + "-solution.out > SatSolution.out"
    os.system(order)
    order = "sed -n '/s UNSATISFIABLE/p' Round" + str(TotalRound) + "-Active" + str(ActiveSbox) + "-solution.out > UnsatSolution.out"
    os.system(order)
    satsol = open("SatSolution.out")
    unsatsol = open("UnsatSolution.out")
    satresult = satsol.readlines()
    unsatresult = unsatsol.readlines()
    satsol.close()
    unsatsol.close()
    if ((len(satresult) == 0) and (len(unsatresult) > 0)):
        flag = False
    if ((len(satresult) > 0) and (len(unsatresult) == 0)):
        flag = True
    order = "rm SatSolution.out"
    os.system(order)
    order = "rm UnsatSolution.out"
    os.system(order)
    # Removing cnf file
    order = "rm Problem-Round" + str(TotalRound) + "-Active" + str(ActiveSbox) + ".cnf"
    os.system(order)
    # Printing solutions
    file = open("InterMediateResult.out", "a")
    if (flag == True):
        print("Round:" + str(TotalRound) + "; Active: " + str(ActiveSbox) + "; Sat; TotalCost: " + str(time_end - time_start))
        file.write("Round:" + str(TotalRound) + "; Active: " + str(ActiveSbox) + "; Sat; TotalCost: " + str(time_end - time_start) + "\n")
    else:
        print("Round:" + str(TotalRound) + "; Active: " + str(ActiveSbox) + "; Unsat; TotalCost: " + str(time_end - time_start))
        file.write("Round:" + str(TotalRound) + "; Active: " + str(ActiveSbox) + "; Unsat; TotalCost: " + str(time_end - time_start) + "\n")
    file.close();
    return (flag, count_var_num, count_clause_num, time_end - time_start)


if __name__ == '__main__':
    CountSbox = InitialLowerBound
    TotalTimeStart = time.time()
    for totalround in range(SearchRoundStart, SearchRoundEnd):
        Result = "UNSATISFIABLE"
        DiffActiveSbox[totalround] = DiffActiveSbox[totalround - 1] + DiffActiveSbox[1]
        blockweightsize = 32
        while (Result == "UNSATISFIABLE"):
            objLowerBoundList = []
            objUpperBoundList = []
            objLowerBound = []
            objUpperBound = []
            for i in range(totalround*blockweightsize):
                objLowerBoundList.append(i)
                objUpperBoundList.append(i)
            pool = Pool()
            objLowerBound = pool.map(partial(AccurateLowerBound, TOTALROUND = totalround, MatsuiProbabilityBound = DiffActiveSbox, BlockWeightSize = blockweightsize), objLowerBoundList)
            objUpperBound = pool.map(partial(AccurateUpperBound, TOTALROUND = totalround, MatsuiProbabilityBound = DiffActiveSbox, BlockWeightSize = blockweightsize), objUpperBoundList)
            pool.close()
            pool.join()
            if ("infeasible" in objLowerBound) or ("infeasible" in objUpperBound):
                DiffActiveSbox[totalround] += 1
                continue
            (flag, var_num, clause_num, Time) = Decision(totalround, objLowerBound, objUpperBound)
            if (flag == True):
                break
            DiffActiveSbox[totalround] += 1
        file = open("RunTimeSummarise.out", "a")
        resultseq = "Round: " + str(totalround) + "; Active S-box: " + str(DiffActiveSbox[totalround]) + "; Runtime: " + str(Time) + "\n"
        file.write(resultseq)
        file.close()
    print(str(DiffActiveSbox))
    TotalTimeEnd = time.time()
    print("Total Runtime: " + str(TotalTimeEnd - TotalTimeStart))
    file = open("RunTimeSummarise.out", "a")
    resultseq = "Total Runtime: " + str(TotalTimeEnd - TotalTimeStart)
    file.write(resultseq)
