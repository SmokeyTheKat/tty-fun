#ifndef __LIST_H__
#define __LIST_H__

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#define LIST_EXPAND_SIZE 4

struct list
{
	char* data;
	intmax_t length;
	intmax_t capacity;
};

#define make_list(cap, type) (struct list){ \
		.data=malloc(sizeof(type)*cap), \
		.length=0, \
		.capacity=cap, \
	}

#define list_isempty(list) \
	(!(list)->length)

#define list_length(list, type) \
	((list)->length / sizeof(type))

#define list_capacity(list, type) \
	((list)->capacity / sizeof(type))

#define list_first(list, type) \
	(*(type*)((list)->data))

#define list_last(list, type) \
	(*(type*)(&(list)->data[(list)->length - sizeof(type)]))

#define list_next(list, itr, type) \
	(((itr+1) >= ((type*)(&(list)->data[(list)->length])) ? (0) : (itr+1)))

#define list_end(list, type) \
	(*(type*)(&(list)->data[(list)->length]))
	

#define list_iterate(list, var_name, type) \
	type* var_name = (type*)((list)->data); \
	(ptrdiff_t)var_name != (ptrdiff_t)&(list)->data[(list)->length]; \
	var_name++

#define list_expand(list, size, type) { \
		if (list_capacity((list), type) < size) \
		{ \
			(list)->capacity = (sizeof(type) * size) + \
						(sizeof(type) * LIST_EXPAND_SIZE); \
			(list)->data = realloc(list->data, (list)->capacity); \
		} \
	}

#define list_push(list, value, type) { \
		if ((list)->length + sizeof(type) >= (list)->capacity) \
		{ \
			(list)->capacity += (sizeof(type) * LIST_EXPAND_SIZE); \
			(list)->data = realloc((list)->data, (list)->capacity); \
		} \
		(*(type*)(&(list)->data[(list)->length])) = (value); \
		(list)->length += sizeof(type); \
	}

#define list_clear(list, ...) { \
		(list)->length = 0; \
	}

#define list_set(list, value, idx, type) { \
		((type*)(list)->data)[(idx)] = (value); \
	}

#define list_get(list, idx, type) \
		(((type*)(list)->data)[(idx)])

#define list_index_of(list, find, type) \
	__list_index_of((list), (find), sizeof(type))

intmax_t __list_index_of(struct list* list, void* find, intmax_t type_size)
{
	for (intmax_t i = 0; i < list->length; i += type_size)
		if (!memcmp(&list->data[i], (char*)find, type_size))
			return i / type_size;
	return -1;
}

#define list_replace(list, find, replace, type) { \
		for (intmax_t i = 0; i < list_length((list), type); i++) \
		{ \
			if (list_get((list), i, type) == (find)) \
			{ \
				list_set((list), (replace), i, type); \
				break; \
			} \
		} \
	}

#define list_pop_back(list, type) { \
		for (intmax_t i = 0; i < list_length((list), type); i++) \
			list_set((list), list_get((list), (i+1), type), i, type); \
		(list)->length -= sizeof(type); \
	}

#define list_remove(list, idx, type) { \
		for (intmax_t i = (idx); i < list_length((list), type); i++) \
			list_set((list), list_get((list), (i+1), type), i, type); \
		(list)->length -= sizeof(type); \
	}

#define list_add(dest, src, type) { \
		for (intmax_t i = 0; i < list_length((src), type); i++) \
			list_push((dest), list_get((src), i, type), type); \
	}

#define list_copy(dest, src, type) { \
		list_expand((dest), list_capacity((src), type), type); \
		for (intmax_t i = 0; i < list_length((src), type); i++) \
			list_push((dest), list_get((src), i, type), type) \
	}

#endif
