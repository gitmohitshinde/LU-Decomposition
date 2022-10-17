#include <iostream>
#include <sys/time.h>
#include <pthread.h>
using namespace std;

int seed = 121;
float **A, **A_dash, *P, **L, **U;
float **Permutation;
double total_time;
int NUM_THREADS, N, thread_count = 0;
pthread_mutex_t lock;

void *Init1(void *args1);
void *Init2(void *args1);
void *Pthread_swap(void *args1);
void *loop1(void *args1);
void *loop2(void *args1);
void *permute(void *args1);
void PrintMatrix(float **matrix, int n);
void MatrixMultiply(float **matrix1, float **matrix2, int n);

struct argslist
{
    int first, second, third;
};
struct argslist *p = new struct argslist();

int main(int argc, char const *argv[])
{
    NUM_THREADS = atoi(argv[2]);
    N = atoi(argv[1]);
    pthread_t thread[NUM_THREADS];

    struct timeval start, end;
    gettimeofday(&start, NULL);

    A = new float *[N];
    A_dash = new float *[N];
    P = new float[N];
    L = new float *[N];
    U = new float *[N];
    Permutation = new float *[N];

    //Initializing data
    int ret_val;
    for (int i = 0; i < 3; i++)
    {
        ret_val = pthread_create(&thread[i], NULL, Init1, NULL);
        if (ret_val) /* could not create thread */
        {
            printf("\n ERROR: return code from pthread_create is %d \n", ret_val);
            exit(1);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(thread[i], NULL);
    }
    thread_count = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        ret_val = pthread_create(&thread[i], NULL, Init2, NULL);
        if (ret_val) /* could not create thread */
        {
            printf("\n ERROR: return code from pthread_create is %d \n", ret_val);
            exit(1);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }
    thread_count = 0;
    //initializaiton done

    //computing LU decomposition
    for (int k = 0; k < N; k++)
    {
        int max = 0, r = 0;
        for (int i = k; i < N; i++)
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
            return -1;
        }
        swap(P[k], P[r]);

        //performing parallel swaps
        p->first = k;
        p->second = r;
        for (int i = 0; i < 2; i++)
        {
            ret_val = pthread_create(&thread[i], NULL, Pthread_swap, (void *)p);
            if (ret_val) /* could not create thread */
            {
                printf("\n ERROR: return code from pthread_create is %d \n", ret_val);
                exit(1);
            }
        }
        for (int i = 0; i < 2; i++)
        {
            pthread_join(thread[i], NULL);
        }
        thread_count = 0;
        // swapping done;

        U[k][k] = A[k][k];

        //parallel loop-1
        p->first = k;
        p->third = 1;

        for (int i = 0; i < NUM_THREADS; i++)
        {
            ret_val = pthread_create(&thread[i], NULL, loop1, (void *)p);
            if (ret_val) /* could not create thread */
            {
                printf("\n ERROR: return code from pthread_create is %d \n", ret_val);
                exit(1);
            }
        }
        for (int i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(thread[i], NULL);
        }
        thread_count = 0;
        //parallel loop-1 done

        //last parallel loop
        for (int i = 0; i < NUM_THREADS; i++)
        {
            ret_val = pthread_create(&thread[i], NULL, loop2, (void *)p);
            if (ret_val) /* could not create thread */
            {
                printf("\n ERROR: return code from pthread_create is %d \n", ret_val);
                exit(1);
            }
        }
        for (int i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(thread[i], NULL);
        }
        thread_count = 0;
        //last parallel loop done
    }

    //Generating Permutation Matrix
    p->first = -1;
    p->third = 2;
    {
        for (int i = 0; i < NUM_THREADS; i++)
        {
            ret_val = pthread_create(&thread[i], NULL, loop1, (void *)p);
            if (ret_val) /* could not create thread */
            {
                printf("\n ERROR: return code from pthread_create is %d \n", ret_val);
                exit(1);
            }
        }
        for (int i = 0; i < NUM_THREADS; i++)
        {
            pthread_join(thread[i], NULL);
        }
        thread_count = 0;
    }

    gettimeofday(&end, NULL);
    total_time = (end.tv_sec - start.tv_sec) * 1e6;
    total_time = (total_time + (end.tv_usec - 
                              start.tv_usec)) * 1e-6;

    cout<<N<<" "<<NUM_THREADS<<" "<<total_time<<endl;

    //Permutaion Matrix Generated
    //MatrixMultiply(Permutation, A_dash, N);
    //MatrixMultiply(L, U, N);
    return 0;
}

void *Init1(void *args1)
{
    int th_no;
    pthread_mutex_lock(&lock);
    th_no = thread_count;
    thread_count++;
    pthread_mutex_unlock(&lock);

    if (th_no == 0)
    {
        for (int i = 0; i < N; i++)
        {
            A[i] = new float[N]{0};
            A_dash[i] = new float[N];
        }
    }

    else if (th_no == 1)
    {
        for (int i = 0; i < N; i++)
        {
            L[i] = new float[N]{0};
            L[i][i] = 1;
        }
    }

    else if (th_no == 2)
    {
        for (int i = 0; i < N; i++)
        {
            U[i] = new float[N]{0};
            Permutation[i] = new float[N]{0};
            P[i] = i;
        }
    }
    pthread_exit(NULL);
}

void *Init2(void *args1)
{
    int th_no;
    int start, end;
    pthread_mutex_lock(&lock);
    th_no = thread_count;
    thread_count++;
    pthread_mutex_unlock(&lock);

    int delta = N / NUM_THREADS;
    start = delta * th_no;
    if (th_no < NUM_THREADS - 1)
        end = delta * (th_no + 1) - 1;
    else
        end = N - 1;

    // 1 2 3 4 5 6 7 8 9 10 11 12
    for (int i = start; i <= end; i++)
    {
        for (int j = 0; j < N; j++)
        {
            seed = (seed % 65536);
            seed = ((3125 * seed) % 65536);
            A[i][j] = int((seed / 65536.0) * ((i + 1) * 100));
            A_dash[i][j] = A[i][j];
        }
    }

    pthread_exit(NULL);
}

void *Pthread_swap(void *args1)
{
    struct argslist *args = (struct argslist *)args1;
    int k = args->first;
    int r = args->second;
    int th_no;
    pthread_mutex_lock(&lock);
    th_no = thread_count;
    thread_count++;
    pthread_mutex_unlock(&lock);

    if (th_no == 0)
    {
        for (int j = 0; j < N; j++)
        {
            swap(A[k][j], A[r][j]);
        }
    }

    else if (th_no == 1)
    {
        for (int j = 0; j <= k - 1; j++)
        {
            swap(L[k][j], L[r][j]);
        }
    }

    pthread_exit(NULL);
}

void *loop1(void *args1)
{
    struct argslist *args = (struct argslist *)args1;
    int th_no;
    int start, end;
    int k = args->first;
    int size = N - k - 1;
    pthread_mutex_lock(&lock);
    th_no = thread_count;
    thread_count++;
    pthread_mutex_unlock(&lock);

    int delta = size / NUM_THREADS;
    start = delta * th_no + k + 1;
    if (th_no < NUM_THREADS - 1)
        end = delta * (th_no + 1) - 1 + k + 1;
    else
        end = N - 1;

    if (start <= end && args->third == 1)
    {
        for (int j = start; j <= end; j++)
        {
            L[j][k] = A[j][k] / U[k][k];
            U[k][j] = A[k][j];
        }
    }
    else if (start <= end && args->third == 2)
    {
        for (int i = start; i <= end; i++)
        {
            int k = P[i];
            Permutation[i][k] = 1;
        }
    }

    pthread_exit(NULL);
}

void *loop2(void *args1)
{
    struct argslist *args = (struct argslist *)args1;
    int th_no;
    int start, end;
    int k = args->first;
    int size = N - k - 1;
    pthread_mutex_lock(&lock);
    th_no = thread_count;
    thread_count++;
    pthread_mutex_unlock(&lock);

    int delta = size / NUM_THREADS;
    start = delta * th_no + k + 1;
    if (th_no < NUM_THREADS - 1)
        end = delta * (th_no + 1) - 1 + k + 1;
    else
        end = N - 1;

    if (start <= end)
    {
        for (int i = start; i <= end; i++)
        {
            for (int j = k + 1; j < N; j++)
            {
                A[i][j] -= L[i][k] * U[k][j];
            }
        }
        pthread_mutex_unlock(&lock);
    }

    pthread_exit(NULL);
}

void PrintMatrix(float **matrix, int n)
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

void MatrixMultiply(float **matrix1, float **matrix2, int n)
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