#include <iostream>
#include <omp.h>
#include <sys/time.h>
using namespace std;

class LUD
{
public:
    float **A, **A_dash, *P, **L, **U;
    float **Permutation;
    double total_time;
    int CHUNK_SIZE, NUM_THREADS;
    LUD(int n, int c_size, int n_threads, int seed);
    void Compute_OMP_LUD(int n);
    void Generate_p_Matrix(int n);
    void MatrixMultiply(float **matrix1, float **matrix2, int n);
    void PrintMatrix(float **matrix, int n);
};

LUD::LUD(int n, int cs, int nt, int sd)
{
    CHUNK_SIZE = cs;
    NUM_THREADS = nt;
    int seed = sd;
    A = new float *[n];
    A_dash = new float *[n];
    P = new float[n];
    L = new float *[n];
    U = new float *[n];
    Permutation = new float *[n];

    double start, end;

    omp_set_dynamic(0);               // Explicitly disable dynamic teams
    omp_set_num_threads(NUM_THREADS); // Use NUM_THREADS threads for all consecutive parallel regions

    /*The omp_get_wtime function returns a doubleprecision value equal to the number 
     of seconds since the initial value of the operating system real-time clock.*/
    start = omp_get_wtime();

#pragma omp parallel
    {

        #pragma omp single
        {
            #pragma omp task
        {
            for (int i = 0; i < n; i++)
            {
                A[i] = new float[n]{0};
                A_dash[i] = new float[n];
            }
        }

#pragma omp task
        {
            for (int i = 0; i < n; i++)
            {
                L[i] = new float[n]{0};
                L[i][i] = 1;
            }
        }

#pragma omp task
        {
            for (int i = 0; i < n; i++)
            {
                U[i] = new float[n]{0};
                Permutation[i] = new float[n]{0};
                P[i] = i;
            }
        }
        }
    }

#pragma omp parallel for schedule(static, CHUNK_SIZE)
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

    end = omp_get_wtime();
    total_time += end - start;
}

void LUD::Compute_OMP_LUD(int n)
{
    double start, end;
    start = omp_get_wtime();
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

#pragma omp parallel sections
        {
#pragma omp section
            {
                for (int j = 0; j < n; j++)
                {
                    swap(A[k][j], A[r][j]);
                }
            }
#pragma omp section
            {
                for (int j = 0; j <= k - 1; j++)
                {
                    swap(L[k][j], L[r][j]);
                }
            }
        }

        U[k][k] = A[k][k];

#pragma omp parallel for schedule(static, CHUNK_SIZE)
        for (int j = k + 1; j < n; j++)
        {
            L[j][k] = A[j][k] / U[k][k];
            U[k][j] = A[k][j];
        }

#pragma omp parallel for schedule(static, CHUNK_SIZE)
        for (int i = k + 1; i < n; i++)
        {
            for (int j = k + 1; j < n; j++)
            {
                A[i][j] -= L[i][k] * U[k][j];
            }
        }
    }
    end = omp_get_wtime();
    total_time += end - start;
    Generate_p_Matrix(n);
}

void LUD::Generate_p_Matrix(int n)
{
    double start, end;
    start = omp_get_wtime();
    int k;
#pragma omp parallel for schedule(static, CHUNK_SIZE)
    for (int i = 0; i < n; i++)
    {
        k = P[i];
        Permutation[i][k] = 1;
    }
    end = omp_get_wtime();
    total_time += end - start;
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
    LUD obj(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])); //n,cs,nt,seed
    obj.Compute_OMP_LUD(atoi(argv[1]));
    cout << atoi(argv[3]) << " " << atoi(argv[1]) << " "<< atoi(argv[2]) << " " << obj.total_time << endl;
    //obj.MatrixMultiply(obj.Permutation, obj.A_dash, n);
    //obj.MatrixMultiply(obj.L, obj.U, n);
    // cout<<obj.total_time;
    /* obj.PrintMatrix(obj.A_dash, n);
    
    obj.PrintMatrix(obj.Permutation, n);
    obj.MatrixMultiply(obj.Permutation, obj.A_dash, n);
    obj.MatrixMultiply(obj.L, obj.U, n);
    cout << endl;*/
    return 0;
}
