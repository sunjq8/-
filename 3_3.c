/*采用树形结构计算全局总和，首先计算comm_zs是2的幂，然后改成能够适应所有情况*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/*0号进程读取并且分发数据给各个进程*/
void read_data(int data[], int comm_sz, int local_data)
{
    if(my_rank == 0)
    {
        int i;
        /*读取用户输入到data[]*/
        printf("Plz enter %d data :\n", comm_sz);
        for(i = 0; i < comm_sz; i++)
            scanf("%d", data[i]);

        /*0号进程分发data到各个进程*/
        for(i = 0; i< comm_sz; i++)
        {
            MPI_Send(&data[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    /*其他进程接收数据*/
    else
    {
        MPI_Recv(&local_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

/*迭代传递消息*/
void send_recv(int local_data, int distance, int flag)
{
    if(my_rank % flag == 0) /*接收数据的进程*/
    {
        MPI_Recv(&local_data, 1, MPI_INT, my_rank + distance , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if(my_rank % distance == 0) /*发送数据的进程*/
    {
        MPI_Send(&get_data, 1, MPI_INT, my_rank - distance, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD); /*一次发送接收完毕*/
    
}

/*计算*/
void calculate(int flag, int local_data, int get_data)
{
    if(my_rank % flag == 0)
        local_data += get_data;
    /*一次计算之后完成第一次迭代*/
    distance *= 2；
    flag *= 2；
}

/*主函数*/
int main(void)
{
    int my_rank, comm_sz;
    int data_count; /*data_count == comm_sz*/
    int data[data_count];
    int local_data;
    int get_data;
    int distance = 1;/*发送进程的间隔*/
    int flag = distance * 2; /*用于判断当前迭代下的接收进程*/
    int k;

    /*初始化mpi*/
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /*读取数据并分发*/
    read_data(data, MPI_COMM_WORLD, local_data);
    if((int)pow(2.0, flag) <= 8) /*迭代次数确认*/
    {
        send_recv(local_data, distance, flag);
        calculate(flag, local_data, get_data);
    }

    /*0号进程打印结果*/
    if(my_rank == 0)
    {
        printf("the sum of : ");
        for(k = 0; k < data_count; k++)
            printf("%d, ", data[k]);
        printf("\n is %d", local_data);
    }
    
    MPI_Finalize();
    return 0;
}

