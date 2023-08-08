#include<network.h>
#include <core.h>
#include <event.h>
#include <core.h>

XLqueue_head send_queue_head;
XLqueue_head recv_queue_head;
XLqueue_head broadcast_queue_head;
XLqueue_head total_queue_head;


uint16_t pak_id=0;

//----------------------------------------------//
//               网络数据包制作工具                //
//----------------------------------------------//

//将数据包中的数据（*data）用16进制打印到屏幕上
void data_show(DATA *data,uint size){
    DATA *data_p=data;
    for(int i=0;i<size;i++){
        if(*data_p=='\0')printf("!");
        else printf("%X",*data_p);
        data_p++;
    }
    printf("\n");
}

//从一个结构体（*_struct）中复制（size）个大小的数据到另一个地址（* data），并将指针（*p）大小加上size
void data_add(DATA *data,int * p,void * _struct,uint size){
    if(data==NULL||_struct==NULL)return;
    DATA * data_p=data;
    if(p!=NULL)data_p+=*p;
    int i;
    for(i=0;i<size;i++){
        *data_p=*(DATA*)_struct;
        data_p++;
        _struct++;
    }
    if(p!=NULL)*p+=i;
}

//将字符串（*str）复制到另一个地址（* data），并将指针（*p）大小加上字符串的大小
void data_add_str(DATA *data,int * p,char * str){
    data_add(data,p,str,strlen(str)+1);
}

//从一个地址数据（*data）中复制（size）个位到（* _struct），并将指针（*p）大小加上size
void data_get(DATA *data,int * p,void * _struct,uint size){
    if(data==NULL||_struct==NULL)return;
    DATA * data_p=data,*struct_p=_struct;
    if(p!=NULL)data_p+=*p;
    int i;
    for(i=0;i<size;i++){
        *struct_p=*data_p;
        data_p++;
        struct_p++;
    }
    if(p!=NULL)*p+=i;
}

//从一个地址数据（*data）中自动发现字符串并复制到（* str），并将指针（*p）大小加上size
char * data_get_str(DATA *data,int *p){
    int len=strlen(data+*p)+1;
    if(len>128)return NULL;
    char *s=malloc(sizeof(char)*len);
    data_get(data,p,s,len);
    return s;
}

//----------------------------------------------//
//                   本地信息                    //
//----------------------------------------------//

//获得本机的网络信息，返回以XLnet网络结构体
XLnet network_get_local_info(void)
{
    XLnet net_info;
    //FILE * fd=fopen("../ip.txt","rb+");
    //if(fd==NULL)printf("open fail!\n");
    //str * ip;
    //fseek(fd,0,SEEK_SET);

    //if(fgets(ip,15,fd)==NULL)printf("ip fail!\n");
    //printf("ip:%s\n",ip);
    //net_info.ip=inet_addr(ip);
    //net_info.ip=inet_addr("192.168.0.103");
    net_info.ip=inet_addr("192.168.1.15");

    //net_info->mac=;
    net_info.port=8081;
    //fclose(fd);
    return net_info;
}

//----------------------------------------------//
//                  网络包发送                   //
//----------------------------------------------//

//发送TCP数据包（现在是UDP没改），接收者为XLnet网络结构体对应的网络，发送的内容是(*data),大小为（datasize）
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

    //printf("sockfd = %d\n", sockfd);

    //第二步：填充服务器网络信息结构体 sockaddr_in
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    serveraddr.sin_family = AF_INET; //协议族，AF_INET：ipv4网络协议

    //serveraddr.sin_addr.s_addr =inet_addr("192.168.1.41");//ip地址
    //serveraddr.sin_port = htons(8081);

    serveraddr.sin_addr.s_addr = net->ip;//ip地址
    serveraddr.sin_port = htons(net->port);

    if(sendto(sockfd, data, datasize, 0, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to sendto");
    }
    printf("TCP send\n");
    //第四步：关闭套接字文件描述符
    close(sockfd);
    return 1;
}

//发送广播数据包，发送的内容是(*buf),大小为（bufsize）
int Broadcast_send (void *buf,int bufsize)
{

     int sockfd;
     struct sockaddr_in broadcataddr; //服务器网络信息结构体
     socklen_t addrlen = sizeof(broadcataddr);
     //创建套接字
     if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
     {
         perror("fail to socket");
         return 0;
     }
     //设置为允许发送广播权限
     int on = 1;
     if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
     {
         perror("fail to setsockopt");
         return 0;
     }
     //填充广播信息结构体
     broadcataddr.sin_family = AF_INET;
     broadcataddr.sin_addr.s_addr =inet_addr("255.255.255.255"); //192.168.3.255 255.255.255.255
     broadcataddr.sin_port = htons(NETWORK_BROADCAST_PORT);
     //第四步:进行通信

     if(sendto(sockfd, buf,bufsize, 0, (struct sockaddr *)&broadcataddr, addrlen) < 0)
     {
         perror("fail to sendto");
         return 0;
     }
     //关闭套接字文件描述符
     close(sockfd);
     return 0;
}

//----------------------------------------------//
//                 网络包服务线程                 //
//----------------------------------------------//

//指令发送线程，读取发送队列中的数据包结构体(XLpak_xxx)，将它们转换为buf然后通过TCP发送
void * ins_send_thread(void * arg){
    while (1) {
        while (1) {
            extern XLqueue_head send_queue_head;
            if(send_queue_head.queue==NULL)break;
            XLqueue * queue_now=send_queue_head.queue;
            //发送的包为指令
            if(queue_now->mode==QUEUE_TYPE_INS){
                XLcore *core=core_get_by_net(&queue_now->in.pak_ins.base.net_receiver);
                if(core==NULL)break;
                int size=0;
                DATA * data=pak_ins_to_buf(&queue_now->in.pak_ins,&size);
                if(size==0||data==NULL)break;
                TCP_send(&core->net,data,size);
                break;
            }
            //发送的包为对接同意
            else if(queue_now->mode==QUEUE_TYPE_CONNECT){
                int size;
                DATA * data=pak_connect_to_buf(&queue_now->in.pak_connect,&size);
                TCP_send(&queue_now->in.pak_connect.base.net_receiver,data,size);
                free(data);
                break;
            }
            else if(queue_now->mode==QUEUE_TYPE_SIGN){
                int size;
                DATA * data=pak_sign_to_buf(&queue_now->in.pak_sign,&size);
                TCP_send(&queue_now->in.pak_sign.base.net_receiver,data,size);
                printf("SEND!\n");
                free(data);
                break;
            }
            else {
                break;
            }
        }
        queue_del_head(queue_send());
        usleep(100);//没有这个延时程序会崩
    }
}

//指令接收线程，将网络buf转化为数据包结构体（XLpak_xxx）,对它进行处理或转发
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
        serveraddr.sin_addr.s_addr = network_get_local_info().ip;     //后面要传参
        serveraddr.sin_port = htons(network_get_local_info().port);
        //将套接字与服务器网络信息结构体绑定
        if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0)
        {
            perror("fail to bind");
        }
        while(1)
        {
            //进行通信
            uint8_t * data=malloc(sizeof(uint8_t)*PAK_MAX_SIZE);
            memset(data,0,sizeof(uint8_t)*PAK_MAX_SIZE);
            struct sockaddr_in clientaddr;
            if(recvfrom(sockfd, data, PAK_MAX_SIZE, 0, (struct sockaddr *)&clientaddr, &addrlen) < 0)
            {
                perror("fail to recvfrom");
            }
            printf("[%s - %d]: %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), data);    //调试

            XLnet net;
            net.port=clientaddr.sin_port;
            net.ip=clientaddr.sin_addr.s_addr;

            //解码buf
            uint16_t mode=*(uint16_t*)data;
            uint16_t type=mode>>12;

            //数据包的模式是发送
            if(type==NETWORK_MODE_CONNECT){
                XLpak_connect * pak_connect;
                pak_connect=buf_to_pak_connect(data);
                uint16_t connect=mode&SEE_CONNECT_ONLY;
                //对方发送核心信息
                if(connect==PAK_SEND_CORE){
                    if(core_add(&pak_connect->base.net_sender,pak_connect->core_name)!=-2)printf("发现新核心:%s\n",pak_connect->core_name);
                    else printf("发现重复核心:%s\n",pak_connect->core_name);
                }
                //对方发送核心对接请求
                if(connect==PAK_CONNECT_REQUEST){
                    XLcore *core=core_get_by_net(&pak_connect->base.net_sender);
                    if(core==NULL)continue;
                    core->safety=CORE_STATE_VERIFIED;
                    printf("已设置核心%s为安全核心\n",core->name);
                    network_core_connect_require_receive(pak_connect);
                }
                //对方同意核心对接
                if(connect==PAK_CONNECT_ACCEPT){
                    network_core_connect(pak_connect);
                    printf("核心已通过验证\n");
                }
                //对方取消核心对接
                if(connect==PAK_DISCONNECT){
                    XLcore *core=core_get_by_net(&pak_connect->base.net_sender);
                    if(core==NULL)continue;
                    core->safety=CORE_STATE_UNVERIFIED;
                    printf("与核心%s的连接断开\n",core->name);
                }
            }
            //数据包的模式是接收
            else if(type==NETWORK_MODE_INS&&network_safe(&net)>0){
                printf("接收到了一条指令\n");
                XLpak_ins *pak_ins;
                if((pak_ins=buf_to_pak_ins(data))!=NULL)queue_add_ins(queue_total(),pak_ins,0);
                free(pak_ins);
            }
            else if(type==NETWORK_MODE_SIGN&&network_safe(&net)>0){
                printf("接收到了一条SIGN\n");
                XLpak_sign *pak_sign;
                if((pak_sign=buf_to_pak_sign(data))!=NULL){
                    XLcore * core=core_get_by_net(&pak_sign->base.net_sender);
                    if(core!=NULL){
                        core_add_sign(core->id,pak_sign->sign_list,0);
                    }
                }
                free(pak_sign);
            }
            free(data);
        }

        //关闭文件描述符
        close(sockfd);
     }
}

//广播发送线程，读取广播发送队列中的数据包结构体(XLpak_xxx)，将它们转换为buf然后通过广播发送
void * broadcast_send_thread(void * arg){
    while (1) {
        while (1) {
            extern XLqueue_head broadcast_queue_head;
            if(broadcast_queue_head.queue==NULL)break;
            XLqueue * queue_now=broadcast_queue_head.queue;
            //发送的包为对接
            if(queue_now->mode==NETWORK_MODE_CONNECT) {
                int size;
                DATA * data=pak_connect_to_buf(&queue_now->in.pak_connect,&size);
                //printf("BC send!\n");
                Broadcast_send(data,size);
                free(data);
                break;
            }
            else {
                break;
            }
        }
        queue_del_head(queue_broadcast());
        usleep(10000);
    }
}

//广播接收线程，将网络buf转化为数据包结构体（XLpak_connect），对它进行处理
void * broadcast_receive_thread(void * arg)
{
    int sockfd; //文件描述符
    struct sockaddr_in broadcataddr;
    socklen_t addrlen = sizeof(broadcataddr);
    //第一步:创建套接字
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
    perror("fail to socket");
    }
    //第二步:填充广播信息结构体
    broadcataddr.sin_family = AF_INET;
    broadcataddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    broadcataddr.sin_port = htons(8088);
    //第三步:将套接字与广播信息结构体绑定
    if(bind(sockfd, (struct sockaddr *)&broadcataddr, addrlen) < 0)
    {
        perror("fail to bind");
    }
    //第四步:进行通信
    char * data=malloc(sizeof(char)*PAK_MAX_SIZE);
    struct sockaddr_in sendaddr;
    while(1){
        if(recvfrom(sockfd, data,PAK_MAX_SIZE, 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            perror("fail to recvfrom");
        }
        uint16_t mode=*(uint16_t*)data;
        uint16_t type=mode>>12;
        uint16_t connect=mode&SEE_CONNECT_ONLY;
        if(type==NETWORK_MODE_CONNECT&&connect==PAK_SEND_CORE_REQUIRE){
            XLpak_connect *pak_connect;
            if((pak_connect=buf_to_pak_connect(data))==NULL)continue;
            if(pak_connect->base.net_sender.ip==network_get_local_info().ip)continue;
            //核心不重复
            if(network_core_find_receive(pak_connect)!=-2)printf("发现新核心:%s\n",pak_connect->core_name);
            else printf("发现重复核心:%s\n",pak_connect->core_name);
            free(pak_connect);
        }
    }
    return NULL;
}

//总线程，用于处理收到的指令
void * total_thread(void * arg)
{
    while (1) {
        XLqueue_head * queue_head=queue_total();
        if(queue_head!=NULL&&queue_head->queue!=NULL){
            if(queue_head->queue->mode==QUEUE_TYPE_INS){
                ins_send_to_event(&queue_head->queue->in.pak_ins);
            }
        }
        queue_del_head(queue_total());
        usleep(100);
    }
}

//线程初始化
int network_thread_init(void)
{
    pthread_t send,receive,total,broad_send,broad_recv;
    pthread_create(&send,NULL,ins_send_thread,NULL);
    if(!send)perror("send thread");
    pthread_create(&receive,NULL,ins_recv_thread,NULL);
    if(!receive)perror("receive thread");
    pthread_create(&total,NULL,total_thread,NULL);
    if(!total)perror("total thread");
    pthread_create(&broad_send,NULL,broadcast_send_thread,NULL);
    if(!broad_send)perror("total thread");
    pthread_create(&broad_recv,NULL,broadcast_receive_thread,NULL);
    if(!broad_recv)perror("total thread");

    return 1;
}

//----------------------------------------------//
//                      队列                     //
//----------------------------------------------//

//删除队列中的队头成员，输入的是队头的指针
int queue_del_head(XLqueue_head *head)
{
    if(head==NULL)return -1;
    XLqueue * queue_head=head->queue;
    if(queue_head==NULL)return -1;

    if(queue_head->next==queue_head){
        free(queue_head);
        head->queue=NULL;
        return 1;
    }
    queue_head->next->front=queue_head->front;
    queue_head->front->next=queue_head->next;
    XLqueue * tmp=queue_head->next;
    free(queue_head);
    head->queue=tmp;
    return 1;
}

int queue_del_queue(XLqueue_head *head,XLqueue * queue)
{
    if(head==NULL)return -1;
    XLqueue * queue_now=head->queue;
    if(queue_now==NULL)return -1;

    if(queue_now==queue&&queue_now->next==queue_now){
        free(queue_now);
        head->queue=NULL;
        return 1;
    }
    while(1)
    {
        if(queue_now==queue){
            queue_now->next->front=queue_now->front;
            queue_now->front->next=queue_now->next;
            free(queue_now);
            return 1;
        }
        if(queue_now->next==head->queue)return -1;
        queue_now=queue_now->next;
    }
    return -1;
}

int queue_remove_all(XLqueue_head * head){
    if(head==NULL)return -1;
    while(1)
    {
        if(queue_del_head(head)<=0)return 1;
    }
}

//添加成员到队列，*head为队列的队头的指针，in为成员，mode设置成员类型，level设置优先级
int queue_add(int mode,XLqueue_head * head,XLqueue_in * in,LEVEL level)
{
    if(in==NULL||head==NULL)return -1;
    if(mode!=QUEUE_TYPE_CONNECT&&mode!=QUEUE_TYPE_INS&&mode!=QUEUE_TYPE_SIGN)return -1;

    XLqueue * queue_new=malloc(sizeof (XLqueue));
    memset(queue_new,0,sizeof(XLqueue));
    queue_new->mode=mode;
    queue_new->level=level;
    queue_new->in=*in;
    if(mode==QUEUE_TYPE_CONNECT){
        queue_new->in.pak_connect.core_name=malloc(strlen(in->pak_connect.core_name)+1);
        strcpy(queue_new->in.pak_connect.core_name,in->pak_connect.core_name);
    }
    if(mode==QUEUE_TYPE_INS){
    }

    if(head->queue==NULL){
        head->queue=queue_new;
        head->queue->front=head->queue;
        head->queue->next=head->queue;
        return 1;
    }
    XLqueue * queue_head=head->queue;

    XLqueue * queue_now=queue_head;
    while (1) {
        if(level>queue_now->level){
            queue_new->next=queue_now;
            queue_new->front=queue_now->front;

            queue_now->front->next=queue_new;
            queue_now->front=queue_new;

            if(queue_now==queue_head)head->queue=queue_new;
            return 1;
        }
        if(queue_now->next==queue_head)
        {
            queue_new->next=queue_now->next;
            queue_new->front=queue_now;

            queue_now->next->front=queue_new;
            queue_now->next=queue_new;
            return 1;
        }
        queue_now=queue_now->next;
    }

    return 0;
}

//添加INS成员到队列，*head为队列的队头的指针，*ins为成员，level设置优先级
int queue_add_ins(XLqueue_head * head,XLpak_ins * ins,LEVEL level){
    XLqueue_in in;
    in.pak_ins=*ins;

   // in.pak_ins.sender.name=malloc(strlen(ins->sender.name)+1);
   // strcpy(in.pak_ins.sender.name,ins->sender.name);

   // in.pak_ins.receiver.name=malloc(strlen(ins->receiver.name)+1);
   // strcpy(in.pak_ins.receiver.name,ins->receiver.name);

   // in.pak_ins.ins=malloc(strlen(ins->ins)+1);
  // strcpy(in.pak_ins.ins,ins->ins);
    //in.pak_ins.sender.name=ins->sender.name;
    //in.pak_ins.receiver.name=ins->receiver.name;
    //in.pak_ins.ins=ins->ins;
    return queue_add(QUEUE_TYPE_INS,head,&in,level);
}

//添加CONNECT成员到队列，*head为队列的队头的指针，*ins为成员，level设置优先级
int queue_add_connect(XLqueue_head * head,XLpak_connect * connect,LEVEL level){
    XLqueue_in in;
    in.pak_connect=*connect;
    return queue_add(QUEUE_TYPE_CONNECT,head,&in,level);
}

int queue_add_sign(XLqueue_head * head,XLpak_sign * sign,LEVEL level){
    XLqueue_in in;
    in.pak_sign=*sign;
    return queue_add(QUEUE_TYPE_SIGN,head,&in,level);
}
//展示队列中的成员（仅作调试用）
void queue_show(XLqueue_head * head){
    if(head==NULL)return;
    XLqueue * queue_head=head->queue;
    if(queue_head==NULL)
    {
        printf("no!\n");
        return;
    }
    printf("list:");
    XLqueue * ins_now=queue_head;
    while (1) {
        //printf("%d ",ins_now->level);
        printf("%s ",ins_now->in.pak_ins.ins);
        if(ins_now->next==queue_head)break;

        ins_now=ins_now->next;
    }
    printf("\n");
}

//总队列的快捷调用
XLqueue_head * queue_total(void)
{
    extern XLqueue_head total_queue_head;
    return &total_queue_head;
}

//发送队列的快捷调用
XLqueue_head * queue_send(void)
{
    extern XLqueue_head send_queue_head;
    return &send_queue_head;
}

//广播发送队列的快捷调用
XLqueue_head * queue_broadcast(void)
{
    extern XLqueue_head broadcast_queue_head;
    return &broadcast_queue_head;
}

//接收队列的快捷调用
XLqueue_head * queue_recv(void)
{
    extern XLqueue_head recv_queue_head;
    return &recv_queue_head;
}

//队列初始化
int queue_init(void){
    queue_total()->queue=NULL;
    queue_recv()->queue=NULL;
    queue_send()->queue=NULL;
    queue_broadcast()->queue=NULL;
    return 1;
}

//----------------------------------------------//
//            数据包结构体和buf的转换              //
//----------------------------------------------//

//将buf转换为网络数据结构体（XLpak_ins）
XLpak_ins * buf_to_pak_ins(DATA * buf){
    if(buf==NULL)return NULL;
    XLpak_ins * pak_ins=malloc(sizeof(XLpak_ins));
    int p=0;
    //base
    data_get(buf,&p,pak_ins,sizeof(XLpak_base));

    //sender source
    data_get(buf,&p,&pak_ins->sender,sizeof(XLsource)-sizeof(char*));
    if(pak_ins->base.mode==SIGN_NAME&&(pak_ins->sender.name=data_get_str(buf,&p))==NULL){
        free(pak_ins);
        return NULL;
    }

    //receiver source
    data_get(buf,&p,&pak_ins->receiver,sizeof(XLsource)-sizeof(char*));
    if((pak_ins->receiver.mode==START_APP||pak_ins->receiver.mode==SIGN_NAME)&&
            (pak_ins->receiver.name=data_get_str(buf,&p))==NULL){
        free(pak_ins);
        return NULL;
    }

    //INS
    pak_ins->ins=data_get_str(buf,&p);
    return pak_ins;
}

//将网络数据结构体（XLpak_ins）转换为buf
DATA * pak_ins_to_buf(XLpak_ins *pak_ins,int * size){
    if(pak_ins==NULL||size==NULL)return NULL;
    int pak_size;
    //measure packet size
    pak_size=sizeof(XLpak_ins)-sizeof(char*)*3;
    if(pak_ins->sender.mode==SIGN_NAME)
    {
        if(pak_ins->sender.name!=NULL)pak_size+=strlen(pak_ins->sender.name)+1;
        else return NULL;
    }
    if(pak_ins->receiver.mode==SIGN_NAME||pak_ins->receiver.mode==START_APP)
    {
        if(pak_ins->receiver.name!=NULL)pak_size+=strlen(pak_ins->receiver.name)+1;
        else return NULL;
    }
    if(pak_ins->ins!=NULL)
        pak_size+=strlen(pak_ins->ins)+1;
    DATA *data=malloc(pak_size);

    //Write into packet
    int p=0;
    data_add(data,&p,&pak_ins->base,sizeof(XLpak_base));
    data_add(data,&p,&pak_ins->sender,sizeof(XLsource)-sizeof(char*));
    if(pak_ins->sender.mode==SIGN_NAME)
        data_add_str(data,&p,pak_ins->sender.name);
    data_add(data,&p,&pak_ins->receiver,sizeof(XLsource)-sizeof(char*));
    if(pak_ins->receiver.mode==SIGN_NAME||pak_ins->receiver.mode==START_APP)
        data_add_str(data,&p,pak_ins->receiver.name);
    if(pak_ins->ins!=NULL)
        data_add_str(data,&p,pak_ins->ins);
    *size=p;
    return data;
}

//将buf转换为网络数据结构体（XLpak_connect）
XLpak_connect * buf_to_pak_connect(DATA* buf){
    if(buf==NULL)return NULL;
    int p=0;
    XLpak_connect * pak_connect=malloc(sizeof(XLpak_connect));

    data_get(buf,&p,pak_connect,sizeof(XLpak_base));
    if((pak_connect->core_name=data_get_str(buf,&p))==NULL){
        free(pak_connect);
        return NULL;
    }
    return pak_connect;
}

//将网络数据结构体（XLpak_connect）转换为buf
DATA * pak_connect_to_buf(XLpak_connect * pak_connect,int * size){
    if(pak_connect==NULL||size==NULL||pak_connect->core_name==NULL)return NULL;
    int p=0,pak_size;
    pak_size=sizeof(XLpak_base);
    pak_size+=strlen(pak_connect->core_name)+1;
    DATA * data=malloc(pak_size);
    data_add(data,&p,pak_connect,sizeof(XLpak_base));
    data_add_str(data,&p,pak_connect->core_name);
    *size=pak_size;
    return data;
}

DATA * pak_sign_to_buf(XLpak_sign * pak_sign,int * size){
    if(pak_sign==NULL||size==NULL)return 0;
    int p=0,pak_size;
    pak_size=sizeof(XLpak_base);
    pak_size+=sizeof(uint8_t);
    XLpak_signinfo * sign_now=pak_sign->sign_list;
    while(sign_now!=NULL){
        pak_size+=sizeof(uint8_t);
        pak_size+=strlen(sign_now->name)+1;
        pak_size+=strlen(sign_now->type)+1;
        sign_now=sign_now->next;
    }
    printf("size:%d\n",pak_size);
    DATA * data=malloc(pak_size);
    data_add(data,&p,pak_sign,sizeof(XLpak_base)+sizeof(uint8_t));
    sign_now=pak_sign->sign_list;

    while(sign_now!=NULL){
        data_add_str(data,&p,sign_now->name);
        data_add_str(data,&p,sign_now->type);
        sign_now=sign_now->next;
    }
    //data_show(data,pak_size);
    *size=pak_size;
    return data;
}

XLpak_sign * buf_to_pak_sign(DATA * buf){
    if(buf==NULL)return NULL;
    int p=0;
    XLpak_sign * pak_sign=malloc(sizeof(XLpak_sign));
    data_get(buf,&p,pak_sign,sizeof(XLpak_base)+sizeof(uint8_t));
    XLpak_signinfo * signinfo_now=NULL;
    for(int i=0;i<pak_sign->sign_num;i++)
    {
        XLpak_signinfo * sign_info=malloc(sizeof(XLpak_signinfo));
        sign_info->next=NULL;
        sign_info->name=data_get_str(buf,&p);
        sign_info->type=data_get_str(buf,&p);
        printf("\nname:%s\ntype:%s\n",sign_info->name,sign_info->type);
        if(signinfo_now==NULL){
            pak_sign->sign_list=sign_info;
            signinfo_now=sign_info;
        }
        else{
            signinfo_now->next=sign_info;
            signinfo_now=sign_info;
            //signinfo_now=signinfo_now->next;
        }

        signinfo_now->next=NULL;
    }
    return pak_sign;
}

//----------------------------------------------//
//                  网络接口                     //
//----------------------------------------------//

//网络检查，看看传入数据包标记的ip是否和传入的主机ip一致
int network_check(DATA * data,XLnet * net){
    if(data==NULL||net==NULL)return -2;
    XLpak_base base;
    data_get(data,NULL,&base,sizeof(XLpak_base));
    if(base.net_sender.ip!=net->ip)return -1;
    return 1;
}

int network_safe(XLnet *net){
    if(net==NULL)return -1;
    XLcore * core=core_get_by_net(net);
    if(core==NULL||core->safety!=CORE_STATE_VERIFIED)return -1;
    return 1;
}

//广播请求其他核心发送核信息（A1） 广播
int network_core_find_send(void){
    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.mode=PAK_SEND_CORE_REQUIRE+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    if(mycore==NULL)return -1;
    pak.core_name=(char*)&mycore->name;
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    queue_add_connect(queue_broadcast(),&pak,0);
    return 1;
}

//传入对方广播申请的数据包结构体，回复自己的核信息（B1）TCP
int network_core_find_receive(XLpak_connect * pak_connect){
   if(pak_connect==NULL)return -1;
   if(pak_connect->core_name==NULL)return -1;
    int ret;
    ret=core_add(&pak_connect->base.net_sender,pak_connect->core_name);

    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.net_receiver=pak_connect->base.net_sender;

    pak.base.mode=PAK_SEND_CORE+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    if(mycore==NULL)return -1;
    pak.core_name=(char*)&mycore->name;
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    queue_add_connect(queue_send(),&pak,0);
    return ret;
}

//发送核心对接请求（A1）TCP，传入要对接的核心的id
int network_core_connect_require_send(core_id_t core_id){
    if(core_id==1)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.net_receiver=core->net;
    pak.base.mode=PAK_CONNECT_REQUEST+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    pak.core_name=mycore->name;
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    core->safety=CORE_STATE_WAITTING;
    queue_add_connect(queue_send(),&pak,0);
    return 1;
}

//传入对方连接申请的数据包结构体，回复同意或不同意（B1）TCP
int network_core_connect_require_receive(XLpak_connect * pak_connect){
    if(pak_connect==NULL)return -1;
    XLcore *core=core_get_by_net(&pak_connect->base.net_sender);
    if(core==NULL)return -1;
    //缺少安全性验证
    core->safety=CORE_STATE_VERIFIED;

    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.mode=PAK_CONNECT_ACCEPT+(NETWORK_MODE_CONNECT<<12);
    pak.base.net_receiver=pak_connect->base.net_sender;

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    pak.core_name=mycore->name;
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    queue_add_connect(queue_send(),&pak,0);
    return 1;
}

//获得对方对接许可，设置对方的核心为安全核心(A2)
int network_core_connect(XLpak_connect * pak_connect){
    if(pak_connect==NULL)return -1;
    XLcore *core=core_get_by_net(&pak_connect->base.net_sender);
    if(core==NULL)return -1;
    //缺少网络验证
    if(core->safety!=CORE_STATE_WAITTING)return -1;
    core->safety=CORE_STATE_VERIFIED;
    return 1;
}

//断开与核心的连接，传入要断开的核心的ID
int network_core_disconnect_send(core_id_t core_id){
    if(core_id==1)return -1;
    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;
    XLpak_connect pak;
    pak.base.net_sender=network_get_local_info();
    pak.base.net_receiver=core->net;
    pak.base.mode=PAK_DISCONNECT+(NETWORK_MODE_CONNECT<<12);

    XLcore * mycore=core_get_by_id(CORE_MYSELF_ID);
    pak.core_name=mycore->name;
    pak.base.pak_size=sizeof(XLpak_connect)-sizeof(char*)+strlen(mycore->name)+1;
    core->safety=CORE_STATE_UNVERIFIED;
    queue_add_connect(queue_send(),&pak,0);
    return 1;
}

//向指定的核心发送命令
int network_ins(XLsource * sender,XLsource *receiver,INS * ins){
    if(ins==NULL)return -1;
    XLpak_ins pak_ins;
    pak_ins.base.mode=START_APP+(ACCESS<<4)+(NETWORK_MODE_INS<<12);

    //XLcore * core=core_get_by_id(receiver_core);
    XLcore * core=core_get_by_net(&receiver->net);
    if(core==NULL)return -1;
    //pak_ins.base.net_sender=network_get_local_info();
    pak_ins.base.net_sender=core_get_by_id(CORE_MYSELF_ID)->net;
    pak_ins.base.net_receiver=core->net;

    pak_ins.sender=*sender;
    pak_ins.sender.net=pak_ins.base.net_sender;
    pak_ins.receiver=*receiver;
    pak_ins.receiver.net=pak_ins.base.net_receiver;

    pak_ins.ins=ins;

    queue_add_ins(queue_send(),&pak_ins,0);
    return 1;
}

int network_send_sign(core_id_t core_id){

    XLcore * core=core_get_by_id(core_id);
    if(core==NULL)return -1;

    XLpak_sign pak_sign;
    pak_sign.base.net_receiver=core->net;

    pak_sign.base.mode=(NETWORK_MODE_SIGN<<12);
    pak_sign.sign_list=NULL;
    pak_sign.base.net_sender=core_get_by_id(CORE_MYSELF_ID)->net;

    int num=0;
    extern XLevent_list * event_list_head;
    XLevent_list * event_now=event_list_head;
    XLpak_signinfo * sign_now=NULL,*sign_front=NULL;
    pak_sign.sign_num=0;
    while(event_now!=NULL){
        if(event_now->event.sign!=NULL){
            if(pak_sign.sign_list==NULL){
                XLpak_signinfo * new=malloc(sizeof(XLpak_signinfo));
                pak_sign.sign_list=new;

                pak_sign.sign_list->name=event_now->event.sign->name;
                pak_sign.sign_list->type=event_now->event.sign->type;
                pak_sign.sign_list->next=NULL;
                sign_now=pak_sign.sign_list;
            }
            else{
                sign_now->next=malloc(sizeof(XLpak_signinfo));
                XLpak_signinfo * sign_new=sign_now->next;
                sign_new->next=NULL;
                sign_new->name=event_now->event.sign->name;
                sign_new->type=event_now->event.sign->type;
                sign_now=sign_new;
            }
            pak_sign.sign_num++;
        }
        event_now=event_now->next;
    }

    queue_add_sign(queue_send(),&pak_sign,0);
    //printf("sdsa:%d\n",pak_sign.sign_num);
    //int size=0;
    //DATA * data=pak_sign_to_buf(&pak_sign,&size);
    //printf("ggshshs\n");
    //buf_to_pak_sign(data);
    return 1;
}
