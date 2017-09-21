
#ifndef _BUF_QUEUE_H_
#define _BUF_QUEUE_H_

#include <common.h>


typedef struct _buf_element
{
    uint addr;        
    uint sector_num;  
    u8*  buff;        
}buf_element_t;

typedef struct _buf_node
{
    buf_element_t element;
    struct _buf_node * next;
}buf_node_t;


int buf_queue_init(void);
int buf_queue_exit(void);
int buf_enqueue(buf_element_t* element);
int buf_dequeue(buf_element_t* element);
int buf_queue_empty(void);
int buf_queue_full(void);
int buf_queue_free_size(void);
int buf_queue_get_page_size(void);

#endif
