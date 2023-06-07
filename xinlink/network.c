#include<network.h>
#include <core.h>
#include <event.h>
#include <core.h>
XLins_queue_head send_queue_head;
XLins_queue_head recv_queue_head;
XLins_queue_head total_queue_head;


uint16_t pak_id=0;

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

//------------Local_infomations------------//
XLnet * network_get_local_info(void)
{
    XLnet * net_info=malloc(sizeof(XLnet));
    net_info->ip=inet_addr("192.168.1.7");
    //net_info->mac=;
    net_info->port=8081;
    return  net_info;
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


    serveraddr.sin_addr.s_addr =inet_addr("192.168.1.41");//ip地址
    serveraddr.sin_port = htons(8081);

    //serveraddr.sin_addr.s_addr = net->ip;//ip地址
    //serveraddr.sin_port = htons(net->port);

    if(sendto(sockfd, data, datasize, 0, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to sendto");
    }
    //第四步：关闭套接字文件描述符
    close(sockfd);
    return 1;
}
//---------Packet Make and Decode----------//

//part of instruction
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

    //Read sender and receiver info

    str * str_tmp;
    int seek;

    switch (ins->mode&SEE_SENDER_ONLY) {
        case NETWORK_MODE_SENDER_EVENT_ID:
            //ins->send_event_id=*(event_id_t *)data_p;
            ins_sender_set_id(ins,MODE_EVENT_ID,*(event_id_t*)data_p);
            data_p+=sizeof (event_id_t);
            printf("1:sender is event id\n");
            break;
        case NETWORK_MODE_SENDER_PERMIT:
            printf("2:sender is permit\n");
            break;
        default:
            free(data);
            return NULL;
            break;
    }
    switch (ins->mode&SEE_RECEIVER_ONLY) {
        case NETWORK_MODE_RECEIVER_EVENT_ID:
            //ins->send_event_id=*(event_id_t*)data_p;
            ins_sender_set_id(ins,MODE_EVENT_ID,*(event_id_t*)data_p);
            data_p+=sizeof (event_id_t);
            printf("1:receiver is event id\n");
            break;
        case NETWORK_MODE_RECEIVER_PERMIT:
            printf("2:receiver is permit\n");
            break;
        case NETWORK_MODE_RECEIVER_START_EVENT:
            str_tmp=get_str(data_p,&seek,PAK_MAX_SIZE);
            if(str_tmp==NULL){
                free(data);
                return NULL;
            }
            //strcpy((str* )ins->recv_app_name,str_tmp);
            ins_recevier_set_appname(ins,str_tmp);
            data_p+=seek;
            free(str_tmp);
            printf("3:receiver is start event\n");
            break;
        default:
            free(data);
            return NULL;
            break;
    }
    //Read INS
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

    //Write sender and receiver info

    switch (ins->mode&SEE_SENDER_ONLY) {
        case NETWORK_MODE_SENDER_EVENT_ID:
            *(event_id_t*)data_p=ins->sender.id;
            data_p+=sizeof (event_id_t);
            printf("1:sender is event id\n");
            break;
        case NETWORK_MODE_SENDER_PERMIT:
            printf("2:sender is permit\n");
            break;
        default:
            free(data);
            return NULL;
            break;
    }
    switch (ins->mode&SEE_RECEIVER_ONLY) {
        case NETWORK_MODE_RECEIVER_EVENT_ID:
            *(event_id_t*)data_p=ins->sender.id;
            data_p+=sizeof (event_id_t);
            printf("1:receiver is event id\n");
            break;
        case NETWORK_MODE_RECEIVER_PERMIT:
            printf("2:receiver is permit\n");
            break;
        case NETWORK_MODE_RECEIVER_START_EVENT:
            strcpy((str *)data_p,(str *)ins->receiver.name);
            data_p+=strlen((str *)ins->receiver.name)+1;
            printf("3:receiver is start event\n");
            break;
        default:
            free(data);
            return NULL;
            break;
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
    *size=data_p-data;
    //ins_decode_data(data);
    return data;
}

//part of connect

uint16_t create_pak_id(void)
{
    extern uint16_t pak_id;
    pak_id++;
    return pak_id;
}

DATA * connect_make_data(core_id_t core_id,int req_or_acc,uint16_t pak_id){
    //Get core
    if(req_or_acc!=0&&req_or_acc!=1)return -1;
    XLcore * core=core_get_by_id(core_id);
        if(core==NULL)return -1;

        //Measure size
       int s=2+2+4+2+6+2; //|mode|size|ip|port|MAC|PAK_ID|

       s+=strlen(core->name)+1;
       //Write head
       DATA * data=malloc(sizeof (DATA)*s+20),*data_p=data;

       if(req_or_acc==0)
       *(uint16_t *)data_p=NETWORK_MODE_CONNECT_REQUEST;
       if(req_or_acc==1)
       *(uint16_t *)data_p=NETWORK_MODE_CONNECT_ACCEPT;

       data_p+=2;
       *(uint16_t *)data_p=(uint16_t)s;
       data_p+=2;

       *(IP *)data_p=core->net.ip;
       data_p+=sizeof(IP);
       *(PORT *)data_p=core->net.port;
       data_p+=sizeof(PORT);
       //*(PORT *)data_p=core->net.mac[1];
       data_p+=6;
       if(req_or_acc==0)*(uint16_t*)data_p=create_pak_id();
       if(req_or_acc==1)*(uint16_t*)data_p=pak_id;
       data_p+=2;

       //Write ins
       str * name_p=(str *)core->name;
       for(int i=0;i<(int)strlen(core->name)+1;i++) {
           *data_p=*name_p;
           name_p++;
           data_p++;
       }
       show_buf(data,s);
       connect_decode_data_and_connect(data);
       return data;
}

core_id_t connect_decode_data_and_connect(DATA * data){
    DATA * data_p=data;
    XLnet net;
    int datasize;
    //Get head of the network packets

    if(*(uint16_t *)data_p!=NETWORK_MODE_CONNECT_REQUEST)return -1;

    data_p+=2;
    datasize=*(uint16_t *)data_p;
    data_p+=2;
    net.ip=*(IP*)data_p;
    data_p+=4;
    net.port=*(PORT*)data_p;
    data_p+=2;
    //net.mac=(MAC*)data_p;
    data_p+=6;
    printf("IP:%d port:%d\n",net.ip,net.port);

    //Get core by Network information

    uint16_t pak_id;
    pak_id=*(uint16_t *)data_p;
    data_p+=2;
    //Read INS
    int seek;
    str * name=get_str(data_p,&seek,PAK_MAX_SIZE);
    printf("name:%s\n",name);

    core_id_t core_id=core_add(&net,name);
    if(core_id<=0){free(data);printf("sd\n");return -1;}

    printf("core_id:%d\n",core_id);
    printf("name:%s\n",core_get_by_id(core_id)->name);
    return 1;
}

//-----------------Thread-----------------//

void * ins_send_thread(void * arg){
    while (1) {
        while (1) {
            extern XLins_queue_head send_queue_head;
            if(send_queue_head.queue==NULL)break;
            XLins_queue * queue_now=send_queue_head.queue;
            if(queue_now->ins==NULL)break;

            XLcore *core=core_get_by_id(queue_now->ins->core_id);

            if(core<=0)break;
            int size=0;
            DATA * data=ins_make_data(queue_now->ins,&size);
            if(size==0||data==NULL)break;
            TCP_send(&core->net,data,size);
            queue_del(queue_send());
        }
    }
}

void * ins_recv_thread(void * arg)
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
            XLins *ins=ins_decode_data(data);
            queue_add(queue_total(),ins,0);
            //ins_send_to_event(&ins);
        }

        //关闭文件描述符
        close(sockfd);
     }
}

void * ins_total_thread(void * arg)
{
    while (1) {
        XLins_queue * queue_head=queue_total()->queue;
        if(queue_head!=NULL)ins_send_to_event(queue_head->ins);
        queue_del(queue_total());
        usleep(100);
    }
}

int network_thread_init(void)
{
    pthread_t send_thread,receive_thread,total_thread;
    pthread_create(&send_thread,NULL,ins_send_thread,NULL);
    if(!send_thread)perror("send thread");
    pthread_create(&receive_thread,NULL,ins_recv_thread,NULL);
    if(!receive_thread)perror("receive thread");
    pthread_create(&total_thread,NULL,ins_total_thread,NULL);
    if(!total_thread)perror("total thread");
    return 1;
}

//-----------------Queue-----------------//

int queue_del(XLins_queue_head *head)
{
    if(head==NULL)return -1;
    XLins_queue * queue_head=head->queue;
    if(queue_head==NULL)return -1;

    if(queue_head->next==queue_head){
        free(queue_head);
        head->queue=NULL;
        return 1;
    }
    queue_head->next->front=queue_head->front;
    queue_head->front->next=queue_head->next;
    XLins_queue * tmp=queue_head->next;
    free(queue_head);
    head->queue=tmp;
    return 1;
}

int queue_add(XLins_queue_head * head,XLins * ins,LEVEL level)
{
    if(ins==NULL||head==NULL)return -1;
    if(head->queue==NULL){
            head->queue=malloc(sizeof (XLins_queue));
            head->queue->ins=ins;
            head->queue->level=level;
            head->queue->front=head->queue;
            head->queue->next=head->queue;
            return 1;
    }
    XLins_queue * queue_head=head->queue;

    XLins_queue * ins_now=queue_head;
    while (1) {
        if(level>ins_now->level){
            XLins_queue * ins_new=malloc(sizeof (XLins_queue));
            ins_new->next=ins_now;
            ins_new->front=ins_now->front;

            ins_now->front->next=ins_new;
            ins_now->front=ins_new;

            ins_new->ins=ins;
            ins_new->level=level;
            if(ins_now==queue_head)head->queue=ins_new;

            return 1;
        }
        if(ins_now->next==queue_head)
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

void queue_show(XLins_queue_head * head){
    if(head==NULL)return;
    XLins_queue * queue_head=head->queue;
    if(queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    XLins_queue * ins_now=queue_head;
    while (1) {
        printf("%d ",ins_now->level);
        if(ins_now->next==queue_head)break;
        ins_now=ins_now->next;
    }
    printf("\n");
}

XLins_queue_head * queue_total(void)
{
    extern XLins_queue_head total_queue_head;
    return &total_queue_head;
}

XLins_queue_head * queue_send(void)
{
    extern XLins_queue_head send_queue_head;
    return &send_queue_head;
}
XLins_queue_head * queue_recv(void)
{
    extern XLins_queue_head recv_queue_head;
    return &recv_queue_head;
}
