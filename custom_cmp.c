#include "custom_cmp.h"
#include <string.h>
#include "list.h"
#include "queue.h"

int q_asc_cmp(void *, const struct list_head *l1, const struct list_head *l2)
{
    if (l1 == NULL || l2 == NULL)
        return 0;

    return strcmp(list_entry(l1, element_t, list)->value,
                  list_entry(l2, element_t, list)->value);
}

int q_desc_cmp(void *, const struct list_head *l1, const struct list_head *l2)
{
    if (l1 == NULL || l2 == NULL)
        return 0;

    return -strcmp(list_entry(l1, element_t, list)->value,
                   list_entry(l2, element_t, list)->value);
}