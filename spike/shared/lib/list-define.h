/* Linux-like double-linked list implementation */

#ifndef SYSPROG21_LIST_DEFINE_H
#define SYSPROG21_LIST_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * struct list_head - Head and node of a double-linked list
 * @prev: pointer to the previous node in the list
 * @next: pointer to the next node in the list
 *
 * The simple double-linked list consists of a head and nodes attached to
 * this head. Both node and head share the same struct type. The list_*
 * functions and macros can be used to access and modify this data structure.
 *
 * The @prev pointer of the list head points to the last list node of the
 * list and @next points to the first list node of the list. For an empty list,
 * both member variables point to the head.
 *
 * The list nodes are usually embedded in a container structure which holds the
 * actual data. Such an container object is called entry. The helper list_entry
 * can be used to calculate the object address from the address of the node.
 */
struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

#ifdef __cplusplus
}
#endif

#endif /* SYSPROG21_LIST_DEFINE_H */
