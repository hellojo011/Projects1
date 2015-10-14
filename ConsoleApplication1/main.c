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
	
	// 1 ��ü��(population)���� ��� ��ü(individual)���� ����ü(chromosome)�� �ʱ�ȭ
	rand_population(pop);
	// 2 ������ ��
	evaluate(pop, fitness_func);
	// ������ ��ü(individual)����
	ivd1 = select_individual(pop);
	ivd2 = select_individual(pop);
	//3. ���õ� ��ü�� ����ü ���
	decode(ivd1->crms, param, bit_count, data_count);
	printf("%d,%d\n",param[0],param[1]);
	decode(ivd2->crms,param,bit_count,data_count);
	printf("%d,%d\n",param[0],param[1]);
	for(i=0;i<pop_size;i++)
	{
		printf("%d��° individual �ּ�: %p\n", i,&pop->ivd[i]);
		printf("chromosome �ּ�:%p\n", pop->ivd[i].crms);
	}
	printf("---1���� ���---\n");
	display_population(pop);
	for(i=0;i<gen_count;i++)
	{
		evaluate(old_pop, fitness_func2);
		new_generate(old_pop, new_pop);
		tmp_pop = old_pop;
		old_pop = new_pop;
		new_pop = tmp_pop;
	}
	printf("---%d���� ���\n",gen_count);
	free_population(pop);
	printf("�ƹ� Ű�� �����ʽÿ�...");getchar();
	return 0;
}