#ifndef S_RING_BUFFER_H__
#define S_RING_BUFFER_H__

#include "s_ring_buffer_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 环形缓冲区类型 */
typedef struct s_ring_buffer_s {
    unsigned int                   element_size;               /* 元素大小，单位：byte */
    unsigned char*                 element_pool;               /* 元素池 */
    unsigned int                   element_pool_cap;           /* 元素池容量 */

    unsigned int                   element_could_read_num;     /* 元素可读数量 */
    s_ring_buffer_lock_t           element_read_lock;          /* 元素读取锁 */
    unsigned int                   element_read_index;         /* 元素读取索引 */

    unsigned int                   element_could_write_num;    /* 元素可写数量 */
    s_ring_buffer_lock_t           element_write_lock;         /* 元素写入锁 */
    unsigned int                   element_write_index;        /* 元素写入索引 */
} s_ring_buffer_t;

/* 元素池构造器类型 */
typedef unsigned char* (*s_ring_buffer_element_pool_constructor_t)(unsigned int element_size,
                                                                   unsigned int element_pool_cap,
                                                                   void*        other_arg);
/* 元素池析构器类型 */
typedef void (*s_ring_buffer_element_pool_destructor_t)(unsigned char*  element_pool_addr);

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
                                           void*                                    other_arg);
/**
 * 环形缓冲区析构构器
 * s_ring_buffer_p：环形缓冲区指针
 * element_pool_destructor：环形缓冲区元素存储池析构器，为NULL，采用默认方式析构，需要与element_pool_constructor匹配
 * */
void s_ring_buffer_destructor(s_ring_buffer_t*                        s_ring_buffer_p,
                              s_ring_buffer_element_pool_destructor_t element_pool_destructor);

/**
 * 清空环形缓冲区
 * s_ring_buffer_p：环形缓冲区指针
 */
void s_ring_buffer_clear(s_ring_buffer_t* s_ring_buffer_p);

/**
 * 写数据进环形缓冲区,返回实际写入元素数量
 * s_ring_buffer_p: 环形缓冲区指针
 * p_elements_start_addr: 元素起始地址
 * elements_num: 元素数量
 * time_out: 超时时间，单位 ms
 */
unsigned int s_ring_buffer_write_elements(s_ring_buffer_t* s_ring_buffer_p,
                                          void*          p_elements_start_addr,
                                          unsigned int   elements_num,
                                          unsigned int   time_out);

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
                                         unsigned int      time_out);
/**
 * 获取环形缓冲区容量
 * s_ring_buffer_p：环形缓冲区指针
 */
unsigned int s_ring_buffer_cap_get(s_ring_buffer_t* s_ring_buffer_p);

/**
 * 获取环形缓冲区可读元素数量
 * s_ring_buffer_p：环形缓冲区指针
 */
unsigned int s_ring_buffer_could_read_num_get(s_ring_buffer_t* s_ring_buffer_p);

/**
 * 获取环形缓冲区可写元素数量
 * s_ring_buffer_p：环形缓冲区指针
 */
unsigned int s_ring_buffer_could_write_num_get(s_ring_buffer_t* s_ring_buffer_p);

#ifdef __cplusplus
} /* "C" */
#endif

#endif /* S_RING_BUFFER_H__ */
