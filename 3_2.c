#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>

/*定义生成-1 到 1 的随机数*/
#define random() rand()*2.0 / RAND_MAX - 1

/*每一个进程都运行的估计函数*/
void local_sum(long long int local_in_circle, long long int local_tosses)
{
    local_in_circle = 0;
    for(toss = 0; toss < local_tosses; toss++)
    {
        x = (double)random();
        y = (double)random();
        distance_squared = x*x + y*y;
        if(distance_squared <= 1)
            local_in_circle ++;
    }
}

/*用mpi编写一个程序来估计pi，0读入总的投掷次数，并广播给各个进程，使用reduce来求局部变量的全局总和number_in_circle，用0来打印*/
/*击中圆内部的次数和投掷总数用long long int类型来表示，以精确pi的取值*/

void read_tosses(long long int number_of_tosses)
{
    if(my_rank == 0)
    {
        printf("Enter the number_of_tosses :\n");
        scanf("%lld", &number_of_tosses);
        /*计算出每个进程对应的local_tosses*/
        local_tosses = number_of_tosses / comm_sz;
        MPI_Bcast(local_tosses, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
    }
}

int main(void)
{
    int my_rank, comm_sz;
    long long int number_of_tosses;
    long long int number_in_circle;
    long long int local_tosses;
    long long int local_in_circle;
    double pi_estimate;

    /*初始化mpi*/
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, comm_sz);

    /*0号进程读入number_of_tosses并且分发*/
    read_tosses(number_of_tosses, local_tosses);

    /*对个进程对分配到的tosses进行计算*/
    local_sum(local_in_circle, local_tosses);

    /*0号进程计算总的和*/
    if(my_rank == 0)
    {
        MPI_Reduce(&local_tosses, &number_of_tosses, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_in_circle, &number_in_circle, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        pi_estimate = 4 * (number_in_circle / (double)number_of_tosses);
        printf("with the Monte Carlo method,\n from total %lld times tosses, we get %lld in circle,\n the estimate pi is : &f", number_of_tosses, number_in_circle, pi_estimate);
    }    

    MPI_Finalize();
    return 0;
}

