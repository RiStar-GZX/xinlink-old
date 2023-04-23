#include<network.h>

XLnet * network_get_local_info(void)
{
    XLnet * net_info=malloc(sizeof(net_info));
    net_info->ip=inet_addr("192.168.1.7");
    //net_info->mac=;
    net_info->port=8081;
    return  net_info;
}
