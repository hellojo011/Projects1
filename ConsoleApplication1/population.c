#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <minmax.h>
#include "common.h"

population* create_population(int pop_size, int bit_count, int data_count)
{
	int i;
	//1 ��ü�� �Ǵ� ����(population)�� ���� �޸� ����
	population *pop=(population*)malloc(sizeof(population));
	//2 ��ü�� (pop_size)��ŭ�� ��ü(individual)�� ����
	individual *ivd = (individual*)malloc(sizeof(individual)*pop_size);
	//3 ��� ��ü(individual) ���� ����ü �޸� ����
	for(i=0; i<pop_size; i++)
	{
		ivd[i].crms = (int*)malloc(sizeof(chromosome)*bit_count);
		ivd[i].fitness = 0.0;
	}
	//4 ��ü��(population)�� ���� ��ü �� ��ü���� ����
	pop->ivd = ivd;
	pop->pop_size = pop_size;
	pop->bit_count = bit_count; //chromosome�� ��Ʈ��
	pop->data_count = data_count; //chromosome ���� ������ ����
	//5 ��ü�� ����
	return pop;
}

void free_population(population *pop){
	int i;
	// 1 ��� ��ü(individual)���� ����ü�� �޸� ����
	for (i=0; i<pop->pop_size;i++)
	{
		free(pop->ivd[i].crms);
	}
	// 2 ��ü ��ü(individual)�� ��ü �޸� ����
	free(pop->ivd);
	//3 ��ü��(population)�� �޸� ����
	free(pop);
}

//a�� b���̿� �����ϴ� ��(number)�� �ϳ��� �����ϰ� �����Ѵ�

int rand_between(int a,int b)
{
	int delta,randnum;
	delta = abs(a-b)+1;
	randnum=rand()%delta;
	return (min(a,b)+randnum);
}
void rand_population(population *pop)
{
	int i; //��ü���� index
	int pos; //����ü�� index
	// rand()�Լ��� seed ����
	srand(time(NULL));
	//2 ��� ��ü �˻�
	for(i=0;i<pop->pop_size;i++)
	{
		//3 ��ü ���� ����ü�� ��Ʈ�� �˻�
		for(pos=0;pos<pop->bit_count;pos++)
		{
			pop->ivd[i].crms[pos]=rand_between(0,1);
		}
	}
}

void decode(chromosome *crms, int *param, int bit_count, int data_count)
{
	int pos; //����ü�� index
	int n; // �������� index
	int i; //bit index
	//1. �������� ��Ʈ�� ���
	int data_bit_count = bit_count/data_count;
	//2. ����ü ���� �������� ������ŭ param ȹ��
	pos = 0;
	for(n = 0; n<data_count; n++)
	{
		param[n] = 0;
		//3. ��ü ����ü�� ��Ʈ�� �������� ��Ʈ��(data_bit_count)��ŭ �߶� param�� �Ҵ�
		for(i=0;i<data_bit_count;i++)
		{
			//4. ������ ��Ʈ�� 2�� �¼��� �����ش�.
			param[n] = param[n] + pow(2.0,(double)i) *crms[pos++];
		}
	}
}

void display_population(population *pop)
{
	int n;
	int i;
	int *param = (int *)malloc(sizeof(int) *pop->data_count);
	for(i=0;i<pop->pop_size; i++)
	{
		decode(pop->ivd[i].crms, param, pop->bit_count, pop->data_count);
		for(n=0; n<pop->data_count;n++)
		{
			printf("%d\t", param[n]);
		}
		printf("\n");
	}
	free(param);
}

// ����ü(chromosome)�� ��������(fitness)�� ���

void evaluate(population *pop, double (*function)(int[],int))
{
	int index;
	//1. ����ü�� �����͸� �����ؼ� ������ �޸� ����
	int *param = (int *)malloc(sizeof(int)*pop->data_count);
	//2. ��ü��(pop_size)��ŭ ��ü(individual)�� ����ü�� �����͸� �����Ѵ�.
	for(index = 0; index <pop->pop_size; index++)
	{
		//3. ����ü(chromosome)�� �����͸� �ؼ��ؼ� param�� ����
		decode(pop->ivd[index].crms, param,pop->bit_count,pop->data_count);
		//4. ������ �Լ�(fitness functon)�� �̿��ؼ� ��������(fitness)�� ����Ѵ�.
		pop->ivd[index].fitness=(*function)(param,pop->data_count);
	}
	//5. �޸� ����
	free(param);
}

double fitness_func(int *param, int data_count)
{
	int i;
	double fitness=0;
	double sum=0;
	for(i=0;i < data_count;i++)
	{
	sum += param[i];
	}
	fitness = sum;
	return fitness;
}

double fitness_func2(int *param, int data_count)
{
	int i;
	double average=0;
	double variance=0;
	for(i=0;i < data_count;i++)
	{
		average += param[i];
	}
	average /= data_count;
	for(i=0;i<data_count;i++)
	{
		variance += (average - param[i]) *(average - param[i]);
	}
	return variance;
}

// ����ü�� Ư�� ��ġ�� �������� �߰�
void mutate(chromosome *crms, int bit_count)
{
	int i;
	//1. ����ü�� ��� ��Ʈ�� �˻��Ѵ�.
	for(i = 0; i<bit_count; i++)
	{
		if(rand_between(0, 300) ==0)
		{
			crms[i] ^=1 ;
		}
	}
}

// �ϳ��� ��ü��(population)���� ������ �ϳ��� ��ü(individual) ����
individual* select_individual(population *pop)
{
	unsigned int index1, index2;
	int i, count = 0;
	//1. �� ���� ��ü�� ����
	//�� ��ü�� �������� (fitness value)�� ���� ���������� ��ü ����
	do
	{
		count++;
		//2.����� ��ü�� �����ϰ� 2���� ����
		index1= rand_between(0, pop->pop_size-1);
		index2= rand_between(0, pop->pop_size-1);
		//3.���������� 0�̸� �ʱⰪ�̱� ������ while���� ������ �� �� �ϳ��� �����Ѵ�
		if(pop->ivd[index1].fitness == 0)
		{
			break;
		}
		//4.���������� 4�� ������ ��� ��ü�� ������� �������̸� �߰��Ѵ�
		if(count>3)
		{
			for(i=0; i<pop->pop_size; i++)
			{
				mutate(pop->ivd[i].crms, pop->bit_count);
			}
			break;
		}
		//5. ���õ� �� ���� ��ü�� ��������(fitness)�� �ٸ��� while ���� �������´�
	}while(pop->ivd[index1].fitness == pop->ivd[index2].fitness);
	//6. ������ ���� ũ�� �����Ѵ�.
	if(pop->ivd[index1].fitness>pop->ivd[index2].fitness)
		return &pop->ivd[index1];
	else
		return &pop->ivd[index2];
}


//�� ���� ����ü�� �����Ͽ� ���ο� �� ���� ����ü�� �����.
void crossover(chromosome *old_c1,
			   chromosome *old_c2,
			   chromosome *new_c1,
			   chromosome *new_c2,
			   int bit_count){
				   int i;
				   int pos;
				   //1. ����ü(chromosome)�� �����͸� �����Ѵ�.
				   for(i=0; i<bit_count; i++)
				   {
					   new_c1[i] = old_c2[i];
					   new_c2[i] = old_c1[i];
				   }
				   //2.������ ��ġ�� ����
				   pos = rand_between(0, bit_count - 1);
				   //3. Ư�� ��ġ(pos)�� �����͸� �����Ѵ�.
				   new_c1[pos] = old_c1[pos];
				   new_c2[pos] = old_c2[pos];
}

//�� ��ü�� ���� �Ǵ� �������̸� �����ؼ� ����Ѵ�.
void breed(individual *old_v1, individual *old_v2, individual *new_v1, individual *new_v2, int bit_count)
{
	//1. �����Ų��
	crossover(old_v1->crms, old_v2->crms, new_v1->crms, new_v2->crms, bit_count);
	//2. �������̸� ���Ѵ�
	mutate(new_v1->crms, bit_count);
	mutate(new_v2->crms, bit_count);
}

void new_generate(population *old_pop, population *new_pop)
{
	int i;
	individual *v1, *v2;
	//�����ϰ� 2���� ��ü�� pop_size/2 ��
	for(i=0;i<old_pop->pop_size-1;i++)
	{
		//����� ��ü(individual)���� �����Ѵ�
		v1 = select_individual(old_pop);
		v2 = select_individual(old_pop);
		//2. ���(breed)�ؼ� ��ü���� ��ü�� ����� ���ο� ���븦 �����.
		breed(v1, v2, &new_pop->ivd[i], &new_pop->ivd[i+1], old_pop->bit_count);
	}
}