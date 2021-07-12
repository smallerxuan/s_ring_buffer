#include <stdio.h>
#include <stdlib.h>
#include "s_ring_buffer.h"

/**
 * 默认元素池构造器
 */
unsigned char* s_ring_buffer_element_pool_constructor_malloc(unsigned int element_size,
                                                             unsigned int element_pool_cap,
                                                             void*        other_arg)
{
    return (unsigned char*)s_ring_buff_malloc(sizeof(unsigned char) * (element_size * element_pool_cap));
}

/**
 * 默认元素池析构器
 */
void s_ring_buffer_element_pool_destructor_malloc(unsigned char*  element_pool_addr)
{
    if(element_pool_addr != NULL) {
        s_ring_buff_free(element_pool_addr);
    }
    return;
}

/**
 * 环形缓冲区构造器
 * element_size：元素大小，单位 byte
 * element_pool_cap：元素存储池容量
 * element_pool_constructor：环形缓冲区元素存储池构造器，为NULL，采用默认方式构造，需要与element_pool_destructor匹配
 * other_arg：环形缓冲区元素存储池构造器的第三参数
 */
s_ring_buffer_t* s_ring_buffer_constructor(unsigned int                             element_size,
                                           unsigned int                             element_pool_cap,
                                           s_ring_buffer_element_pool_constructor_t element_pool_constructor,
                                           void*                                    other_arg)
{
    s_ring_buffer_t* r_buf = NULL;

    if(element_size == 0 || element_pool_cap == 0) {
        goto s_ring_buffer_constructor_out;
    }
    r_buf = (s_ring_buffer_t*)s_ring_buff_malloc(sizeof(s_ring_buffer_t));
    if(r_buf == NULL) {
        goto s_ring_buffer_constructor_out;
    }

    if(element_pool_constructor == NULL) {
        r_buf->element_pool = s_ring_buffer_element_pool_constructor_malloc(element_size, element_pool_cap, NULL);
    } else {
        r_buf->element_pool = element_pool_constructor(element_size, element_pool_cap, other_arg);
    }

    if(r_buf->element_pool == NULL) {
        s_ring_buff_free(r_buf);
        r_buf = NULL;
        goto s_ring_buffer_constructor_out;
    }

    r_buf->element_size             = element_size;
    r_buf->element_pool_cap         = element_pool_cap;
    r_buf->element_could_read_num   = 0;
    s_ring_buffer_unlock(&r_buf->element_read_lock);

    r_buf->element_read_index       = 0;
    r_buf->element_could_write_num  = element_pool_cap;
    s_ring_buffer_unlock(&r_buf->element_write_lock);
    r_buf->element_write_index      = 0;

s_ring_buffer_constructor_out:
    return r_buf;
}

/**
 * 环形缓冲区析构构器
 * s_ring_buffer_p：环形缓冲区指针
 * element_pool_destructor：环形缓冲区元素存储池析构器，为NULL，采用默认方式析构，需要与element_pool_constructor匹配
 * */
void s_ring_buffer_destructor(s_ring_buffer_t*                        s_ring_buffer_p,
                              s_ring_buffer_element_pool_destructor_t element_pool_destructor)
{
    if(s_ring_buffer_p == NULL) {
        goto s_ring_buffer_destructor_out;
    }
    if(element_pool_destructor == NULL) {
        s_ring_buffer_element_pool_destructor_malloc(s_ring_buffer_p->element_pool);
    } else {
        element_pool_destructor(s_ring_buffer_p->element_pool);
    }
    s_ring_buff_free(s_ring_buffer_p);

s_ring_buffer_destructor_out:
    return;
}

/**
 * 清空环形缓冲区
 * s_ring_buffer_p：环形缓冲区指针
 */
void s_ring_buffer_clear(s_ring_buffer_t* s_ring_buffer_p)
{
    if(s_ring_buffer_p == NULL || s_ring_buffer_p->element_pool == NULL) {
        goto s_ring_buffer_clear_out;
    }

    s_ring_buffer_lock(&s_ring_buffer_p->element_read_lock);
    s_ring_buffer_lock(&s_ring_buffer_p->element_write_lock);
    s_ring_buffer_p->element_could_read_num   = 0;
    s_ring_buffer_p->element_could_write_num  = s_ring_buffer_p->element_pool_cap;
    s_ring_buffer_p->element_read_index       = 0;
    s_ring_buffer_unlock(&s_ring_buffer_p->element_read_lock);
    s_ring_buffer_p->element_write_index      = 0;
    s_ring_buffer_unlock(&s_ring_buffer_p->element_write_lock);
    

s_ring_buffer_clear_out:
    return;
}

/**
 * 写数据进环形缓冲区,返回实际写入元素数量
 * s_ring_buffer_p: 环形缓冲区指针
 * p_elements_start_addr: 元素起始地址
 * elements_num: 元素数量
 * time_out: 超时时间，单位 ms
 */
unsigned int s_ring_buffer_write_elements(s_ring_buffer_t* s_ring_buffer_p,
                                          void*            p_elements_start_addr,
                                          unsigned int     elements_num,
                                          unsigned int     time_out)
{
    unsigned int res                            = 0;
    unsigned int elements_ture_write_num        = 0;
    unsigned int elements_current_write_num     = 0;
    unsigned int element_could_write_num        = 0;
    unsigned char* p_elements_cpy_addr          = (unsigned char*)p_elements_start_addr;

    if(s_ring_buffer_p == NULL ||
       p_elements_start_addr == NULL ||
       s_ring_buffer_p->element_pool == NULL ||
       s_ring_buffer_p->element_could_write_num == 0) {
        goto s_ring_buffer_write_elements_out;
    }
    
    time_out++;
    do {
        if(s_ring_buffer_try_lock(&s_ring_buffer_p->element_write_lock) == 1) {
            element_could_write_num = s_ring_buffer_p->element_could_write_num;
            elements_ture_write_num = (elements_num >= element_could_write_num)?(element_could_write_num):(elements_num);
            res = elements_ture_write_num;

            while(elements_ture_write_num > 0) {
                elements_current_write_num = ((s_ring_buffer_p->element_write_index + elements_ture_write_num) > s_ring_buffer_p->element_pool_cap)?
                                             (s_ring_buffer_p->element_pool_cap - s_ring_buffer_p->element_write_index):(elements_ture_write_num);

                s_ring_buff_memcpy(s_ring_buffer_p->element_pool + (s_ring_buffer_p->element_size * s_ring_buffer_p->element_write_index),
                                   p_elements_cpy_addr,
					               (s_ring_buffer_p->element_size * elements_current_write_num));
                p_elements_cpy_addr += (s_ring_buffer_p->element_size * elements_current_write_num);
                
                elements_ture_write_num                 -= elements_current_write_num;
                s_ring_buffer_p->element_could_write_num  -= elements_current_write_num;
                s_ring_buffer_p->element_write_index      = (s_ring_buffer_p->element_write_index + elements_current_write_num)%(s_ring_buffer_p->element_pool_cap);
            }
            s_ring_buffer_unlock(&s_ring_buffer_p->element_write_lock);
            s_ring_buffer_lock(&s_ring_buffer_p->element_read_lock);

            s_ring_buffer_p->element_could_read_num   += res;
            s_ring_buffer_unlock(&s_ring_buffer_p->element_read_lock);

            goto s_ring_buffer_write_elements_out;
        }
        s_ring_buff_sleep(1);
    } while(time_out--);

s_ring_buffer_write_elements_out:
	return res;
}

/**
 * 自环形缓冲区读取数据，返回实际读取元素数量
 * s_ring_buffer_p: 环形缓冲区指针
 * p_elements_save_start_addr: 元素存放起始地址
 * elements_num: 元素数量
 * time_out: 超时时间，单位 ms
 */
unsigned int s_ring_buffer_read_elements(s_ring_buffer_t*  s_ring_buffer_p,
                                         void*             p_elements_save_start_addr,
                                         unsigned int      elements_num,
                                         unsigned int      time_out)
{
    unsigned int    res  = 0;
    unsigned int    elements_ture_read_num        = 0;
    unsigned int    elements_current_read_num     = 0;
    unsigned int    element_could_read_num        = 0;
    unsigned char*  p_elements_cpy_addr           = (unsigned char*)p_elements_save_start_addr;

    if(s_ring_buffer_p == NULL ||
       p_elements_save_start_addr == NULL ||
       s_ring_buffer_p->element_pool == NULL ||
       s_ring_buffer_p->element_could_read_num == 0) {
        goto s_ring_buffer_read_elements_out;
    }

    time_out++;
    do {
        if(s_ring_buffer_try_lock(&s_ring_buffer_p->element_read_lock) == 1) {
            element_could_read_num = s_ring_buffer_p->element_could_read_num;
            elements_ture_read_num = (elements_num >= element_could_read_num)?(element_could_read_num):(elements_num);
            res = elements_ture_read_num;

            while(elements_ture_read_num > 0) {
                elements_current_read_num = ((s_ring_buffer_p->element_read_index + elements_ture_read_num) > s_ring_buffer_p->element_pool_cap)?
                                             (s_ring_buffer_p->element_pool_cap - s_ring_buffer_p->element_read_index):(elements_ture_read_num);

                s_ring_buff_memcpy(p_elements_cpy_addr,
				                   s_ring_buffer_p->element_pool + (s_ring_buffer_p->element_size * s_ring_buffer_p->element_read_index),
                                  (s_ring_buffer_p->element_size * elements_current_read_num));
                p_elements_cpy_addr += (s_ring_buffer_p->element_size * elements_current_read_num);
                
                elements_ture_read_num                   -= elements_current_read_num;
                s_ring_buffer_p->element_could_read_num  -= elements_current_read_num;
                s_ring_buffer_p->element_read_index      = (s_ring_buffer_p->element_read_index + elements_current_read_num)%(s_ring_buffer_p->element_pool_cap);
            }
            s_ring_buffer_unlock(&s_ring_buffer_p->element_read_lock);
            s_ring_buffer_lock(&s_ring_buffer_p->element_write_lock);

            s_ring_buffer_p->element_could_write_num   += res;
            s_ring_buffer_unlock(&s_ring_buffer_p->element_write_lock);

            goto s_ring_buffer_read_elements_out;
        }
        s_ring_buff_sleep(1);
    } while(time_out--);

s_ring_buffer_read_elements_out:
    return res;
}

/**
 * 获取环形缓冲区容量
 * s_ring_buffer_p：环形缓冲区指针
 */
unsigned int s_ring_buffer_cap_get(s_ring_buffer_t* s_ring_buffer_p)
{
    unsigned int res = 0;

    if(s_ring_buffer_p == NULL) {
        goto s_ring_buffer_cap_get_out;
    }

    res = s_ring_buffer_p->element_pool_cap;
s_ring_buffer_cap_get_out:
	return res;
}

/**
 * 获取环形缓冲区可读元素数量
 * s_ring_buffer_p：环形缓冲区指针
 */
unsigned int s_ring_buffer_could_read_num_get(s_ring_buffer_t* s_ring_buffer_p)
{
    unsigned int res = 0;

    if(s_ring_buffer_p == NULL) {
        goto s_ring_buffer_could_read_num_get_out;
    }

    res = s_ring_buffer_p->element_could_read_num;
s_ring_buffer_could_read_num_get_out:
	return res;
}

/**
 * 获取环形缓冲区可写元素数量
 * s_ring_buffer_p：环形缓冲区指针
 */
unsigned int s_ring_buffer_could_write_num_get(s_ring_buffer_t* s_ring_buffer_p)
{
    unsigned int res = 0;

    if(s_ring_buffer_p == NULL) {
        goto s_ring_buffer_could_write_num_get_out;
    }
    res = s_ring_buffer_p->element_could_write_num;
s_ring_buffer_could_write_num_get_out:
	return res;
}
