/**
 * @file list.h
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __list_head_h__
#define __list_head_h__

#ifdef __GNUC__
#define proto_typeof __typeof__
#endif

struct __proto_list_node
{
  struct __proto_list_node* next;
  struct __proto_list_node* prev;
};

typedef struct __proto_list_node proto_list_t;

#define proto_container_of(ptr, type, member) \
  ((type *) ((char *) (ptr) - (unsigned long)(&((type *)0)->member)))

static inline void proto_list_init(proto_list_t* l)
{
  l->next = l->prev = l;
}

static inline void proto_list_insert_after(proto_list_t* l, proto_list_t* n)
{
  l->next->prev = n;
  n->next = l->next;

  l->next = n;
  n->prev = l;
}

static inline void proto_list_insert_before(proto_list_t* l, proto_list_t* n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

static inline void proto_list_remove(proto_list_t* n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

static inline int proto_list_isempty(const proto_list_t *l)
{
    return l->next == l;
}

static inline unsigned int proto_list_len(const proto_list_t *l)
{
    unsigned int len = 0;
    const proto_list_t *p = l;
    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

#define proto_list_entry(node, type, member) \
    proto_container_of(node, type, member)

#define proto_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define proto_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

#define proto_list_for_each_entry(pos, head, member) \
    for (pos = proto_list_entry((head)->next, proto_typeof(*pos), member); \
         &pos->member != (head); \
         pos = proto_list_entry(pos->member.next, proto_typeof(*pos), member))

#define proto_list_for_each_entry_safe(pos, n, head, member) \
    for (pos = proto_list_entry((head)->next, proto_typeof(*pos), member), \
         n = proto_list_entry(pos->member.next, proto_typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = proto_list_entry(n->member.next, proto_typeof(*n), member))

#define proto_list_first_entry(ptr, type, member) \
    proto_list_entry((ptr)->next, type, member)

#endif