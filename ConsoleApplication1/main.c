#include <stdlib.h>
#include <stdio.h>
#include "common.h"

population *create_population(int pop_size, int bit_count, int data_count);
void decode(chromosome *crms, int *param, int bit_count, int data_count);
void free_population(population *pop);
void rand_population(population *pop);
void display_population(population *pop);
double fitness_func(int *param, int data_count);
double fitness_func2(int *param, int data_count);
void evaluate(population *pop, double (*function)(int[],int));
void mutate(chromosome *crms, int bit_count);
individual *select_individual(population *pop);
void new_generate(population *old_pop, population *new_pop);

int main(void)
{
	int i;
	int gen_count=150;
	individual *ivd1, *ivd2;
	int *param = (int *)malloc(sizeof(int));
	int pop_size=16;
	int bit_count=16;
	int data_count=2;
	population *pop=create_population(pop_size, bit_count, data_count);
	population *tmp_pop;
	population *old_pop = create_population(pop_size, bit_count, data_count);
	population *new_pop = create_population(pop_size, bit_count, data_count);
	
	// 1 개체군(population)내의 모든 개체(individual)들의 염색체(chromosome)를 초기화
	rand_population(pop);
	// 2 적응도 평가
	evaluate(pop, fitness_func);
	// 교배할 개체(individual)선택
	ivd1 = select_individual(pop);
	ivd2 = select_individual(pop);
	//3. 선택된 개체의 염색체 출력
	decode(ivd1->crms, param, bit_count, data_count);
	printf("%d,%d\n",param[0],param[1]);
	decode(ivd2->crms,param,bit_count,data_count);
	printf("%d,%d\n",param[0],param[1]);
	for(i=0;i<pop_size;i++)
	{
		printf("%d번째 individual 주소: %p\n", i,&pop->ivd[i]);
		printf("chromosome 주소:%p\n", pop->ivd[i].crms);
	}
	printf("---1세대 출력---\n");
	display_population(pop);
	for(i=0;i<gen_count;i++)
	{
		evaluate(old_pop, fitness_func2);
		new_generate(old_pop, new_pop);
		tmp_pop = old_pop;
		old_pop = new_pop;
		new_pop = tmp_pop;
	}
	printf("---%d세대 출력\n",gen_count);
	free_population(pop);
	printf("아무 키나 누르십시오...");getchar();
	return 0;
}