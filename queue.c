#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q_head =
        (struct list_head *) malloc(sizeof(struct list_head));

    if (q_head)
        INIT_LIST_HEAD(q_head);

    return q_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *element, *safe;

    list_for_each_entry_safe (element, safe, head, list) {
        q_release_element(element);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *element = (element_t *) malloc(sizeof(element_t));

    if (!element || !head)
        return false;

    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }

    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *element = (element_t *) malloc(sizeof(element_t));

    if (!element || !head)
        return false;

    element->value = strdup(s);
    if (!element->value) {
        free(element);
        return false;
    }

    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_first_entry(head, element_t, list);
    list_del(&element->list);

    if (sp && bufsize > 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_last_entry(head, element_t, list);
    list_del(&element->list);

    if (sp && bufsize > 0) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int size = 0;
    struct list_head *iterator;
    list_for_each (iterator, head) {
        ++size;
    }

    return size;
}

struct list_head *find_mid_node(struct list_head *head)
{
    struct list_head *fast, *slow, *tail;

    fast = slow = head->next;
    tail = head->prev;

    // queue size > 1
    if (slow != tail) {
        while (fast != head && fast != tail) {
            fast = fast->next->next;
            if (fast != head)
                slow = slow->next;
        }
    }
    return slow;
}


/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

    struct list_head *mid;

    mid = find_mid_node(head);

    list_del(mid);
    element_t *mid_element = list_entry(mid, element_t, list);
    q_release_element(mid_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head)
        return false;

    struct list_head *node, *safe;
    bool delete = false;

    list_for_each_safe (node, safe, head) {
        element_t *cur = list_entry(node, element_t, list);
        element_t *next = list_entry(safe, element_t, list);


        if (safe != head && strcmp(cur->value, next->value) == 0) {
            list_del(node);
            q_release_element(cur);
            delete = true;
        } else if (delete) {
            list_del(node);
            q_release_element(cur);
            delete = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    struct list_head *node, *safe, *pre = head;

    list_for_each_safe (node, safe, head) {
        if (node->next != head) {
            pre->next = safe;
            node->next = safe->next;
            node->prev = safe;
            safe->next = node;
            safe->prev = pre;
            pre = node;

            safe = node->next;
        }


        if (safe == head)
            head->prev = node;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe, *tmp = head;


    list_for_each_safe (node, safe, head) {
        node->next = tmp;
        node->prev = safe;
        tmp = node;
    }
    head->prev = head->next;
    head->next = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k == 1)
        return;

    int times = 1;

    struct list_head *node, *safe;
    struct list_head tmp, rk_head;
    INIT_LIST_HEAD(&tmp);
    INIT_LIST_HEAD(&rk_head);

    list_for_each_safe (node, safe, head) {
        if (times == k) {
            list_cut_position(&tmp, head, node);
            q_reverse(&tmp);
            list_splice_tail_init(&tmp, &rk_head);
            times = 0;
        }
        ++times;
    }
    list_splice(&rk_head, head);
}

/* Merge tow sorted queues and set l to the queue head*/
void merge_sorted_queues(struct list_head *l, struct list_head *r, bool descend)
{
    struct list_head head;
    INIT_LIST_HEAD(&head);

    while (!list_empty(l) && !list_empty(r)) {
        struct list_head *selected, *l_first = l->next, *r_first = r->next;
        element_t *left_e = list_first_entry(l, element_t, list);
        element_t *right_e = list_first_entry(r, element_t, list);

        int cmp_result = strcmp(left_e->value, right_e->value);

        selected = descend ? ((cmp_result < 0) ? r_first : l_first)
                           : ((cmp_result > 0) ? r_first : l_first);

        list_move_tail(selected, &head);
    }

    list_splice_tail_init(list_empty(l) ? r : l, &head);
    list_splice_tail(&head, l);
}


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* left and right sub queue*/
    struct list_head l, r;
    struct list_head *mid;

    INIT_LIST_HEAD(&l);
    INIT_LIST_HEAD(&r);

    mid = find_mid_node(head);

    /* After cut
     * l will point to the left part of the queue relative to the mid position.
     * otherwise head will point to the right part (excluding mid) */
    list_cut_position(&l, head, mid);

    q_sort(&l, descend);
    q_sort(head, descend);
    merge_sorted_queues(&l, head, descend);
    list_splice_tail(&l, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    int amount = 1;

    struct list_head *min = head->prev, *node = head->prev->prev, *next;


    for (next = node->prev; node != (head); node = next, next = node->prev) {
        element_t *element = list_entry(node, element_t, list);
        element_t *cur_min_element = list_entry(min, element_t, list);

        if (strcmp(cur_min_element->value, element->value) < 0) {
            list_del(&element->list);
            q_release_element(element);
        } else {
            min = node;
            ++amount;
        }
    }
    return amount;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    int amount = 1;

    struct list_head *max = head->prev, *node = head->prev->prev, *next;


    for (next = node->prev; node != (head); node = next, next = node->prev) {
        element_t *element = list_entry(node, element_t, list);
        element_t *cur_max_element = list_entry(max, element_t, list);

        if (strcmp(cur_max_element->value, element->value) > 0) {
            list_del(&element->list);

            /* should "delete" the element, otherwise cant pass trace-06-ops */
            q_release_element(element);
        } else {
            max = node;
            ++amount;
        }
    }
    return amount;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    if (!head || list_empty(head))
        return 0;

    queue_contex_t *target = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *other = NULL;

    list_for_each_entry (other, head, chain) {
        if (target == other)
            continue;
        merge_sorted_queues(target->q, other->q, descend);
        other->size = 0;
    }

    return q_size(target->q);
}
