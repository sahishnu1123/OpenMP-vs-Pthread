#include<iostream>
#include<fstream>
#include<omp.h>
#include<chrono>

using namespace std;
using namespace std::chrono;

int N,k;

struct state{

    int validity;
    int thread_num;
    int count;
};

typedef struct state *State;

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

int main(int argc, char *argv[]){

    ifstream ipfile;

    ipfile.open(argv[1]);

    ipfile>>k;
    ipfile>>N;

    if(sqroot(N) == -1){

        return 1;
    }

    int args[N*N];

    for(int i = 0; i < N*N; i++){

        ipfile>>args[i];
    }

    ipfile.close();

    State info[3*N];

    for(int i = 0; i < 3*N; i++){

        info[i] = (State)malloc(sizeof(struct state));
    }   

    omp_set_num_threads(k);

    auto start = high_resolution_clock::now();

    #pragma omp parallel
    {

        int i = omp_get_thread_num();

        int count = 0;
        int check = k*count + i;

        while(check < 3*N){

            int q = check/N;

            if(q == 0){

                info[check]->thread_num = i;
                info[check]->validity = 1;
                info[check]->count = count;

                if(check_rows(args, (k*count + i)%N) != 0){

                    info[check]->validity = 0;
                }
            }

            if(q == 1){

                info[check]->thread_num = i;
                info[check]->validity = 1;
                info[check]->count = count;

                if(check_columns(args, (k*count + i)%N) != 0){

                    info[check]->validity = 0;
                }
            }

            if(q == 2){

                info[check]->thread_num = i;
                info[check]->validity = 1;
                info[check]->count = count;

                if(check_grids(args, (k*count + i)%N) != 0){

                    info[check]->validity = 0;
                }
            }

            count++;
            check += k;
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    int confirm = 1;

    ofstream ofile;
    ofile.open("output.txt");

    for(int i = 0; i < 3*N; i++){

        int rcg = i/N;
        int rcgnum = ((k*info[i]->count) + i)%N;
        int validity = info[i]->validity;

        if(rcg == 0 && validity == 1){

            ofile<<"Thread "<<info[i]->thread_num<<" checks row "<<rcgnum<<" and is valid.\n\n";
        }

        else if(rcg == 1 && validity == 1){

            ofile<<"Thread "<<info[i]->thread_num<<" checks column "<<rcgnum<<" and is valid.\n\n";
        }

        else if(rcg == 2 && validity == 1){

            ofile<<"Thread "<<info[i]->thread_num<<" checks grid "<<rcgnum<<" and is valid.\n\n";
        }

        else if(rcg == 0 && validity == 0){

            ofile<<"Thread "<<info[i]->thread_num<<" checks row "<<rcgnum<<" and is invalid.\n\n";
            ofile<<"Sudoku is invalid\n";
            ofile<<"The total time taken is "<<duration.count()<<" microseconds.";
            confirm = 0;
            ofile.close();
            break;
        }

        else if(rcg == 1 && validity == 0){

            ofile<<"Thread "<<info[i]->thread_num<<" checks column "<<rcgnum<<" and is invalid.\n\n";
            ofile<<"Sudoku is invalid\n\n";
            ofile<<"The total time taken is "<<duration.count()<<" microseconds.";
            confirm = 0;
            ofile.close();
            break;
        }

        else if(rcg == 2 && validity == 0){

            ofile<<"Thread "<<info[i]->thread_num<<" checks grid "<<rcgnum<<" and is invalid.\n\n";
            ofile<<"Sudoku is invalid\n\n";
            ofile<<"The total time taken is "<<duration.count()<<" microseconds.";
            confirm = 0;
            ofile.close();
            break;
        }
    }

    if(confirm == 1){

        ofile<<"Sudoku is valid.";
        ofile<<"The total time taken is "<<duration.count()<<" microseconds.";
        ofile.close();
    }

    for(int i = 0; i < k; i++){

        free(info[i]);
    }    

    return 0;

}
