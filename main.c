#include<stdio.h>
#include"s_ring_buffer.h"

#define S_RING_BUFF_ELEM_TYPE int
#define S_RING_BUFF_ELEM_CAP 6

s_ring_buffer_t* s_ring_buffer_g_p = NULL;

#define READ_WRITE_BUFF_CAP S_RING_BUFF_ELEM_CAP * 6
S_RING_BUFF_ELEM_TYPE write_src[READ_WRITE_BUFF_CAP] = {0};
S_RING_BUFF_ELEM_TYPE* p_write_src = NULL;

S_RING_BUFF_ELEM_TYPE read_des[READ_WRITE_BUFF_CAP] = {0};
S_RING_BUFF_ELEM_TYPE* p_read_des = NULL;

int main(void)
{
	s_ring_buffer_g_p = s_ring_buffer_constructor(sizeof(S_RING_BUFF_ELEM_TYPE), S_RING_BUFF_ELEM_CAP, NULL, NULL);
	printf("s_ring_buffer_g_p:%x\n", s_ring_buffer_g_p);
	
	for(int index = 0; index < READ_WRITE_BUFF_CAP; index++) {
		write_src[index] = index + 1;
	}
	p_write_src = write_src;
	p_read_des = read_des;
	
	unsigned int write_count = 0, read_count = 0;
	
	for(int step = 0; step < READ_WRITE_BUFF_CAP - S_RING_BUFF_ELEM_CAP; step++) {
		printf("\nstep %d\n", step + 1);

		read_count = s_ring_buffer_read_elements(s_ring_buffer_g_p, p_read_des, 1, 1);
		p_read_des += read_count;
		printf("read_count:%d\n", read_count);
		for(int index = 0; index < READ_WRITE_BUFF_CAP; index++) {
			printf("%d ",read_des[index]);
		}
		
		write_count = s_ring_buffer_write_elements(s_ring_buffer_g_p, (void*)p_write_src, READ_WRITE_BUFF_CAP, 1);
		p_write_src += write_count;
		printf("\nwrite_count:%d\n", write_count);
	
		for(int index_2 = 0; index_2 < S_RING_BUFF_ELEM_CAP; index_2++) {
			printf("%d ",*(((S_RING_BUFF_ELEM_TYPE*)s_ring_buffer_g_p->element_pool) + index_2));
		}
		sleep(1);
	}
	
	return 0;
}