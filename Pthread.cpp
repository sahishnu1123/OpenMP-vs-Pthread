#include<iostream>
#include<fstream>
#include<pthread.h>
#include<chrono>

using namespace std;
using namespace std::chrono;

struct t{

    int *args;
    int index;
};

struct ret{

    int index;
    int rcg;
    int rcgnum;
    int validity;
    int count;
};

typedef struct ret* retp;
typedef struct t* tpass;

int N,k;

int sqroot(int num){

    int i = 0;

    while(i*i < num){

        i++;
    }

    if(i*i == num){

        return i;
    }

    else return -1;
}

int check_rows(int *args, int rnum){

    int hasharr[N];

    for(int i = 0; i < N; i++){

        hasharr[i] = 0;
    }

    for(int i = 0; i < N; i++){

        hasharr[args[(rnum*N) + i] - 1] += 1;
    }

    for(int i = 0; i < N; i++){

        if(hasharr[i] != 1){

            return -1;
        }
    }

    return 0;
}

int check_columns(int *args, int cnum){

    int hasharr[N];

    for(int i = 0; i < N; i++){

        hasharr[i] = 0;
    }

    for(int i = 0; i < N; i++){

        hasharr[args[cnum + (i*N)] - 1] += 1;
    }

    for(int i = 0; i < N; i++){

        if(hasharr[i] != 1){

            return -1;
        }
    }

    return 0;
}

int check_grids(int *args, int gnum){

    int hasharr[N];

    for(int i = 0; i < N; i++){

        hasharr[i] = 0;
    }
    
    int n = sqroot(N);

    int r = gnum/n;
    int c = gnum%n;

    for(int i = 0; i < n; i++){

        for(int j = 0; j < n; j++){

            hasharr[args[((r*n + i) * N) + (c*n + j)] - 1] += 1;
        }
    }

    for(int i = 0; i < N; i++){

        if(hasharr[i] != 1){

            return -1;
        }
    }

    return 0;

}

void *thread_func(void *thread_pass){

    tpass a = (tpass) thread_pass;

    int i = a->index;
    int *args = a->args;

    int count = 0;
    int check = k*count + i;
    int checkd = k*count + i;

    int *ret = (int *)malloc(sizeof(int));
    *ret = 1;

    int countd = 0;

    while(checkd < 3*N){

        checkd+=k;
        countd++;
    }

    retp *retvar = (retp *)malloc(sizeof(retp) * countd);

    for(int i = 0; i < countd; i++){

        retvar[i] = (retp)malloc(sizeof(struct ret));
    }

    while(check < 3*N){

        int q = check/N;

        if(q == 0){

            retvar[count]->index = i;
            retvar[count]->rcg = 0;
            retvar[count]->rcgnum = (k*count + i)%N;
            retvar[count]->validity = 1;
            retvar[count]->count = countd;

            if(check_rows(args, (k*count + i)%N) != 0){

                retvar[count]->validity = 0;
                return (void *)retvar;
            }
        }

        else if(q == 1){

            retvar[count]->index = i;
            retvar[count]->rcg = 1;
            retvar[count]->rcgnum = (k*count + i)%N;
            retvar[count]->validity = 1;
            retvar[count]->count = countd;

            if(check_columns(args, (k*count + i)%N) != 0){

                retvar[count]->validity = 0;
                return (void *)retvar;
            }
        }

        else if(q == 2){

            retvar[count]->index = i;
            retvar[count]->rcg = 2;
            retvar[count]->rcgnum = (k*count + i)%N;
            retvar[count]->validity = 1;
            retvar[count]->count = countd;

            if(check_grids(args, (k*count + i)%N) != 0){

                retvar[count]->validity = 0;
                return (void *)retvar;
            }
        }

        check += k;
        count++;
    }

    return (void *)retvar;
}

int main(int argc, char* argv[]){

    ifstream ipfile;

    ipfile.open(argv[1]);

    ipfile>>k;
    ipfile>>N;

    int args[N*N];

    for(int i = 0; i < N*N; i++){

        ipfile>>args[i];
    }

    ipfile.close();

    if(sqroot(N) == -1){

        cout<<"Not Valid dimension";

        return 1;
    }

    pthread_t tid[k];
    void* ret[k];
    retp *retvar[k];

    auto start = high_resolution_clock::now();

    for(int i = 0; i < k; i++){

        tpass t_pass = (tpass)malloc(sizeof(struct t));

        t_pass->index = i;
        t_pass->args = args;

        pthread_create(&tid[i], NULL, &thread_func, (void *)t_pass);
    }

    for(int i = 0; i < k; i++){

        pthread_join(tid[i], &ret[i]);
    }

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    for(int i = 0; i < k; i++){

        retvar[i] = (retp *)ret[i];
    }

    int br = 1;

    ofstream ofile;

    ofile.open("output.txt");

    for(int i = 0; i < k; i++){

        int count = retvar[i][0]->count;

        for(int x = 0; x < count; x++){

            int val = retvar[i][x]->validity;
            int rcg = retvar[i][x]->rcg;
            int rcgnum = retvar[i][x]->rcgnum;

            if(val == 1 && rcg == 0){

                ofile<<"Thread "<<i<<" checks row "<<rcgnum<<" and is valid.\n\n";
            }

            else if(val == 1 && rcg == 1){

                ofile<<"Thread "<<i<<" checks column "<<rcgnum<<" and is valid.\n\n";
            }

            else if(val == 1 && rcg == 2){

                ofile<<"Thread "<<i<<" checks grid "<<rcgnum<<" and is valid.\n\n";
            }

            else if(val == 0 && rcg == 0){

                ofile<<"Thread "<<i<<" checks row "<<rcgnum<<" and is invalid.\n\nThe sudoku is invalid.\n\n";
                ofile<<"The total time taken is "<<duration.count()<<" microseconds.";
                ofile.close();

                br = 0;
                break;
            }

            else if(val == 0 && rcg == 1){

                ofile<<"Thread "<<i<<" checks column "<<rcgnum<<" and is invalid.\n\nThe sudoku is invalid.\n\n";
                ofile<<"The total time taken is "<<duration.count()<<" microseconds.";

                ofile.close();

                br = 0;
                break;
            }

            else if(val == 0 && rcg == 2){

                ofile<<"Thread "<<i<<" checks grid "<<rcgnum<<" and is invalid.\n\nThe sudoku is invalid.\n\n";
                ofile<<"The total time taken is "<<duration.count()<<" microseconds.";

                ofile.close();

                br = 0;
                break;
            }
            
        }

        if(br == 0){

            break;
        }
    }

    if(br == 1){

        ofile<<"The sudoku is valid.\n\n";
        ofile<<"The total time taken is "<<duration.count()<<" microseconds.";
        ofile.close();
    }

    for(int i = 0; i < k; i++){

        int p = retvar[i][0]->count;

        for(int j = 0; j < p; j++){

            free(retvar[i][j]);
        }

        free(retvar[i]);
    }

    return 0;
}