#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <string.h>

#define TotalCandidateNum 4864

using namespace std;

int S[16];

int TotalSbox[TotalCandidateNum][16];

int DiffTable[16][16];
int LinearTable[16][16];

void S_fun(int *in, int * out)
{
    int a=(*in);
    (*out)=S[a];
}

void GenerateTable()
{
    for (int i=0;i<16;i++)
    {
        for (int j=0;j<16;j++)
        {
            DiffTable[i][j]=0;
        }
    }
    
    int InDiff;
    int In1,In2;
    int Out1,Out2;
    int OutDiff;
    
    for (int i=0;i<16;i++)
    {
        InDiff=i;
        for (int j=0;j<16;j++)
        {
            In1=j;
            In2=(In1^InDiff);
            S_fun(&In1,&Out1);
            S_fun(&In2,&Out2);
            OutDiff=(Out1^Out2);
            DiffTable[InDiff][OutDiff]+=1;
        }
    }
}

void GenerateLinearTable()
{
    for (int i=0;i<16;i++)
    {
        for (int j=0;j<16;j++)
        {
            LinearTable[i][j]=0;
        }
    }
    
    int InMask;
    int OutMask;
    
    int In,Out;
    for (int i=0;i<16;i++)
    {
        for (int j=0;j<16;j++)
        {
            InMask=i;
            OutMask=j;
            int Parity;
            for (int k=0;k<16;k++)
            {
                In=k;
                S_fun(&In,&Out);
                Parity=0;
                for (int l=0;l<4;l++)
                {
                    Parity^=((In>>l)&(InMask>>l)&1);
                    Parity^=((Out>>l)&(OutMask>>l)&1);
                }
                if (Parity==0)
                {
                    LinearTable[i][j]+=1;
                }
            }
        }
    }
    
    for (int i=0;i<16;i++)
    {
        for (int j=0;j<16;j++)
        {
            LinearTable[i][j]-=8;
            if (LinearTable[i][j]<0)
            {
                LinearTable[i][j]=0-LinearTable[i][j];
            }
        }
    }
}

int main()
{
    ifstream fin("Candidate4864.out");
    for (int i = 0; i < TotalCandidateNum; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            fin >> (hex) >> TotalSbox[i][j];
        }
    }
    fin.close();

    ofstream fout("DDTLAT.out");
    for (int index = 0; index < TotalCandidateNum; index++)
    {
        for (int i = 0; i < 16; i++)
        {
            S[i] = TotalSbox[index][i];
        }
        GenerateTable();
        GenerateLinearTable();

        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                fout << DiffTable[i][j] << " ";
            }
        }
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                fout << LinearTable[i][j] << " ";
            }
        }
        fout << "\n";
    }
    fout.close();

    return 0;
}