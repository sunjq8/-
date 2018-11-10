#include<stdio.h>
#include<mpi.h>
#include<math.h>

/*读输入数据，并且分发给各个进程*/
void read_data(
    double data[],
    int data_count,
    int local_count
)
{
    if(my_rank == 0)
    {
        int i;
        /*用于储存缓冲输入*/
        /*double* d = NULL; 
        d = malloc(data_count * sizeof(double));*/
        printf("Enter a double group, with the size of %d :\n", data_count);
        for(i = 0; i<data_count; i++)
        {
            scanf("%lf, ", &data[i]);
        }
        MPI_Scatter(data, local_count, MPI_DOUBLE, local_data, local_count, MPI_DOUBLE, 0, MPI_COMM_WRLD);
        /*free(d);*/  
    }
    else
    {
        MPI_Scatter(data, local_count, MPI_DOUBLE, local_data, local_count, MPI_DOUBLE, 0, MPI_COMM_WRLD);
    }
}

/*对每一个data元素找到对应的bin*/
void find_bin(
    double single_data, 
    double bin_maxes[], 
    double loc_bin_cts[],
    double min_meas,
    int local_count,
    int bin_width)
{
    int i;
    for(i = 0; i < local_count; i++)
    {
        if(single_data < bin_maxes[i] && single_data >= (min_meas + bin_width * i)
        {
            loc_bin_cts[i]++;
        }
    }
}

/*取最值*/
void max_minDouble(double data[], maxDouble, minDouble)
{
    double min = data[0];
    double max = data[0];
    int i;
    int length = sizeof(data) / sizeof(data[0]);
    for(i = 1; i < length; i++)
    {
        if(data[i] > min)
        {
            max = data[i];
        }
        if(data[i] < min)
        {
            min = data[i];
        }
    }
    maxDouble = max;
    minDouble = min;
}

/*主函数*/
int main(void)
{
    int my_rank, comm_sz;
    int data_count = 16, bin_count = 5;
    double data[data_count]; /*所有输入数据*/
    int local_count;/*每个进程分配到的数据的个数*/
    double local_data[local_int];/*每个进程分配到的数据*/
    int loc_bin_cts[bin_count];
    int bin_counts[bin_count];/*每个桶中元素的个数*/
    double min_meas, max_meas;
    double bin_maxes[bin_count];/*每个桶的最大值*/
    double bin_width;/*每个桶的range*/
    int j;/*用于遍历*/

    /*初始化MPI*/
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, comm_sz);

    /*相关数值计算, 假定可以整除, 0号进程进完成*/
    if(my_rank == 0)
    {
        local_count = data_count / comm_sz;
        max_minDouble(data, max_meas, min_meas);
        bin_width = (max_meas - min_meas) / bin_count;
        for(j = 0; j< bin_count; j++)
        {
            bin_maxes[j] = min_meas + bin_width * (j + 1);
        }
    }
    
    /*读数据，并且分发*/
    read_data(data, data_count, local_count);
    for(j = 0; j<data_count; j++)
    {
        find_bin(data[j], bin_maxes, loc_bin_cts, min_meas, local_count, bin_width);
    }

    /*数据求总*/
    MPI_Reduce(loc_bin_cts, bin_counts, local_count, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    /*0号进程打印结果*/
    if(my_rank == 0)
    {
        int max, min;
        int level;
        max_minDouble(bin_counts, max, min);
        printf(" ^\t\t\t\t\t\t\t\n"); /*打印第一排的内容*/
        printf(" |\t\t\t\t\t\t\t\n"); /*打印第二排的内容*/
        for(level = max; level > 0; level--)
        {
            int k;
            printf("%d+\t");/*打印每第一排最开始的6个元素*/
            for(k = 0; k < bin_count; k++)
            {
                if(bin_counts[k]==level)
                {
                    printf("____");
                }
                else
                {
                    printf("\t");
                }
            }
            printf("\t\n");/*打印每第二排最后的空格*/

            printf(" |\t");/*打印每第二排最后的空格*/
            for(k = 0; k < bin_count; k++)
            {
                if(bin_counts[k]==level)
                {
                    printf("|  |");
                }
                else
                {
                    printf("\t");
                }
            }
            printf("\t\n");
        }
        printf("-+----");
        /*打印倒数第二排的坐标轴*/
        for(k = 0; k < bin_count; k++)
        {
            printf("----");
        }
        printf("---->");
        /*打印最后一排的坐标点*/
        printf(" |\t")
        for(k = 0; k < bin_count; k++)
        {
            printf("%lf  %lf", (bin_maxes[k]-bin_width), bin_maxes[k]);
        }
    }
}


