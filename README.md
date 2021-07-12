# s_ring_buffer
一个小巧的有锁环形缓冲区实现，读写锁分离，性能更强劲。支持多读、多写同时操作；支持动态（指定元素池构造器与元素池析构器）和静态（不指定元素池构造器与元素池析构器）创建环形缓冲区的元素池；指定元素大小可以操作指定大小的元素对象(单位byte)，如：char、int、long、bool、struct、point等。

## 文件结构
s_ring_buffer源文件目前版本由：`s_ring_buffer_platform.h`、`s_ring_buffer_platform.c`、`s_ring_buffer.h` 、`s_ring_buffer.c` 组成，仓库中的 `main.c` 中对使用方式进行了简单示例。

## 用户接口
s_ring_buffer的用户接口包括：  
`s_ring_buffer_constructor()`、`s_ring_buffer_destructor()`、`s_ring_buffer_clear()`、`s_ring_buffer_write_elements()`、`s_ring_buffer_read_elements()`、`s_ring_buffer_cap_get()`、`s_ring_buffer_could_read_num_get()`、`s_ring_buffer_could_write_num_get()`

## 使用方式
在需要使用s_ring_buffer的文件、模块中包含头文件 s_ring_buffer.h即可使用。

## 如何移植、适配
s_ring_buffer中与平台相关的接口实现均在`s_ring_buffer_platform.h`和`s_ring_buffer_platform.c`中，用户主要需要适配实现的包括：
宏`s_ring_buff_sleep`、`s_ring_buff_memcpy`、`s_ring_buff_malloc`、`s_ring_buff_free`
接口`s_ring_buffer_lock()`、`s_ring_buffer_try_lock()`、`s_ring_buffer_unlock()`



