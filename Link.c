#include "Link.h"
void insert_client(Link *head, Link new_node)
{
    if (head == NULL || new_node == NULL)
    {
        return;
    }
    else if (*head == NULL)
    {
        new_node->next = new_node;
        new_node->prev = new_node;
        *head = new_node;
        return;
    }
    new_node->next = (*head)->next;
    (*head)->next->prev = new_node;
    new_node->prev = *head;
    (*head)->next = new_node;
}
void remove_client(Link *head, Link target)
{
    if (target == NULL || head == NULL || *head == NULL)
    {
        return;
    }
    else if (target->next == target)
    {
        *head = NULL;
        return;
    }
    target->prev->next = target->next;
    target->next->prev = target->prev;
    if (target == *head)
    {
        *head = target->next;
    }
}