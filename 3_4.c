/*采用蝶形结构计算全局总和，首先计算comm_zs是2的幂，然后改成能够适应所有情况*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<mpi.h>

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

/*找到每个进程在每一轮对应的传输对象进程*/
void find_partner(
    int group_sz, /*in*/
    int distance, /*in*/
    int my_partner/*out*/)
{
    if(my_rank % group_sz < group_sz/2)
    {
        my_partner = my_rank + distance;
    }   
    else
    {
        my_partner = my_rank - distance;
    }
}

/*传输接收信息*/
void send_Recv(int group_sz, int distance, int local_data, int get_data, int my_partner)
{
    MPI_Send(&local_data, 1, MPI_INT, my_rank, my_partner, MPI_COMM_WORLD);/*数据发送给partner*/
    local_data += get_data; /*用刚刚接收的数据更新自己的local_data*/
    MPI_Barrier(MPI_COMM_WORLD); /*同步一次迭代*/
}
int main(void)
{
    int my_rank, comm_sz;
    int local_data; /*自己的数据*/
    int get_data;/*recv的数据*/
    int data_count; /*data_count == comm_sz*/
    int data[data_count];
    int my_parter;
    int distance = 1;
    int group_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /*0号进程读取用户输入并分发*/
    read_data(data, data_count, local_data);
    /*迭代循环*/
    for(group_sz = distance * 2; group_sz < data_count; group_sz *= 2)
    {
        find_partner(group_sz, distance, my_partner);
        send_Recv(group_sz, distance, local_data, get_data);
        distance *= 2;
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
