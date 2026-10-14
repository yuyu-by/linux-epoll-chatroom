#ifndef LINK_H
#define LINK_H
#include"net_utils.h"

void insert_client(Link*head,Link new_node);
void remove_client(Link *head,Link target);
void broadcast(Link head,int sender_fd,const char *meaasge, size_t len);

#endif