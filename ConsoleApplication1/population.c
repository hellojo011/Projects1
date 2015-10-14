#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <minmax.h>
#include "common.h"

population* create_population(int pop_size, int bit_count, int data_count)
{
	int i;
	//1 개체군 또는 집단(population)을 위한 메모리 생성
	population *pop=(population*)malloc(sizeof(population));
	//2 개체수 (pop_size)만큼의 개체(individual)를 생성
	individual *ivd = (individual*)malloc(sizeof(individual)*pop_size);
	//3 모든 개체(individual) 내의 염색체 메모리 생성
	for(i=0; i<pop_size; i++)
	{
		ivd[i].crms = (int*)malloc(sizeof(chromosome)*bit_count);
		ivd[i].fitness = 0.0;
	}
	//4 개체군(population)을 위한 개체 및 개체정보 설정
	pop->ivd = ivd;
	pop->pop_size = pop_size;
	pop->bit_count = bit_count; //chromosome의 비트수
	pop->data_count = data_count; //chromosome 내의 데이터 개수
	//5 개체군 리턴
	return pop;
}

void free_population(population *pop){
	int i;
	// 1 모든 개체(individual)들의 염색체의 메모리 제거
	for (i=0; i<pop->pop_size;i++)
	{
		free(pop->ivd[i].crms);
	}
	// 2 전체 개체(individual)의 전체 메모리 제거
	free(pop->ivd);
	//3 개체군(population)의 메모리 제거
	free(pop);
}

//a와 b사이에 존재하는 수(number)중 하나를 랜덤하게 추출한다

int rand_between(int a,int b)
{
	int delta,randnum;
	delta = abs(a-b)+1;
	randnum=rand()%delta;
	return (min(a,b)+randnum);
}
void rand_population(population *pop)
{
	int i; //개체들의 index
	int pos; //염색체의 index
	// rand()함수에 seed 설정
	srand(time(NULL));
	//2 모든 개체 검색
	for(i=0;i<pop->pop_size;i++)
	{
		//3 개체 내의 염셕체의 비트를 검색
		for(pos=0;pos<pop->bit_count;pos++)
		{
			pop->ivd[i].crms[pos]=rand_between(0,1);
		}
	}
}

void decode(chromosome *crms, int *param, int bit_count, int data_count)
{
	int pos; //염색체의 index
	int n; // 데이터의 index
	int i; //bit index
	//1. 데이터의 비트수 계산
	int data_bit_count = bit_count/data_count;
	//2. 염색체 내의 데이터의 개수만큼 param 획득
	pos = 0;
	for(n = 0; n<data_count; n++)
	{
		param[n] = 0;
		//3. 전체 염색체의 비트를 데이터의 비트수(data_bit_count)만큼 잘라서 param에 할당
		for(i=0;i<data_bit_count;i++)
		{
			//4. 각각의 비트에 2의 승수를 곱해준다.
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

// 염색체(chromosome)의 적응도값(fitness)을 계산

void evaluate(population *pop, double (*function)(int[],int))
{
	int index;
	//1. 염색체의 데이터를 추출해서 저장할 메모리 생성
	int *param = (int *)malloc(sizeof(int)*pop->data_count);
	//2. 개체수(pop_size)만큼 개체(individual)의 염색체의 데이터를 추출한다.
	for(index = 0; index <pop->pop_size; index++)
	{
		//3. 염색체(chromosome)의 데이터를 해석해서 param에 저장
		decode(pop->ivd[index].crms, param,pop->bit_count,pop->data_count);
		//4. 적응도 함수(fitness functon)를 이용해서 적응도값(fitness)을 계산한다.
		pop->ivd[index].fitness=(*function)(param,pop->data_count);
	}
	//5. 메모리 해제
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

// 염색체의 특정 위치에 돌연변이 추가
void mutate(chromosome *crms, int bit_count)
{
	int i;
	//1. 염색체의 모든 비트를 검색한다.
	for(i = 0; i<bit_count; i++)
	{
		if(rand_between(0, 300) ==0)
		{
			crms[i] ^=1 ;
		}
	}
}

// 하나의 개체군(population)에서 교배할 하나의 개체(individual) 선택
individual* select_individual(population *pop)
{
	unsigned int index1, index2;
	int i, count = 0;
	//1. 두 개의 개체를 추출
	//두 개체의 적응도값 (fitness value)이 같지 않을때까지 개체 추출
	do
	{
		count++;
		//2.배양할 개체를 랜덤하게 2개를 추출
		index1= rand_between(0, pop->pop_size-1);
		index2= rand_between(0, pop->pop_size-1);
		//3.적응도값이 0이면 초기값이기 때문에 while문을 빠져서 둘 중 하나를 선택한다
		if(pop->ivd[index1].fitness == 0)
		{
			break;
		}
		//4.적응도값이 4번 같으면 모든 개체를 대상으로 돌연변이를 추가한다
		if(count>3)
		{
			for(i=0; i<pop->pop_size; i++)
			{
				mutate(pop->ivd[i].crms, pop->bit_count);
			}
			break;
		}
		//5. 선택된 두 개의 개체의 적응도값(fitness)이 다르면 while 문을 빠져나온다
	}while(pop->ivd[index1].fitness == pop->ivd[index2].fitness);
	//6. 적응도 값이 크면 선택한다.
	if(pop->ivd[index1].fitness>pop->ivd[index2].fitness)
		return &pop->ivd[index1];
	else
		return &pop->ivd[index2];
}


//두 개의 염색체를 교배하여 새로운 두 개의 염색체로 만든다.
void crossover(chromosome *old_c1,
			   chromosome *old_c2,
			   chromosome *new_c1,
			   chromosome *new_c2,
			   int bit_count){
				   int i;
				   int pos;
				   //1. 염색체(chromosome)의 데이터를 복제한다.
				   for(i=0; i<bit_count; i++)
				   {
					   new_c1[i] = old_c2[i];
					   new_c2[i] = old_c1[i];
				   }
				   //2.교배할 위치를 얻어낸다
				   pos = rand_between(0, bit_count - 1);
				   //3. 특정 위치(pos)의 데이터를 교배한다.
				   new_c1[pos] = old_c1[pos];
				   new_c2[pos] = old_c2[pos];
}

//두 개체를 교배 또는 돌연변이를 가미해서 배양한다.
void breed(individual *old_v1, individual *old_v2, individual *new_v1, individual *new_v2, int bit_count)
{
	//1. 교배시킨다
	crossover(old_v1->crms, old_v2->crms, new_v1->crms, new_v2->crms, bit_count);
	//2. 돌연변이를 가한다
	mutate(new_v1->crms, bit_count);
	mutate(new_v2->crms, bit_count);
}

void new_generate(population *old_pop, population *new_pop)
{
	int i;
	individual *v1, *v2;
	//랜덤하게 2개의 개체를 pop_size/2 번
	for(i=0;i<old_pop->pop_size-1;i++)
	{
		//배양할 개체(individual)쌍을 선택한다
		v1 = select_individual(old_pop);
		v2 = select_individual(old_pop);
		//2. 배양(breed)해서 개체군의 개체를 만들고 새로운 세대를 만든다.
		breed(v1, v2, &new_pop->ivd[i], &new_pop->ivd[i+1], old_pop->bit_count);
	}
}