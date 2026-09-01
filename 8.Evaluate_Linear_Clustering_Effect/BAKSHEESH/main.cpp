#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <vector>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>

#define TestRound 2

using std::vector;
using namespace CMSat;
using namespace std; 

int Constraint[39][9] = {
	{9, 0, 1, 1, 1, 9, 0, 1, 9}, {9, 0, 9, 0, 1, 0, 9, 1, 9}, {0, 9, 9, 9, 9, 0, 0, 0, 1}, {1, 9, 9, 9, 9, 1, 1, 1, 1}, {9, 1, 1, 1, 9, 9, 0, 0, 9}, {9, 1, 1, 0, 9, 0, 9, 0, 9}, {1, 9, 1, 1, 0, 1, 9, 9, 9}, {1, 9, 9, 0, 0, 9, 1, 9, 1}, {9, 1, 0, 1, 1, 1, 0, 9, 9}, {9, 1, 0, 1, 1, 0, 1, 9, 9}, {9, 9, 0, 9, 0, 1, 1, 9, 1}, {9, 9, 0, 0, 0, 1, 9, 0, 9}, {0, 0, 0, 9, 9, 1, 1, 9, 9}, {0, 0, 1, 1, 9, 0, 9, 9, 9}, {9, 1, 1, 1, 9, 9, 1, 1, 9}, {9, 1, 1, 0, 9, 1, 9, 1, 9}, {0, 9, 9, 9, 0, 9, 0, 0, 1}, {0, 9, 9, 9, 0, 0, 9, 0, 1}, {0, 9, 9, 9, 0, 0, 0, 9, 1}, {9, 0, 0, 9, 9, 0, 0, 9, 1}, {1, 0, 9, 9, 9, 1, 1, 9, 1}, {0, 9, 9, 9, 1, 1, 1, 0, 9}, {9, 0, 0, 0, 9, 9, 0, 1, 9}, {1, 9, 9, 9, 0, 1, 9, 1, 1}, {0, 9, 9, 9, 1, 1, 0, 1, 9}, {1, 9, 9, 9, 0, 9, 1, 1, 1}, {0, 9, 9, 9, 1, 0, 1, 1, 9}, {9, 0, 0, 0, 9, 0, 1, 9, 9}, {1, 9, 9, 9, 1, 1, 0, 0, 9}, {1, 9, 9, 9, 1, 0, 1, 0, 9}, {1, 9, 9, 9, 1, 0, 0, 1, 9}, {1, 9, 0, 9, 0, 0, 0, 9, 9}, {0, 0, 9, 0, 9, 9, 0, 9, 1}, {9, 9, 1, 9, 9, 9, 9, 9, 0}, {9, 1, 0, 0, 0, 9, 9, 9, 9}, {9, 9, 9, 9, 0, 0, 0, 0, 1}, {9, 9, 9, 9, 0, 1, 1, 1, 1}, {9, 9, 9, 1, 9, 9, 9, 9, 0}, {9, 9, 9, 9, 1, 9, 9, 9, 0}
};

void GenerateLess(int * x, int ** u, int * vn, int * cn, SATSolver & solver, vector<Lit> & clause)
{
    int n=(*vn);
    int k=(*cn);

    if (k>0)
    {
        clause.clear();
        clause.push_back(Lit(x[0], true));
        clause.push_back(Lit(u[0][0],false));
        solver.add_clause(clause);

        for (int j=1;j<k;j++)
        {
            clause.clear();
            clause.push_back(Lit(u[0][j],true));
            solver.add_clause(clause);
        }
        for (int i=1;i<n-1;i++)
        {
            clause.clear();
            clause.push_back(Lit(x[i],true));
            clause.push_back(Lit(u[i][0],false));
            solver.add_clause(clause);

            clause.clear();
            clause.push_back(Lit(u[i-1][0],true));
            clause.push_back(Lit(u[i][0],false));
            solver.add_clause(clause);

            clause.clear();
            clause.push_back(Lit(x[i],true));
            clause.push_back(Lit(u[i-1][k-1],true));
            solver.add_clause(clause);
        }
        for (int j=1;j<k;j++)
        {
            for (int i=1;i<n-1;i++)
            {
                clause.clear();
                clause.push_back(Lit(x[i],true));
                clause.push_back(Lit(u[i-1][j-1],true));
                clause.push_back(Lit(u[i][j],false));
                solver.add_clause(clause);

                clause.clear();
                clause.push_back(Lit(u[i-1][j],true));
                clause.push_back(Lit(u[i][j],false));
                solver.add_clause(clause);
            }
        }

        clause.clear();
        clause.push_back(Lit(x[n-1],true));
        clause.push_back(Lit(u[n-2][k-1],true));
        solver.add_clause(clause);
    }
    if (k==0)
    {
        for (int i=0;i<n;i++)
        {
            clause.clear();
            clause.push_back(Lit(x[i], true));
            solver.add_clause(clause);
        }
    }
}

string GeneFileName(int round, int weight)
{
    string re = "Round";
    if (round == 0)
    {
        re += "0";
    }
    else
    {
        int len = 0, temp = round;
        while(temp!=0)
        {
            temp /= 10;
            len++;
        }
        temp = round;
        while(len--)
        {
            re += (int)('0'+temp/pow(10,len));
            temp = temp%(int)(pow(10,len)+0.5);
        }
    }
    re += "-Weight";
    if (weight == 0)
    {
        re += "0";
    }
    else
    {
        int len = 0, temp = weight;
        while(temp!=0)
        {
            temp /= 10;
            len++;
        }
        temp = weight;
        while(len--)
        {
            re += (int)('0'+temp/pow(10,len));
            temp = temp%(int)(pow(10,len)+0.5);
        }
    }
    re += ".out";
    return re;
}

int CorrelationBound[26] = {0, 0, 1, 3, 5, 8, 12, 14, 18, 22, 26, 30, 34, 37, 40, 43, 46, 49, 52, 55, 58, 61, 64, 67};

int OptimalInOutMask[22][16] = {
	{0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x8888, 0x7777, 0x7777, 0x7777, 0x7777, 0x7777, 0x7777, 0x7777, 0x7777},
	{0x0, 0xc000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x40, 0x0, 0x0, 0x0, 0x10},
	{0xdc00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x2, 0x0, 0x4, 0x0, 0x0},
	{0x0, 0x0, 0x0, 0xca00, 0x0, 0x0, 0x0, 0x0, 0x800, 0x0, 0x101, 0x0, 0x202, 0x0, 0x404, 0x0},
	{0x0, 0x0, 0x0, 0x0, 0x0, 0xa00c, 0x0, 0x0, 0x10, 0x4050, 0x20, 0x8020, 0x40, 0x1050, 0x80, 0x2020},
	{0x0, 0x0, 0x90c, 0x90a, 0x0, 0x0, 0x0, 0x0, 0x14, 0x4, 0x20, 0x0, 0x41, 0x4000, 0x2, 0x8002},
	{0xca0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x44, 0x0, 0x0, 0x11, 0x11, 0x22, 0x22, 0x44},
	{0x90c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x3000, 0x60, 0x2000, 0x40, 0x0, 0x10, 0x1000},
	{0x0, 0x0, 0x0, 0x90c, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x110, 0x1111, 0x0, 0x2222, 0x400, 0x4444},
	{0x0, 0x0, 0x0, 0xa00c, 0x0, 0x0, 0x0, 0x0, 0x10, 0x1300, 0x100, 0x2600, 0x200, 0x4400, 0x80, 0x100},
	{0x0, 0xa00c, 0x0, 0x90c, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2300, 0x8, 0x4600, 0x81, 0x400, 0x10, 0x1100},
	{0x20c0, 0x0, 0xa000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x40, 0x50, 0x8000, 0x20},
	{0x900, 0x0, 0x17, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20},
	{0x900, 0x0, 0xf060, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x2020, 0x20, 0x40, 0x50, 0x0, 0x20},
	{0x900, 0x0, 0x17, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x40, 0x50, 0x8000, 0x20},
	{0x900, 0x0, 0x17, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20},
	{0x10a0, 0x0, 0xa000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20},
	{0x10a0, 0x0, 0xd0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20},
	{0x10c0, 0x0, 0x90, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x2020, 0x20, 0x40, 0x50, 0x0, 0x20},
	{0x900, 0x0, 0x10a0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20},
	{0x900, 0x0, 0x4a, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20},
	{0x900, 0x0, 0x10a0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1010, 0x50, 0x20, 0x20, 0x4040, 0x50, 0x0, 0x20}
};

int main()
{
	clock_t start, end_time;
    start=clock();

    for (int trailround = TestRound; trailround < (TestRound + 1); trailround++)
    {
        for (int addweight = 0; addweight < 10; addweight++)
        {
            int LowestWeight = CorrelationBound[trailround] + addweight;

            int ** xin = new int * [trailround];
            int ** p = new int * [trailround];
            int ** xout = new int * [trailround];
            
            for (int round = 0; round < trailround; round++)
            {
                xin[round] = new int [128];
                p[round] = new int [32];
                xout[round] = new int [128];
            }

            // Allocate variable
            int sx = 0;
            for (int round = 0; round < trailround; round++)
            {
                for (int i = 0; i < 128; i++)
                {
                    xin[round][i] = sx++;
                }
                for (int i = 0; i < 32; i++)
                {
                    p[round][i] = sx++;
                }
            }
            for (int round = 0; round < trailround - 1; round++)
            {
                for (int i = 0; i < 128; i++)
                {
                    xout[round][i] = xin[round + 1][i];
                }
            }
            for (int i = 0; i < 128; i++)
            {
                xout[trailround - 1][i] = sx++;
            }

            int TotalProb = 32 * trailround;
            int Prob = LowestWeight;
            
            int ** UP = new int * [TotalProb - 1];
            for (int i = 0; i < TotalProb - 1; i++)
            {
                UP[i] = new int [Prob];
            }
            for (int i = 0; i < TotalProb - 1; i++)
            {
                for (int j = 0; j < Prob; j++)
                {
                    UP[i][j] = sx++;
                }
            }

            SATSolver solver;
            vector<Lit> clause;
            solver.set_num_threads(1);
            solver.new_vars(sx);

            // Set input difference
            for (int group = 0; group < 8; group++)
            {
                for (int bit = 0; bit < 16; bit++)
                {
                    if ((OptimalInOutMask[trailround - 1][group] >> (15 - bit)) & (0x1))
                    {
                        clause.clear();
                        clause.push_back(Lit(xin[0][16 * group + bit], false));
                        solver.add_clause(clause);
                    }
                    else 
                    {
                        clause.clear();
                        clause.push_back(Lit(xin[0][16 * group + bit], true));
                        solver.add_clause(clause);
                    }
                }
            }

            // Set input difference
            for (int group = 0; group < 8; group++)
            {
                for (int bit = 0; bit < 16; bit++)
                {
                    if ((OptimalInOutMask[trailround - 1][group + 8] >> (15 - bit)) & (0x1))
                    {
                        clause.clear();
                        clause.push_back(Lit(xout[trailround - 1][16 * group + bit], false));
                        solver.add_clause(clause);
                    }
                    else
                    {
                        clause.clear();
                        clause.push_back(Lit(xout[trailround - 1][16 * group + bit], true));
                        solver.add_clause(clause);
                    }
                }
            }

            for (int round = 0; round < trailround; round++)
            {
                int y[128];
                int P[128] = {96, 65, 34, 3, 64, 33, 2, 99, 32, 1, 98, 67, 0, 97, 66, 35, 100, 69, 38, 7, 68, 37, 6, 103, 36, 5, 102, 71, 4, 101, 70, 39, 104, 73, 42, 11, 72, 41, 10, 107, 40, 9, 106, 75, 8, 105, 74, 43, 108, 77, 46, 15, 76, 45, 14, 111, 44, 13, 110, 79, 12, 109, 78, 47, 112, 81, 50, 19, 80, 49, 18, 115, 48, 17, 114, 83, 16, 113, 82, 51, 116, 85, 54, 23, 84, 53, 22, 119, 52, 21, 118, 87, 20, 117, 86, 55, 120, 89, 58, 27, 88, 57, 26, 123, 56, 25, 122, 91, 24, 121, 90, 59, 124, 93, 62, 31, 92, 61, 30, 127, 60, 29, 126, 95, 28, 125, 94, 63};
                for (int i = 0; i < 128; i++)
                {
                    y[i] = xout[round][P[i]];
                }

                for (int sbox = 0; sbox < 32; sbox++)
                {
                    int X[9];
                    for (int i = 0; i < 4; i++)
                    {
                        X[i] = xin[round][4 * sbox + i];
                        X[i + 4] = y[4 * sbox + i];
                    }
                    X[8] = p[round][sbox];

                    for (int restriction = 0; restriction < 39; restriction++)
                    {
                        clause.clear();
                        for (int i = 0; i < 9; i++)
                        {
                            if (Constraint[restriction][i] == 1)
                            {
                                clause.push_back(Lit(X[i], true));
                            }
                            if (Constraint[restriction][i] == 0)
                            {
                                clause.push_back(Lit(X[i], false));
                            }
                        }
                        solver.add_clause(clause);
                    }
                }
            }

            int * AddProb = new int [TotalProb];
            for (int round = 0; round < trailround; round++)
            {
                for (int i = 0; i < 32; i++)
                {
                    AddProb[32 * round + i] = p[trailround - 1 - round][i];
                }
            }

            int NumProb = TotalProb;
            int ConsProb = Prob;
            GenerateLess(AddProb, UP, &NumProb, &ConsProb, solver, clause);

            // Find multiple solutions
            lbool ret = l_True;
            int Solution = 0;
            
            int trailweight = LowestWeight;
            string outputfilename = GeneFileName(trailround, trailweight);
            const char * OutputFileName = outputfilename.c_str();
            ofstream fout(OutputFileName);

            while (ret == l_True)
            {
                ret = solver.solve();
                if (ret == l_True)
                {
                    Solution += 1;
                    // Fout solution here
                    fout<<"Solution: "<<(dec)<<Solution<<" ================================= "<<"\n";
                    for (size_t round = 0; round < trailround; round++)
                    {
                        fout<<"Round: "<<(dec)<<round<<" ---------------------------- "<<"\n";
                        int tem[128];
                        fout<<"xin: "<<"\n";
                        for (size_t i = 0; i < 128; i++)
                        {
                            if (solver.get_model()[xin[round][i]]!=l_Undef)
                            {
                                if (solver.get_model()[xin[round][i]]==l_True)
                                {
                                    tem[i]=1;
                                }
                                else
                                {
                                    tem[i]=0;
                                }
                            }
                        }
                        for (int sbox = 0; sbox < 32; sbox++)
                        {
                            int a = 0;
                            for (int i = 0; i < 4; i++)
                            {
                                a ^= (tem[4 * sbox + i] << (3 - i));
                            }
                            fout<<(hex)<<a<<",";
                            if (((sbox + 1)%4) == 0)
                            {
                                fout<<" ";
                            }
                        }
                        fout<<"\n";
                        
                        fout<<"xout: "<<"\n";
                        for (size_t i = 0; i < 128; i++)
                        {
                            if (solver.get_model()[xout[round][i]]!=l_Undef)
                            {
                                if (solver.get_model()[xout[round][i]]==l_True)
                                {
                                    tem[i]=1;
                                }
                                else
                                {
                                    tem[i]=0;
                                }
                            }
                        }
                        for (int sbox = 0; sbox < 32; sbox++)
                        {
                            int a = 0;
                            for (int i = 0; i < 4; i++)
                            {
                                a ^= (tem[4 * sbox + i] << (3 - i));
                            }
                            fout<<(hex)<<a<<",";
                            if (((sbox + 1)%4) == 0)
                            {
                                fout<<" ";
                            }
                        }
                        fout<<"\n";
                    }
                    // ---------------------
                    // Delete solution
                    clause.clear();
                    for (size_t round = 0; round < trailround; round++)
                    {
                        for (size_t i = 0; i < 128; i++)
                        {
                            if (solver.get_model()[xin[round][i]] != l_Undef)
                            {
                                clause.push_back(Lit(xin[round][i], solver.get_model()[xin[round][i]] == l_True));
                            }
                        }
                    }
                    for (size_t i = 0; i < 128; i++)
                    {
                        if (solver.get_model()[xout[trailround - 1][i]]!=l_Undef)
                        {
                            clause.push_back(Lit(xout[trailround - 1][i], solver.get_model()[xout[trailround - 1][i]] == l_True));
                        }
                    }
                    solver.add_clause(clause);
                }
                else if (ret == l_False)
                {
                    //cout<<"No more solutions."<<endl;
                }
                else
                {
                    cout<<"Solver returned that it didn't finish running."<<endl;
                }
            }

            cout<<"Round: " << (dec) << trailround << "; Weight: " << LowestWeight << "; Number of Solution: "<<(dec)<<Solution << endl;

            fout.close();

            // Delete dynamic variable
            for (int round = 0; round < trailround; round++)
            {
                delete [] xin[round];
                delete [] p[round];
                delete [] xout[round];
            }
            delete [] xin;
            delete [] p;
            delete [] xout;
            for (int i = 0; i < TotalProb - 1; i++)
            {
                delete [] UP[i];
            }
            delete [] UP;
            delete [] AddProb;
        }
    }

    end_time=clock();
    std::cout<<"Run time: "<<(double)(end_time - start) / CLOCKS_PER_SEC<<"S"<<std::endl;

    return 0;
}
