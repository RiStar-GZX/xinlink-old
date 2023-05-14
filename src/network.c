#include<network.h>
#include <core.h>
XLins_queue * send_queue_head;
XLins_queue * recv_queue_head;

str * get_ins(uint8_t *buf,int maxsize)
{
     if(buf==NULL)return NULL;
     uint8_t * buf_p=buf;
     int i=1;
     while (*buf_p!='\0')
     {
         buf_p++;
         i++;
         if(i>maxsize)return NULL;
     }
     str * data=malloc(sizeof(str)*i),*data_p=data;
     buf_p=buf;
     for(int j=0;j<i;j++)
     {
         *data_p=*buf_p;
         data_p++;
         buf_p++;
     }
     return data;
}

void show_buf(uint8_t *data,int size)       //展示数据包(调试用)
{
    int i=0;
    char *p =(char*)data;
    while (i<=size)
    {
        if(*p=='\0')printf(" ");
        else printf("%c",*p);
        p++;
        i++;
    }
    printf("\n");
}

int TCP_send(XLnet * net,DATA * data,int datasize)
{
    //ERROR
    if(net==NULL)return 0;

    //create socket
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
    }

    printf("sockfd = %d\n", sockfd);

    //第二步：填充服务器网络信息结构体 sockaddr_in
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    serveraddr.sin_family = AF_INET; //协议族，AF_INET：ipv4网络协议

    serveraddr.sin_addr.s_addr = net->ip;//ip地址
    serveraddr.sin_port = htons(net->port);

    if(sendto(sockfd, data, datasize, 0, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to sendto");
    }
    //第四步：关闭套接字文件描述符
    close(sockfd);
    return 1;
}

int TCP_receive(void)   //接受网络数据包的线程
 {
     while(1)
     {
        int sockfd; //文件描述符
        struct sockaddr_in serveraddr; //服务器网络信息结构体
        socklen_t addrlen = sizeof(serveraddr);
        //创建套接字
        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("fail to socket");
        }
        //填充服务器网络信息结构体
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = network_get_local_info()->ip;     //后面要传参
        serveraddr.sin_port = htons(network_get_local_info()->port);
        //将套接字与服务器网络信息结构体绑定
        if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0)
        {
            perror("fail to bind");
        }
        while(1)
        {
            //进行通信
            uint8_t * data=malloc(sizeof(uint8_t)*PAK_MAX_SIZE);
            struct sockaddr_in clientaddr;
            if(recvfrom(sockfd, data, PAK_MAX_SIZE, 0, (struct sockaddr *)&clientaddr, &addrlen) < 0)
            {
                perror("fail to recvfrom");
            }
            printf("[%s - %d]: %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), data);    //调试

        }

        //关闭文件描述符
        close(sockfd);
     }
}

XLins * ins_decode_data(DATA *data,int size){
    DATA * data_p=data;
    XLnet net;
    int pak_size;
    if(size<=13)return  NULL;
    XLins * ins=malloc(sizeof (XLins));

    ins->mode=*data_p;
    if(ins->mode==NETWORK_MODE_COMMON_INS)
    {
        data_p++;
        pak_size=(int)*(uint16_t *)data_p;
        data_p+=2;
        net.ip=*(IP*)data_p;
        data_p+=4;
        net.port=*(PORT*)data_p;
        data_p+=2;
        //net.mac=*(MAC*)data_p;
        data_p+=6;

        printf("IP:%d port:%d\n",net.ip,net.port);
        XLcore *core;
        core=core_get_by_net(&net);
        if(core==NULL)return NULL;
        printf("core_id:%d\n",core->id);
        ins->core_id=core->id;
        printf("get str:%s\n",get_ins(data_p,pak_size-15));
    }
    else if(*data_p==NETWORK_MODE_START_INS)
    {

    }
    else return NULL;
}
DATA * ins_make_data(XLins *ins,int * size){
    //Measure size
    int s=1+2+4+2+6; //|mode|size|ip|port|MAC|INS|
    s+=strlen((str*)ins->ins)+1;

    //Write head
    DATA * data=malloc(sizeof (DATA)*s),*data_p=data;
    *data_p=NETWORK_MODE_COMMON_INS;
    data_p++;
    *(uint16_t *)data_p=(uint16_t)s;
    data_p+=2;
    XLcore *core=core_get_by_id(ins->core_id);
    if(core==NULL)
    {
        free(data);
        return NULL;
    }
    *(IP *)data_p=core->net.ip;
    data_p+=sizeof(IP);
    *(PORT *)data_p=core->net.port;
    data_p+=sizeof(PORT);
    //*(PORT *)data_p=core->net.mac[1];
    data_p+=6;

    //Write ins
    INS * ins_p=ins->ins;
    if(sizeof (ins)>=PAK_MAX_SIZE)
    {
        free(data);
        return NULL;
    }
    for(int i=0;i<(int)strlen(ins->ins)+1;i++) {
        *data_p=*ins_p;
        ins_p++;
        data_p++;
    }
    *size=s;
    return data;
}
void send_ins_thread(void){
    while (1) {
        while (1) {
            printf("ins!\n");
            extern XLins_queue * send_queue_head;
            if(send_queue_head==NULL)break;
            XLins_queue * queue_now=send_queue_head;
            if(queue_now->ins==NULL)break;
            XLcore *core=core_get_by_id(queue_now->ins->core_id);
            if(core<=0)break;
            int size=0;
            DATA * data=ins_make_data(queue_now->ins,&size);
            if(size==0||data==NULL)break;
            show_buf(data,size);
            ins_decode_data(data,size);
            TCP_send(&core->net,data,size);
            //printf("TCP send!\n");
            send_queue_del();
        }
        printf("no ins!\n");
        int a;
        scanf("%d",&a);
    }
}

XLnet * network_get_local_info(void)
{
    XLnet * net_info=malloc(sizeof(XLnet));
    net_info->ip=inet_addr("192.168.1.7");
    //net_info->mac=;
    net_info->port=8081;
    return  net_info;
}

int send_queue_del(void)
{

    extern XLins_queue * send_queue_head;

    if(send_queue_head==NULL)return 0;
    if(send_queue_head->next==send_queue_head){
        free(send_queue_head);
        send_queue_head=NULL;
        return 1;
    }
    send_queue_head->next->front=send_queue_head->front;
    send_queue_head->front->next=send_queue_head->next;
    XLins_queue * tmp=send_queue_head->next;
    free(send_queue_head);
    send_queue_head=tmp;
    return 1;
}

int send_queue_add(XLins * ins,LEVEL level)
{
    extern XLins_queue * send_queue_head;
    if(send_queue_head==NULL)
    {
        send_queue_head=malloc(sizeof (XLins_queue));
        send_queue_head->ins=ins;
        send_queue_head->level=level;
        send_queue_head->front=send_queue_head;
        send_queue_head->next=send_queue_head;
        return 1;
    }
    XLins_queue * ins_now=send_queue_head;
    while (1) {
        if(level>ins_now->level){
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now;
            ins_new->front=ins_now->front;

            ins_now->front->next=ins_new;
            ins_now->front=ins_new;

            ins_new->ins=ins;
            ins_new->level=level;
            if(ins_now==send_queue_head)send_queue_head=ins_new;

            return 1;
        }
        if(ins_now->next==send_queue_head)
        {
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now->next;
            ins_new->front=ins_now;

            ins_now->next->front=ins_new;
            ins_now->next=ins_new;

            ins_new->ins=ins;
            ins_new->level=level;
            return 1;
        }
        ins_now=ins_now->next;
    }
    return 0;
}

void send_queue_show(void){
    extern XLins_queue * send_queue_head;
    if(send_queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    XLins_queue * ins_now=send_queue_head;
    while (1) {
        printf("%d ",ins_now->level);
        if(ins_now->next==send_queue_head)break;
        ins_now=ins_now->next;
    }
    printf("\n");
}

int recv_queue_del(void)
{
    extern XLins_queue * recv_queue_head;
    if(recv_queue_head==NULL)return 0;
    if(recv_queue_head->next==recv_queue_head){
        free(recv_queue_head);
        send_queue_head=NULL;
        return 1;
    }
    recv_queue_head->next->front=recv_queue_head->front;
    recv_queue_head->front->next=recv_queue_head->next;
    XLins_queue * tmp=recv_queue_head->next;
    free(recv_queue_head);
    recv_queue_head=tmp;
    return 1;
}

int recv_queue_add(XLins * ins,LEVEL level)
{
    extern XLins_queue * recv_queue_head;
    if(recv_queue_head==NULL)
    {
        recv_queue_head=malloc(sizeof (XLins_queue));
        recv_queue_head->ins=ins;
        recv_queue_head->level=level;
        recv_queue_head->front=recv_queue_head;
        recv_queue_head->next=recv_queue_head;
        return 1;
    }
    XLins_queue * ins_now=recv_queue_head;
    while (1) {
        if(level>ins_now->level){
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now;
            ins_new->front=ins_now->front;

            ins_now->front->next=ins_new;
            ins_now->front=ins_new;


            ins_new->ins=ins;
            ins_new->level=level;
            if(ins_now==recv_queue_head)recv_queue_head=ins_new;

            return 1;
        }
        if(ins_now->next==recv_queue_head)
        {
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now->next;
            ins_new->front=ins_now;

            ins_now->next->front=ins_new;
            ins_now->next=ins_new;


            ins_new->ins=ins;
            ins_new->level=level;
            return 1;
        }
        ins_now=ins_now->next;
    }
    return 0;
}

void recv_queue_show(void){
    extern XLins_queue * recv_queue_head;
    if(recv_queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    XLins_queue * ins_now=recv_queue_head;
    while (1) {
        printf("%d ",ins_now->level);
        if(ins_now->next==recv_queue_head)break;
        ins_now=ins_now->next;
    }
    printf("\n");
}
