#include <network.h>
/*----------------------------------------------------------------------*/
/*下面的几个函数是用于对数据的处理*/
int add_data(void * buf,int * p,void * data,int datasize)
{
    if(p==NULL||data==NULL||buf==NULL)return 0;
    uint8_t *buf_p=buf,*data_p=data,i=0;
    buf_p+=*p;
    for(i=0;i<datasize;i++)
    {
        *buf_p=*data_p;
        data_p++;
        buf_p++;
    }
    *p+=i;
    return 1;
}

void * get_data(void *buf,int *p,int datasize)
{
    if(buf==NULL||p==NULL||datasize<=0)return NULL;
    uint8_t * data=malloc(sizeof(uint8_t)*datasize),*data_p=data;
    uint8_t * buf_p=buf+*p;
    int i;
    for(i=0;i<datasize;i++)
    {
        *data_p=*buf_p;
        data_p++;
        buf_p++;
    }
    *p+=i;
    return data;
}

str * get_str(void *buf,int *p,int maxsize)
{
     if(buf==NULL||p==NULL||*p>=maxsize)return NULL;
     uint8_t * buf_p=buf;
     buf_p+=*p;
     int i=1;
     while (*buf_p!='\0')
     {
         buf_p++;
         i++;
         if(i>maxsize)return NULL;
     }
     str * data=malloc(sizeof(uint8_t)*i),*data_p=data;
     buf_p=buf+*p;
     for(int j=0;j<i;j++)
     {
         *data_p=*buf_p;
         data_p++;
         buf_p++;
     }
     *p+=i;
     return data;
}

void * get_par_data(void *buf,int *p,int * datasize,int maxsize)
{
     if(buf==NULL||p==NULL||*p>=maxsize)return NULL;

     uint8_t * buf_p=buf+*p;
     int i=1;
     while (*buf_p!=PAK_PAR_DATA_END)
     {
         buf_p++;
         i++;
         if(i>maxsize)return NULL;
     }
     uint8_t * data=malloc(sizeof(uint8_t)*i),*data_p=data;
     buf_p=buf+*p;
     for(int j=0;j<i;j++)
     {
         *data_p=*buf_p;
         data_p++;
         buf_p++;
     }
     *p+=i;
     *datasize=i-1;
     return data;
}

void show_buf(uint8_t *data,int size)       //展示数据包(调试用)
{
    int i=0;
    char *p = data;
    while (i<=size)
    {
        if(*p==(char)PAK_PAR_DATA_END)printf("*");
        else if(*p=='\0')printf(" ");
        else printf("%c",*p);
        p++;
        i++;
    }
}

void show_ipv4(void * buf)
{
    if(buf==NULL){printf("error ip\n");return 0;}
    uint8_t * buf_p=buf;
    printf("ip地址:");
    for(int i=0;i<4;i++)
    {
        printf("%d.",*buf_p);
        buf_p++;
    }
}

void show_par_data(uint8_t *data,uint32_t datasize)          //展示par_data(调试用)
{
    uint8_t *p = data,i=0;
    while (i!=datasize)
    {
        printf("%c",*p);
        p++;
        i++;
    }
}

int du_buf_size(uint8_t *data)
{
    int i=1;
    uint8_t *p = data;
    while (*p!=PAK_DATA_END)
    {
        p++;
        i++;
        if(i>PAK_MAX_SIZE)return -1;
    }
    return i;
}
/*----------------------------------------------------------------------------------------------------*/
/*下面的几个函数是用于数据包的发送与接收*/

/*网络接受程序初始化*/
int net_init(void)
{
    pthread_t tid_sig,tid_broadcast;
    //创建网络接收线程
    if(pthread_create(&tid_sig,NULL,net_receive,&tid_sig))
    {
        perror("Fail to create thread:");
        return 0;
    }
    if(pthread_create(&tid_broadcast,NULL,net_broadcast_receive,&tid_broadcast))
    {
        perror("Fail to create thread:");
        return 0;
    }
    return 1;
}

/*发送网络数据包（正常）*/
int net_send(XLnet * net,uint8_t * data,uint32_t size)
{
    if(net==NULL)return 0;
    //XLnet * net=&netdev_get(id)->net;
    //第一步：创建套接字
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("fail to socket");
        exit(1);
    }

    printf("sockfd = %d\n", sockfd);

    //第二步：填充服务器网络信息结构体 sockaddr_in
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);

    serveraddr.sin_family = AF_INET; //协议族，AF_INET：ipv4网络协议


    serveraddr.sin_addr.s_addr = net->ip.net_ipv4;//ip地址
    serveraddr.sin_port = htons(net->port);

    if(sendto(sockfd, data, size, 0, (struct sockaddr *)&serveraddr, addrlen) == -1)
    {
        perror("fail to sendto");
        exit(1);
    }
    //第四步：关闭套接字文件描述符
    close(sockfd);
    return 0;
}

int net_receive(void)   //接受网络数据包的线程
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
            exit(1);
        }
        //填充服务器网络信息结构体
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = core_get(1)->net.ip.net_ipv4;     //后面要传参
        serveraddr.sin_port = htons(core_get(1)->net.port);
        //将套接字与服务器网络信息结构体绑定
        if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0)
        {
            perror("fail to bind");
            exit(1);
        }
        while(1)
        {
            //进行通信
            uint8_t * data=malloc(sizeof(uint8_t)*PAK_MAX_SIZE);
            struct sockaddr_in clientaddr;
            if(recvfrom(sockfd, data, PAK_MAX_SIZE, 0, (struct sockaddr *)&clientaddr, &addrlen) < 0)
            {
                perror("fail to recvfrom");
                exit(1);
            }
            //printf("[%s - %d]: %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), data);    //调试
            if(*data==PAK_MODE_SIGNAL&&core_get_by_ip(clientaddr.sin_addr.s_addr)!=NULL)
            {
                system("clear");
                netdev_id_t id;
                XLsig_pak pak;

                if(net_get_sig(data,&id,&pak)<=0)printf("pak error!\n");  //从数据包中提取出pak
                else{
                    sig_send(id,pak.name,&pak);
                }
            }
            else if (*data==PAK_MODE_CONNECT) {

                XLnet net_rev;
                net_get_core(data,&net_rev);

                if(net_rev.ip.net_ipv4!=core_get(1)->net.ip.net_ipv4)
                {
                    printf("对方连接请求:");
                    show_ipv4(&net_rev.ip.net_ipv4);
                    printf("端口:%d\n",net_rev.port);
                    net_connect(&net_rev,PAK_MODE_ACCEPT_CONNECT);
                }
                core_create(&net_rev);
            }
            else if (*data==PAK_MODE_ACCEPT_CONNECT) {
                printf("同意对方的连接请求!\n");
                XLnet net_rev;
                if(net_rev.ip.net_ipv4!=core_get(1)->net.ip.net_ipv4)
                    net_get_core(data,&net_rev);
                core_create(&net_rev);
            }
            else if (*data==PAK_MODE_GET_DEV) {
                printf("对方发送设备信息!\n");
                show_ipv4(&clientaddr.sin_addr.s_addr);
                net_get_dev_info(clientaddr.sin_addr.s_addr,data);
            }
        }

        //关闭文件描述符
        close(sockfd);
     }
}

/*网络广播接收（将加入net_init）*/
int net_broadcast_receive(void)
{
    int sockfd; //文件描述符
    struct sockaddr_in broadcataddr;
    socklen_t addrlen = sizeof(broadcataddr);
    //第一步:创建套接字
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
    perror("fail to socket");
    exit(1);
    }
    //第二步:填充广播信息结构体
    broadcataddr.sin_family = AF_INET;
    broadcataddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    broadcataddr.sin_port = htons(8088);
    //第三步:将套接字与广播信息结构体绑定
    if(bind(sockfd, (struct sockaddr *)&broadcataddr, addrlen) < 0)
    {
        perror("fail to bind");
        exit(1);
    }
    //第四步:进行通信
    char * data=malloc(sizeof(char)*PAK_MAX_SIZE);
    struct sockaddr_in sendaddr;
    while(1){
        if(recvfrom(sockfd, data,PAK_DATA_SIZE, 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            perror("fail to recvfrom");
            exit(1);
        }

        XLnet net_rev;
        net_get_core(data,&net_rev);
        uint32_t ip_lo=core_get(1)->net.ip.net_ipv4;

        printf("收到广播信号:");
        show_ipv4(&net_rev.ip.net_ipv4);
        printf("端口:%d\n",net_rev.port);

        if(net_rev.ip.net_ipv4!=ip_lo&&*data==PAK_MODE_SEARCH){

            net_connect(&net_rev,PAK_MODE_CONNECT); //in common
        }
        else printf("广播发送者是自己\n");
    }
    //|mode|ip|port|end|
    return NULL;
}
/*网络数据广播*/
int net_broadcast_send (uint16_t port,void *buf,uint32_t bufsize)
{

     int sockfd;
     struct sockaddr_in broadcataddr; //服务器网络信息结构体
     socklen_t addrlen = sizeof(broadcataddr);
     //第一步:创建套接字
     if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
     {
         perror("fail to socket");
         return 0;
     }
     //第二步:设置为允许发送广播权限
     int on = 1;
     if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
     {
         perror("fail to setsockopt");
         return 0;
     }
     //第三步:填充广播信息结构体
     broadcataddr.sin_family = AF_INET;
     broadcataddr.sin_addr.s_addr =inet_addr("255.255.255.255"); //192.168.3.255 255.255.255.255
     broadcataddr.sin_port = htons(port);
     //第四步:进行通信
     if(sendto(sockfd, buf,bufsize, 0, (struct sockaddr *)&broadcataddr, addrlen) < 0)
     {
         perror("fail to sendto");
         return 0;
     }
     printf("广播成功!\n");
     //第四步：关闭套接字文件描述符
     close(sockfd);
     return 0;
}
/*-----------------------------------------------------------------------------------------------*/
/*数据包打包和解包*/
int net_send_sig(netdev_id_t netdev_id,XLsig_pak * pak)      //将参数打包
{
    //度量大小
    uint16_t buf_size=1+2;    //|mode(u8)|size(u16)|
    str * dev_name=netdev_get(netdev_id)->name;
    str * sig_name=pak->name;
    buf_size+=strlen(dev_name)+1;
    buf_size+=strlen(sig_name)+1;
    if(pak->sig_par_h!=NULL)
    {
        XLsig_par * par_now=pak->sig_par_h;
        while(par_now!=NULL)
        {
            buf_size+=strlen(par_now->name)+2;  //
            if(par_now->data!=NULL) buf_size+=par_now->datasize;
            par_now=par_now->next;
        }
    }
    //buf的制作
    uint8_t *buf=malloc(sizeof(uint8_t)*buf_size);
    int p=0;
    uint8_t mode=PAK_MODE_SIGNAL;
    add_data(buf,&p,&mode,sizeof(uint8_t));
    add_data(buf,&p,&buf_size,sizeof(uint16_t));    //size
    add_data(buf,&p,dev_name,strlen(dev_name)+1);
    add_data(buf,&p,sig_name,strlen(sig_name)+1);
    XLsig_par * par_now=pak->sig_par_h;
    uint8_t t=PAK_PAR_DATA_END;
    while (par_now!=NULL) {
        add_data(buf,&p,par_now->name,strlen(par_now->name)+1);
        if(par_now->datasize>0)
        {
           add_data(buf,&p,par_now->data,par_now->datasize);
           add_data(buf,&p,&t,1);
        }
        par_now=par_now->next;
    }
    show_buf(buf,buf_size);//调试
    if(netdev_get(netdev_id)==NULL)return 0;
    net_send(&netdev_get(netdev_id)->net,buf,buf_size);
    return 1;
}

/*将网络包解包*/

int net_get_sig(uint8_t * buf,dev_id_t * dev_id,XLsig_pak * pak)
{
    int p=1,a=0;    //p=1:第一个字节是模式，跳过
    uint16_t buf_size;
    if((buf_size=*(uint16_t *)get_data(buf,&p,sizeof(uint16_t)))==NULL)return 0;
    XLsig_pak pake;
    memset(&pake,0,sizeof(pake));
    printf("paksize:%d\n\n",buf_size);
    str * device_name,*signal_name;
    if((device_name=(str *)get_str(buf,&p,(uint32_t)buf_size))==NULL)return 0;
    if((signal_name=(str *)get_str(buf,&p,(uint32_t)buf_size))==NULL)return 0;
    strcpy(pake.name,signal_name);
    str * s;
    while (p<(int)buf_size-1)
    {
        if(a==0)
        {
            if((s=get_str(buf,&p,buf_size))==NULL)return 0;
            pak_add_par(&pake,s);
            a=1;
        }
        else if(a==1){
            int size=0;
            uint8_t *da;
            if((da=get_par_data(buf,&p,&size,buf_size))==NULL)return 0;
            pak_set_data(&pake,s,da,size);
            a=0;
        }
    }
    dev_id_t  nd_id;
    nd_id=dev_get_byname(device_name);
    if(nd_id==0)return 0;
    *dev_id=nd_id;
    *pak=pake;
    free(device_name);
    free(signal_name);
    return 1;
}

int net_send_core(void)
{
    XLnet net;
    net.port=core_get(1)->net.port;
    net.ip.net_ipv4=core_get(1)->net.ip.net_ipv4;
    uint8_t *buf=malloc(sizeof(uint8_t)*(1+4+2+1)),bufsize=1+4+2+1; //|mode|ip|port|end|
    int p=0;
    uint8_t t=PAK_MODE_SEARCH;
    if(add_data(buf,&p,&t,1)<=0)return 0;
    if(add_data(buf,&p,&net.ip.net_ipv4,sizeof(uint32_t))<=0)return 0;
    if(add_data(buf,&p,&net.port,sizeof(uint16_t))<=0)return 0;
    t=PAK_DATA_END;
    if(add_data(buf,&p,&t,1)<=0)return 0;
    printf("主机发送广播\n");
    net_broadcast_send(8088,buf,bufsize);
    return 1;
}

int net_get_core(void * buf,XLnet * net)
{
    int p=1;
    XLnet net_t;
    memset(&net_t,0,sizeof(XLnet));
    uint32_t * ip;
    if((ip=(uint32_t *)get_data(buf,&p,sizeof(uint32_t)))==NULL)return 0;
    net_t.ip.net_ipv4=*ip;
    uint16_t * port;
    if((port=(uint16_t *)get_data(buf,&p,sizeof(uint16_t)))==NULL)return 0;
    net_t.port=*port;
    *net=net_t;
    return 1;
}

int net_connect(XLnet *net,int mode)
{
    if(net==NULL)return 0;
    if(mode==PAK_MODE_CONNECT)
    {
        XLnet net_lo;
        net_lo.port=core_get(1)->net.port;
        net_lo.ip.net_ipv4=core_get(1)->net.ip.net_ipv4;
        uint8_t *buf=malloc(sizeof(uint8_t)*(1+4+2+1)),bufsize=1+4+2+1; //|mode|ip|port|end|
        int p=0;
        uint8_t t=PAK_MODE_CONNECT;
        if(add_data(buf,&p,&t,1)<=0)return 0;
        if(add_data(buf,&p,&net_lo.ip.net_ipv4,sizeof(uint32_t))<=0)return 0;
        if(add_data(buf,&p,&net_lo.port,sizeof(uint16_t))<=0)return 0;
        t=PAK_DATA_END;
        if(add_data(buf,&p,&t,1)<=0)return 0;
        net_send(net,buf,bufsize);
    }
    else if(mode==PAK_MODE_ACCEPT_CONNECT)
    {
        XLnet net_lo;
        net_lo.port=core_get(1)->net.port;
        net_lo.ip.net_ipv4=core_get(1)->net.ip.net_ipv4;
        uint8_t *buf=malloc(sizeof(uint8_t)*(1+4+2+1));uint32_t bufsize=1+4+2+1; //|mode|ip|port|end|
        int p=0;
        uint8_t t=PAK_MODE_ACCEPT_CONNECT;
        if(add_data(buf,&p,&t,1)<=0)return 0;
        if(add_data(buf,&p,&net_lo.ip.net_ipv4,sizeof(uint32_t))<=0)return 0;
        if(add_data(buf,&p,&net_lo.port,sizeof(uint16_t))<=0)return 0;
        t=PAK_DATA_END;
        if(add_data(buf,&p,&t,1)<=0)return 0;
        net_send(net,buf,bufsize);
    }
    return 1;
}

int net_send_dev_info(core_id_t core_id)
{
    int p=0;
    uint16_t buf_size=1+2; //|mode(u8)|size(u16)|
    extern XLdev * dev_head;
    XLdev * dev_now=dev_head;
    for(;;)
    {
        buf_size+=strlen(dev_now->name)+1;
        if(dev_now->next==NULL)break;
        dev_now=dev_now->next;
    }
    printf("size of buf=%d\n",buf_size);
    dev_now=dev_head;
    uint8_t t=PAK_MODE_GET_DEV,* buf=malloc(sizeof(uint8_t)*buf_size);
    if(add_data(buf,&p,&t,1)<=0)return 0;
    if(add_data(buf,&p,&buf_size,2)<=0)return 0;
    for(;;)
    {
        if(add_data(buf,&p,dev_now->name,strlen(dev_now->name)+1)<=0)return 0;
        if(dev_now->next==NULL)break;
        dev_now=dev_now->next;
    }
    XLCore * core=core_get(core_id);
    if(core==NULL)return 0;
    if(net_send(&core->net,buf,(uint32_t)buf_size)<=0)return 0;
    return 1;
}

core_id_t net_get_dev_info(uint32_t ipv4,uint8_t * buf)
{
      XLCore * core=core_get_by_ip(ipv4);
      if(core==NULL)return 0;
      int p=1;
      uint16_t buf_size;
      if((buf_size=*(uint16_t*)get_data(buf,&p,2))==NULL)return 0;
      while (p<=buf_size) {
          str * s;
          if((s=get_str(buf,&p,(int)buf_size))==NULL)return 0;
          netdev_id_t netdev;
          if(netdev=netdev_create(s)>0);
          {
            printf("网络:");
            show_ipv4(&ipv4);
            printf("找到网络设备:%s\n",s);
            struct ip ip;
            ip.net_ipv4=ipv4;
            netdev_set_net(netdev,ip,core->net.port,NETWORK_IPV4);
          }
      }
      return 1;
}

