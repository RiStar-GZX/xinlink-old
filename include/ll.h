#ifndef LL_H
#define LL_H

#include <malloc.h>
#include <stdio.h>
#include <type.h>
#include <string.h>

typedef struct XLll_member
{
    uint size;
    void * data;
    struct XLll_member * front;
    struct XLll_member * next;
}XLll_member;

typedef struct XLll
{
    uint member_num;
    uint member_size;
    struct XLll_member * head;
    struct XLll_member * tail;
}XLll;

void data_sh(uint8_t *data,uint size);

XLll * ll_create(uint size);

int ll_add_member_tail(XLll * ll,void * member,uint member_size);

int ll_add_member_head(XLll * ll,void * member,uint member_size);

int ll_insert_member_next(XLll * ll,void * member,uint member_size,uint member_num);

int ll_insert_member_front(XLll * ll,void * member,uint member_size,uint member_num);

int ll_del_member(XLll * ll,uint member_num);

int ll_del_member_head(XLll * ll);

int ll_del_member_tail(XLll * ll);

void ll_show_member(XLll *ll);

void * ll_get_member(XLll * ll,uint member_num);

#endif // LL_H
