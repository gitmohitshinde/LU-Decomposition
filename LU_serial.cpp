#include <iostream>
#include <sys/time.h>
using namespace std;

class LUD
{
public:
    float **A, **A_dash, *P, **L, **U;
    float **Permutation;
    double total_time;
    LUD(int n);
    void Compute_Serial_LUD(int n);
    void Generate_p_Matrix(int n);
    int Verify(int n);
    void MatrixMultiply(float **matrix1, float **matrix2, int n);

    void PrintMatrix(float **matrix, int n);
};

LUD::LUD(int n)
{
    int seed = 121;
    A = new float *[n];
    A_dash = new float *[n];
    P = new float[n];
    L = new float *[n];
    U = new float *[n];
    Permutation = new float *[n];

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < n; i++)
    {
        A[i] = new float[n];
        A_dash[i] = new float[n];
        L[i] = new float[n];
        U[i] = new float[n];
        Permutation[i] = new float[n];
        P[i] = i;
        for (int j = 0; j < n; j++)
        {
            if (i > j)
                U[i][j] = 0;
            else if (i == j)
                L[i][j] = 1;
            else
                L[i][j] = 0;
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            seed = (seed % 65536);
            seed = ((3125 * seed) % 65536);
            A[i][j] = int((seed / 65536.0) * ((i + 1) * 100));
            A_dash[i][j] = A[i][j];
        }
    }

    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1e6;
    total_time = (total_time + (end.tv_usec - 
                              start.tv_usec)) * 1e-6;
}

void LUD::Compute_Serial_LUD(int n)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int k = 0; k < n; k++)
    {
        int max = 0, r = 0;
        for (int i = k; i < n; i++)
        {
            if (max < abs(A[i][k]))
            {
                max = abs(A[i][k]);
                r = i;
            }
        }
        if (max == 0)
        {
            cout << "Matrix is singular";
            return;
        }
        swap(P[k], P[r]);
        for (int j = 0; j < n; j++)
        {
            swap(A[k][j], A[r][j]);
        }
        for (int j = 0; j <= k - 1; j++)
        {
            swap(L[k][j], L[r][j]);
        }
        U[k][k] = A[k][k];
        for (int j = k + 1; j < n; j++)
        {
            L[j][k] = A[j][k] / U[k][k];
            U[k][j] = A[k][j];
        }
        for (int i = k + 1; i < n; i++)
        {
            for (int j = k + 1; j < n; j++)
            {
                A[i][j] -= L[i][k] * U[k][j];
            }
        }
    }
    Generate_p_Matrix(n);
    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1e6;
    total_time = (total_time + (end.tv_usec - 
                              start.tv_usec)) * 1e-6;
}

void LUD::Generate_p_Matrix(int n)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j == P[i])
                Permutation[i][j] = 1;
            else
                Permutation[i][j] = 0;
        }
    }
    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1e6;
    total_time = (total_time + (end.tv_usec - 
                              start.tv_usec)) * 1e-6;
}

void LUD::PrintMatrix(float **matrix, int n)
{
    string dash = "----------------";
    for (int i = 0; i < n; i++)
    {
        cout << endl;
        for (int j = 0; j < n; j++)
        {
            if (j != 0)
                cout << " |\t";
            else
                cout << "\t";
            cout << matrix[i][j] << "\t";
        }
        if (i != n - 1)
        {
            cout << endl;
            for (int i = 0; i < n; i++)
            {
                cout << dash;
            }
        }
    }
    cout << "\n\n";
}

void LUD::MatrixMultiply(float **matrix1, float **matrix2, int n)
{
    float **result;
    result = new float *[n];
    for (int h = 0; h < n; h++)
    {
        result[h] = new float[n];
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            result[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    PrintMatrix(result, n);
}

int main(int argc, char const *argv[])
{
    int n = atoi(argv[1]);
    LUD obj(n);
    obj.Compute_Serial_LUD(n);
    cout << obj.total_time;
    // cout<<obj.total_time;
    /* obj.PrintMatrix(obj.A_dash, n);
    
    obj.PrintMatrix(obj.Permutation, n);
    cout<<obj.Verify(n);
    obj.MatrixMultiply(obj.Permutation, obj.A_dash, n);
    obj.MatrixMultiply(obj.L, obj.U, n);
    cout << obj.Verify(n);
    cout << endl;*/
    return 0;
}
