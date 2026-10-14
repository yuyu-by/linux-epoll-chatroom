#include "Link.h"
#include <sys/socket.h>
void broadcast(Link head,int sender_fd,const char *message,size_t len)
{
    if(head==NULL)
    {
        return;
    }
    Link p=head;
    do{
        if(p->fd!=sender_fd)
        {
send(p->fd,message,len,MSG_NOSIGNAL);
        }
        p=p->next;
    
    }while(p!=head);
}
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