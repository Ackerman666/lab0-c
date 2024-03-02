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

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

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

    list_del(slow);
    element_t *mid_element = list_entry(slow, element_t, list);
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

    list_for_each_safe (node, safe, head) {
        element_t *cur = list_entry(node, element_t, list);
        element_t *next = list_entry(safe, element_t, list);


        if (safe != head && strcmp(cur->value, next->value) == 0) {
            safe = safe->next;
            list_del(node);
            list_del(node->next);
            q_release_element(cur);
            q_release_element(next);
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
}


// Todo
struct list_head *merge_two_list(struct list_head *left,
                                 struct list_head *right)
{
    struct list_head head;
    struct list_head *h = &head;
    if (!left && !right) {
        return NULL;
    }
    while (left && right) {
        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) < 0) {
            h->next = left;
            left = left->next;
            h = h->next;
        } else {
            h->next = right;
            right = right->next;
            h = h->next;
        }
    }
    // after merge, there are still one node still not connect yet

    if (left) {
        h->next = left;
    } else if (right) {
        h->next = right;
    }
    return head.next;
}

// todo
struct list_head *merge_recur(struct list_head *head)
{
    if (!head->next)
        return head;

    struct list_head *slow = head;
    // split list
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next) {
        slow = slow->next;
    }

    struct list_head *mid = slow->next;  // the start node of right part
    slow->next = NULL;

    struct list_head *left = merge_recur(head);
    struct list_head *right = merge_recur(mid);

    return merge_two_list(left, right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    // disconnect the circular structure
    head->prev->next = NULL;
    head->next = merge_recur(head->next);
    // reconnect the list (prev and circular)
    struct list_head *c = head, *n = head->next;
    while (n) {
        n->prev = c;
        c = n;
        n = n->next;
    }
    c->next = head;
    head->prev = c;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
