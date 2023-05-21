#include<network.h>
#include <core.h>
#include <event.h>
XLins_queue * send_queue_head;
XLins_queue * recv_queue_head;

//----------Packet production tool----------//
str * get_str(void *ins,int *seek,int maxsize)
{
     if(ins==NULL)return NULL;
     uint8_t * ins_p=ins;
     int i=1;
     while (*ins_p!='\0')
     {
         ins_p++;
         i++;
         if(i>maxsize)return NULL;
     }
     str * data=malloc(sizeof(uint8_t)*i),*data_p=data;
     ins_p=ins;
     for(int j=0;j<i;j++)
     {
         *data_p=*ins_p;
         data_p++;
         ins_p++;
     }
     *seek=i;
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


//----------------Network------------------//
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

int ins_recv_thread(void)
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
            XLins ins=*ins_decode_data(data);
            ins_send_to_event(&ins);
        }

        //关闭文件描述符
        close(sockfd);
     }
}

XLins * ins_decode_data(DATA *data){
    DATA * data_p=data;
    XLnet net;
    int datasize;
    XLins * ins=malloc(sizeof (XLins));

    //Get head of the network packets

    ins->mode=*(uint16_t *)data_p;
    data_p+=2;
    datasize=*(uint16_t *)data_p;
    if(datasize<14)return NULL;
    data_p+=2;
    net.ip=*(IP*)data_p;
    data_p+=4;
    net.port=*(PORT*)data_p;
    data_p+=2;
    //net.mac=(MAC*)data_p;
    data_p+=6;
    printf("IP:%d port:%d\n",net.ip,net.port);

    //Get core by Network information

    ins->core_id=core_get_by_net(&net)->id;
    if(ins->core_id<=0)return NULL;
    printf("core_id:%d\n",ins->core_id);

    //Process data according to the pattern of network packets
    str * str_temp;
    int sender_mode,receiver_mode,seek;
    sender_mode=ins->mode&SEE_SENDER_ONLY;
    receiver_mode=ins->mode&SEE_RECEIVER_ONLY;

    if(sender_mode==NETWORK_MODE_SENDER_DEV|sender_mode==NETWORK_MODE_SENDER_DEV_EVENT)
    {
        printf("sender is dev\n");
        str_temp=get_str(data_p,&seek,PAK_MAX_SIZE);
        if(str_temp==NULL)return NULL;
        data_p+=seek;
        strcpy(ins->send_dev,str_temp);
    }
    if(sender_mode==NETWORK_MODE_SENDER_EVENT|sender_mode==NETWORK_MODE_SENDER_DEV_EVENT)
    {
        printf("sender is event\n");
        str_temp=get_str(data_p,&seek,PAK_MAX_SIZE);
        if(str_temp==NULL)return NULL;
        data_p+=seek;
        strcpy(ins->send_event,str_temp);
    }
    //|DEV_NAME|EVENT_ID|INS
    if(receiver_mode==NETWORK_MODE_RECEIVER_DEV|receiver_mode==NETWORK_MODE_RECEIVER_DEV_EVENT)
    {
        str_temp=get_str(data_p,&seek,PAK_MAX_SIZE);
        if(str_temp==NULL)return NULL;
        data_p+=seek;
        strcpy(ins->recv_dev,str_temp);
        printf("my dev:%s\n",ins->recv_dev);
    }
    if(receiver_mode==NETWORK_MODE_RECEIVER_EXIST_EVENT)
    {
        ins->recv_event_id=*(app_id_t *)data_p;
        data_p+=sizeof (app_id_t);
        printf("my event id:%d\n",ins->recv_event_id);
    }
    ins->INS=get_str(data_p,&seek,PAK_MAX_SIZE);
    printf("ins:%s\n",ins->INS);
    ins_send_to_event(ins);
    return ins;
}
DATA * ins_make_data(XLins *ins,int * size){
    //Measure size
    int s=2+2+4+2+6; //|mode|size|ip|port|MAC|
    s+=strlen((str*)ins->INS)+1;


    //Write head
    DATA * data=malloc(sizeof (DATA)*s+20),*data_p=data;
    *(uint16_t *)data_p=ins->mode;
    data_p+=2;
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

    //write ex
    int sender_mode,receiver_mode,seek;
    sender_mode=ins->mode&SEE_SENDER_ONLY;
    receiver_mode=ins->mode&SEE_RECEIVER_ONLY;

    if(sender_mode==NETWORK_MODE_SENDER_DEV|sender_mode==NETWORK_MODE_SENDER_DEV_EVENT)
    {
        strcat((str*)data_p,ins->send_dev);
        data_p+=strlen(ins->send_dev)+1;
        printf("1:sender is dev\n");
    }
    if(sender_mode==NETWORK_MODE_SENDER_EVENT|sender_mode==NETWORK_MODE_SENDER_DEV_EVENT)
    {
        strcat((str*)data_p,ins->send_event);
        data_p+=strlen(ins->send_dev)+1;
        printf("1:sender is event\n");
    }

    if(receiver_mode==NETWORK_MODE_RECEIVER_DEV|receiver_mode==NETWORK_MODE_RECEIVER_DEV_EVENT)
    {
        strcat((str*)data_p,ins->recv_dev);
        data_p+=strlen(ins->send_dev)+1;
        printf("1:receiver is device\n");
    }
    if(receiver_mode==NETWORK_MODE_RECEIVER_EXIST_EVENT)
    {
        *(app_id_t *)data_p=ins->recv_event_id;
        data_p+=sizeof (app_id_t);
        printf("1:receiver exist event\n");
    }

    //Write ins
    INS * ins_p=ins->INS;
    if(sizeof (ins)>=PAK_MAX_SIZE)
    {
        free(data);
        return NULL;
    }
    for(int i=0;i<(int)strlen(ins->INS)+1;i++) {
        *data_p=*ins_p;
        ins_p++;
        data_p++;
    }
    *size=s;
    ins_decode_data(data);
    return data;
}
void * ins_send_thread(void * arg){
    while (1) {
        while (1) {
            extern XLins_queue * send_queue_head;
            if(send_queue_head==NULL)break;
            XLins_queue * queue_now=send_queue_head;
            if(queue_now->ins==NULL)break;

            XLcore *core=core_get_by_id(queue_now->ins->core_id);

            if(core<=0)break;
            int size=0;
            DATA * data=ins_make_data(queue_now->ins,&size);
            if(size==0||data==NULL)break;
            TCP_send(&core->net,data,size);
            send_queue_del();
        }
    }
}

int thread_init(void)
{
    pthread_t thread;
    pthread_create(&thread,NULL,ins_send_thread,NULL);
    if(!thread)perror("thread");
    //pthread_create(&thread,NULL,ins_recv_thread,NULL);
    return 1;
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
