#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <vector>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>

#define TrailRound 8

using std::vector;
using namespace CMSat;
using namespace std;

int ConsDistribution[44] = {30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 31, 31, 31, 30, 30, 31, 31, 31, 30, 31, 30, 31, 30, 31, 29, 29, 30, 31, 32, 32, 30, 31, 32, 32, 32, 31, 31, 31, 30, 31};

int AllConstraint[44][32][8];

string GenLatexFileName(int part)
{
    string re = "DDTOut";
    if (part == 0)
    {
        re += "0";
    }
    else
    {
        int len = 0, temp = part;
        while(temp!=0)
        {
            temp /= 10;
            len++;
        }
        temp = part;
        while(len--)
        {
            re += (int)('0'+temp/pow(10,len));
            temp = temp%(int)(pow(10,len)+0.5);
        }
    }
    re += ".out";
    return re;
}

int BAKPermutation[128] = {96, 65, 34, 3, 64, 33, 2, 99, 32, 1, 98, 67, 0, 97, 66, 35, 100, 69, 38, 7, 68, 37, 6, 103, 36, 5, 102, 71, 4, 101, 70, 39, 104, 73, 42, 11, 72, 41, 10, 107, 40, 9, 106, 75, 8, 105, 74, 43, 108, 77, 46, 15, 76, 45, 14, 111, 44, 13, 110, 79, 12, 109, 78, 47, 112, 81, 50, 19, 80, 49, 18, 115, 48, 17, 114, 83, 16, 113, 82, 51, 116, 85, 54, 23, 84, 53, 22, 119, 52, 21, 118, 87, 20, 117, 86, 55, 120, 89, 58, 27, 88, 57, 26, 123, 56, 25, 122, 91, 24, 121, 90, 59, 124, 93, 62, 31, 92, 61, 30, 127, 60, 29, 126, 95, 28, 125, 94, 63};

int main()
{
	for (int index = 0; index < 44; index++)
	{
		string latexfilename = GenLatexFileName(index);
        const char * LatexFileName = latexfilename.c_str();
        ifstream fin(LatexFileName);
        for (int i = 0; i < ConsDistribution[index]; i++)
        {
        	for (int j = 0; j < 8; j++)
        	{
        		fin >> AllConstraint[index][i][j];
        	}
        }
        fin.close();
	}

	ofstream fout("ImpossibleDifferentialTrail.out");

	for (int index = 0; index < 44; index++)
	{
		int xin[TrailRound][128];
    	int xout[TrailRound][128];

    	// Allocate variables
	    int sx = 0;
	    for (int round = 0; round < TrailRound; round++)
	    {
	    	for (int i = 0; i < 128; i++)
	        {
	            xin[round][i] = sx++;
	        }
	    }

	    for (int round = 0; round < TrailRound - 1; round++)
	    {
	        for (int i = 0; i < 128; i++)
	        {
	            xout[round][i] = xin[round + 1][i];
	        }
	    }

	    for (int i = 0; i < 128; i++)
	    {
	        xout[TrailRound - 1][i] = sx++;
	    }

	    SATSolver solver;
	    vector<Lit> clause;
	    solver.set_num_threads(1);
	    solver.new_vars(sx);

	    for (int round = 0; round < TrailRound; round++)
	    {
	    	int temp[128];
	    	for (int i = 0; i < 128; i++)
	    	{
	    		temp[i] = xout[round][BAKPermutation[i]];
	    	}
	    	for (int sbox = 0; sbox < 32; sbox++)
	    	{
	    		int X[8];
	    		for (int i = 0; i < 4; i++)
	    		{
	    			X[i] = xin[round][4 * sbox + i];
	    			X[i + 4] = temp[4 * sbox + i];
	    		}

	    		for (int restriction = 0; restriction < ConsDistribution[index]; restriction++)
	    		{
	    			clause.clear();
    				for (int i = 0; i < 8; i++)
    				{
    					if (AllConstraint[index][restriction][i] == 1)
    					{
    						clause.push_back(Lit(X[i], true));
    					}
    					if (AllConstraint[index][restriction][i] == 0)
    					{
    						clause.push_back(Lit(X[i], false));
    					}
    				}
    				solver.add_clause(clause);
	    		}
	    	}
	    }

	    vector<Lit> assumptions;

	    int InputDiff[128];
	    int OutputDiff[128];
	    
	    int Counter = 0;

	    for (int inputposition = 0; inputposition < 128; inputposition++)
	    {
	        for (int outputposition = 0; outputposition < 128; outputposition++)
	        {
	        	for (int i = 0; i < 128; i++)
	            {
	                InputDiff[i] = 0;
	                OutputDiff[i] = 0;
	            }
	            InputDiff[inputposition] = 1;
	            OutputDiff[outputposition] = 1;

	            assumptions.clear();
	            for (int i = 0; i < 128; i++)
	            {
	                if (InputDiff[i])
	                {
	                    assumptions.push_back(Lit(xin[0][i], false));
	                }
	                else
	                {
	                    assumptions.push_back(Lit(xin[0][i], true));
	                }
	            }
	            for (int i = 0; i < 128; i++)
	            {
	                if (OutputDiff[i])
	                {
	                    assumptions.push_back(Lit(xout[TrailRound - 1][i], false));
	                }
	                else
	                {
	                    assumptions.push_back(Lit(xout[TrailRound - 1][i], true));
	                }
	            }
	            lbool ret = solver.solve(&assumptions);

	            if (ret == l_False)
	            {
	                Counter += 1;
	                //fout<<inputposition<<", "<<outputposition<<"\n";
	            }
	        }
	    }
	    fout<<"The number of impossible differential trails: "<<Counter<<"\n";
	}

	fout.close();
	return 0;
}