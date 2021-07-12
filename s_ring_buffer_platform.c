#include <stdio.h>
#include <stdlib.h>
#include "s_ring_buffer_platform.h"

/**
 * s_ring_buffer上锁
 */
void s_ring_buffer_lock(s_ring_buffer_lock_t* s_ring_buffer_lock_p)
{
    while(*s_ring_buffer_lock_p == 1) {
        s_ring_buff_sleep(1);
    }
    *s_ring_buffer_lock_p = 1;

    return;
}

/**
 * s_ring_buffer尝试上锁
 */
int s_ring_buffer_try_lock(s_ring_buffer_lock_t* s_ring_buffer_lock_p)
{
    while(*s_ring_buffer_lock_p == 1) {
        return 0;
    }
    *s_ring_buffer_lock_p = 1;

    return 1;
}

/**
 * s_ring_buffer解锁
 */
void s_ring_buffer_unlock(s_ring_buffer_lock_t* s_ring_buffer_lock_p)
{
    *s_ring_buffer_lock_p = 0;

    return;
}