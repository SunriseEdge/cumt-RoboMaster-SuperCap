#include "filter.h"
LOWPASS_FILTER_STRUCT lpf[5];

/*------一阶低通滤波------*/
void low_filter_init(LOWPASS_FILTER_STRUCT *p, float sample_f, float cutoff_f)
{
    float Tc; //时间常数τ
    p->Fc = cutoff_f;
    p->Fs = sample_f;
    if (p->Fc <= 0.0f || p->Fs <= 0.0f)
    {
        p->Ka = 1.0f;
        p->Kb = 0.0f;
        return;
    }

    Tc = 1.0f / (PI2 * p->Fc);

    p->Ka = 1.0f / (1.0f + Tc * p->Fs);
    p->Kb = 1.0f - p->Ka;
    p->Input = 0.0f;
    p->Output = 0.0f;
}

void low_filter_calc(LOWPASS_FILTER_STRUCT *p)
{
    if (p->Output == p->Input)
        return;

    p->Output = p->Ka * p->Input + p->Kb * p->Output;
}

/*------冒泡排序------*/
void bubble_sort(uint32_t *a, unsigned char Num)
{
    uint32_t *p = a;
    uint8_t i, j, tmp;
    for (i = 0; i < Num - 1; i++)
    {
        for (j = 0; j + i < Num - 1; j++)
        {
            if (a[j] > a[j + 1])
            {
                tmp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = tmp;
            }
        }
    }
}



/*------递推平均滤波法------*/
#define N1 5

float value_buf[N1];
uint8_t i = 0;

float recursive_filter(float a)
{
    uint8_t count = 0;
    float sum = 0;

    value_buf[i++] = a;
    if (i == N1)
    {
        i = 0;    //先进先出
    }
    for (count = 0; count < N1; count++)
    {
        sum += value_buf[count];
    }
    return (float) (sum / 5.0f);
}


/*------中位平均滤波------*/
uint32_t middle_filter(uint32_t *a, unsigned char Num, char extremum)
{
    uint8_t index = 0x00;
    uint32_t avg_sample = 0x00;
    bubble_sort(a, Num);
    // quick_sort(a,0,Num-1)  ;
    for (index = extremum; index < Num - extremum; index++)
    {
        avg_sample += a[index];
    }
    /* 计算N-X 个ADC 采样的均值 */
    avg_sample /= Num - 2 * extremum;
    /* 返回均值 */
    return (uint32_t) avg_sample;
}


/*------中位平均滤波②------*/
#define N 10 // 滤波器窗口大小

uint32_t values[ADC1_CHANNEL_FRE]; // 存储最近 N 个数据

int index1 = 0; // 当前数据的索引

// 比较函数，用于 qsort 排序
int compare(const void *a, const void *b) 
{
    return (*(uint16_t*)a - *(uint16_t*)b);
}

 uint32_t median_filter(uint32_t value,uint32_t values_t[N], uint32_t sorted_values_t[N])
{
	
    // 将新数据添加到数组中
    values_t[index1] = value;
    index1 = (index1 + 1) % N;

    // 复制 values 数组
    for (int i = 0; i < N; i++) {
        sorted_values_t[i] = values_t[i];
    }

    // 对数据进行排序
    qsort(sorted_values_t, N, sizeof(uint32_t), compare);

    // 返回中间数据作为滤波结果
    return sorted_values_t[N / 2];
}



