#include <network.h>
/*----------------------------------------------------------------------*/
/*下面的几个函数是用于对数据的处理*/
void * add_data(void * buf,int * p,void * data,int datasize)
{
    if(p==NULL||data==NULL||buf==NULL)return NULL;
    uint8_t *buf_p=buf,*data_p=data,i=0;
    buf_p+=*p;
    for(i=0;i<datasize;i++)
    {
        *buf_p=*data_p;
        data_p++;
        buf_p++;
    }
    *p+=i;
    return buf;
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

void show_buf(uint8_t *data)       //展示数据包(调试用)
{
    char *p = data;//uint8_t a=255;
    while (*p !=(char)PAK_DATA_END)
    {
        if(*p==(char)PAK_PAR_DATA_END)printf("*");
        else if(*p=='\0')printf(" ");
        else printf("%c",*p);
        p++;
    }
    printf("!\n");
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

/*发送网络数据包（正常）*/
int net_send(XLnet * net,uint8_t * data,uint32_t size)
{
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

/*网络接受程序初始化*/
pthread_t net_init(void)
{
    pthread_t tid;
    //创建网络接收线程
    if(pthread_create(&tid,NULL,net_pthread,&tid))
    {
        perror("Fail to create thread:");
        return 0;
    }
    return tid;
}

int net_pthread(void)   //接受网络数据包的线程
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
        serveraddr.sin_addr.s_addr = inet_addr("192.168.1.23");     //后面要传参
        serveraddr.sin_port = htons(8181);

        //将套接字与服务器网络信息结构体绑定
        if(bind(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0)
        {
            perror("fail to bind");
            exit(1);
        }

        while(1)
        {
            //进行通信
            uint8_t * data=malloc(sizeof(uint8_t)*PAK_DATA_SIZE);
            struct sockaddr_in clientaddr;
            if(recvfrom(sockfd, data, 128, 0, (struct sockaddr *)&clientaddr, &addrlen) < 0)
            {
                perror("fail to recvfrom");
                exit(1);
            }

            printf("[%s - %d]: %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), data);    //调试
            show_buf(data);
            netdev_id_t id;
            XLpak pak;
            if(net_get_pak(data,du_buf_size(data),&id,&pak)<=0)printf("pak error!\n");  //从数据包中提取出pak
            else{
                /*调试部分*/
                printf("pak id=%d pak sig:%s\n",id,pak.name);
                printf("par name:%s\n data:",pak.sig_par_h->name);
                show_par_data(pak.sig_par_h->data,pak.sig_par_h->datasize);
                printf("\npar name:%s\n data:",pak.sig_par_h->next->name);
                show_par_data(pak.sig_par_h->next->data,pak.sig_par_h->next->datasize);
                printf("\n\n");
                /*触发事件*/
                sig_send(id,pak.name,&pak);
            }
        }

        //关闭文件描述符
        close(sockfd);
     }
}

/*网络广播接收（将加入net_init）*/
struct XLCore_list * net_rev_search(void)
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
    broadcataddr.sin_addr.s_addr = inet_addr("255.255.255.255"); //192.168.3.255 255.255.255.255
    broadcataddr.sin_port = htons(8088);
    //第三步:将套接字与广播信息结构体绑定
    if(bind(sockfd, (struct sockaddr *)&broadcataddr, addrlen) < 0)
    {
        perror("fail to bind");
        exit(1);
    }
    //第四步:进行通信
    char * data=malloc(sizeof(char)*64);
    struct sockaddr_in sendaddr;
    while(1){
        if(recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&sendaddr, &addrlen) < 0)
        {
            perror("fail to recvfrom");
            exit(1);
        }
        //system("clear");
        printf("[%s ‐ %d]: %s\n", inet_ntoa(sendaddr.sin_addr), ntohs(sendaddr.sin_port), data);
        /*以下部分将封装为函数*/
        show_buf(data);
        char * data_p=data,*ip_p;
        uint32_t ipv4;
        uint16_t port;
        ip_p=&ipv4;
        if(*data_p==PAK_MODE_CONNECT){
            data_p++;
            for(int i=0;i<4;i++)
            {
                *ip_p=*data_p;
                data_p++;
                ip_p++;
            }
            ip_p=&port;
            for(int i=0;i<2;i++)
            {
                *ip_p=*data_p;
                data_p++;
                ip_p++;
            }
            printf("ip:%x port:%d\n",ipv4,port);
            /*获得发送者的信息后与其对接 */
            //……
        }
    }
    //|mode|ip|port|end|
    return NULL;
}
/*网络数据广播*/
struct XLCore_list * net_search(XLnet * net)
{

     int sockfd; //文件描述符
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
     broadcataddr.sin_addr.s_addr =inet_addr("255.255.255.255");//(in_addr_t)net->ip.net_ipv4; //192.168.3.255 255.255.255.255
     broadcataddr.sin_port = htons(8088);
     //第四步:进行通信

     //char buf[]="hello!";
     char * buf=malloc(sizeof(char)*2+sizeof(uint32_t)+sizeof(uint16_t)),*buf_p;
     //|mode|ip|port|END|
     buf_p=buf;
     *buf_p=PAK_MODE_CONNECT;

     buf_p++;
     uint8_t * ip_p=&net->ip.net_ipv4;
     for(uint8_t i=0;i<4;i++)
     {
        *buf_p=*ip_p;
         buf_p++;
         ip_p++;
     }
     *buf_p=PAK_DATA_END;
     //*buf_p='\0';
     //for(uint8_t i=0;i<times;i++)
     while (1)
     {
         if(sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&broadcataddr, addrlen) < 0)
         {
             perror("fail to sendto");
             return 0;
         }
         sleep(1);
         printf("1\n");
     }

     return 0;
}
/*-----------------------------------------------------------------------------------------------*/
/*数据包打包和解包*/
/*将网络包解包*/
int net_send_pak(netdev_id_t netdev_id,XLpak * pak)      //将参数打包
{
    //度量大小
    uint32_t datasize=0;
    str * dev_name=netdev_get(netdev_id)->name;
    str * sig_name=pak->name;
    datasize+=strlen(dev_name)+1;
    datasize+=strlen(sig_name)+1;
    if(pak->sig_par_h!=NULL)
    {
        XLsig_par * par_now=pak->sig_par_h;
        while(par_now!=NULL)
        {
            datasize+=strlen(par_now->name)+2;  //
            if(par_now->data!=NULL) datasize+=par_now->datasize;
            par_now=par_now->next;
        }
    }
    datasize++;
    //buf的制作
    uint8_t *buf=malloc(sizeof(uint8_t)*datasize),a;
    int p=0;
    add_data(buf,&p,dev_name,strlen(dev_name)+1);
    add_data(buf,&p,sig_name,strlen(sig_name)+1);
    XLsig_par * par_now=pak->sig_par_h;
    a=PAK_PAR_DATA_END;
    while (par_now!=NULL) {
       add_data(buf,&p,par_now->name,strlen(par_now->name)+1);
        if(par_now->datasize>0)
        {
           add_data(buf,&p,par_now->data,par_now->datasize);
           add_data(buf,&p,&a,1);
        }
        par_now=par_now->next;
    }
    a=PAK_DATA_END;
    buf=add_data(buf,&p,&a,1);
    //调试
    show_buf(buf);
    //dev_id_t dev_id;
    //XLpak pak_rev;
    //net_get_pak(buf,datasize,&dev_id,&pak_rev);
    //发送buf
    if(netdev_get(netdev_id)==NULL)return 0;
    net_send(&netdev_get(netdev_id)->net,buf,datasize);
    return 1;
}

/*将网络包解包*/
int net_get_pak(uint8_t * data,uint32_t datasize,dev_id_t * dev_id,XLpak * pak)   //依托答辩
{
    /*uint8_t size=0;
    uint8_t *data_p=data,a=0;
    str * dev_name=malloc(sizeof(str)*DEVICE_NAME_LENGTH),*dev_name_p=dev_name;
    str * sig_name=malloc(sizeof(str)*sig_NAME_LENGTH),*sig_name_p=sig_name;
    str * par_name=malloc(sizeof(str)*PAR_NAME_LENGTH),*par_name_p=par_name;

    XLpak * pak=malloc(sizeof(struct XLpak));
    XLsig_par * par_now=pak->sig_par_h;
    while (*data_p!=PAK_DATA_END) {              //循环解包
        if(a==0)        //读设备名
        {
            if(*data_p=='\0') //字符串末尾
            {
                *dev_name_p='\0';
                a++;
            }
            else //赋值+指针移位
            {
            *dev_name_p=*data_p;
            dev_name_p++;

            }
        }
        else if(a==1)   //读信号名
        {
            if(*data_p=='\0') //字符串末尾
            {
                *sig_name_p='\0';
                strcpy(pak->name,sig_name);
                free(sig_name);
                a++;
            }
            else //赋值+指针移位
            {
            *sig_name_p=*data_p;
            sig_name_p++;
            }
        }
        else if(a>=2)   //读参数名
        {
            if(pak->sig_par_h==NULL)     //创建信号头
            {
                pak->sig_par_h=malloc(sizeof(struct XLsig_par));
                par_now=pak->sig_par_h;
            }
            if(a==3)    //创建新信号
            {
                par_now->next=malloc(sizeof(struct XLsig_par));
                par_now=par_now->next;
                a=2;
            }
            if(*data_p=='\0') //字符串末尾
            {
                *par_name_p='\0';
                strcpy(par_now->name,par_name);
                free(par_name);
                data_p++;
                uint8_t * data_pp=data_p;
                uint32_t par_datasize=0;    //记录参数的大小
                while(*data_pp!=PAK_PAR_DATA_END){   //参数的大小
                    par_datasize++;
                    data_pp++;
                }
                par_now->data=malloc(sizeof(uint8_t)*par_datasize);
                uint8_t *par_data_p=par_now->data;
                while(*data_p!=PAK_PAR_DATA_END){   //写入参数
                    *par_data_p=*data_p;
                    data_p++;
                    par_data_p++;
                }
                //参数赋值
                //par_now->data=malloc(sizeof(uint8_t)*par_datasize);
                //if(par_datasize!=0)*par_now->data=*par_data;
                par_now->datasize=par_datasize;
                //free(par_data);


                //为了下一个参数重新初始化参数名
                par_name=malloc(sizeof(str)*PAR_NAME_LENGTH);
                par_name_p=par_name;

                a=3;    //a=3时表示如果有下一个参数，在下一个循环中会建立新参数
            }
            else    //赋值+指针移位
            {
            *par_name_p=*data_p;
            par_name_p++;
            }
        }
        if(size>datasize){printf("666");return 0;} //超过长度
        size++;
        data_p++;
    }
    *netdev_id=dev_get_byname(dev_name);
    if(*netdev_id==0){printf("no dev!");return 0;}
    free(dev_name);
    if(pak->name==""){printf("sig fail!");return 0;}
    *pake=*pak; //参数传递
    free(pak);
    return 1;*/
    int p=0,a=0;
    XLpak pake;
    memset(&pake,0,sizeof(pake));
    //XLsig_par * par_now=pake.sig_par_h;

    str * device_name,*signal_name;
    if((device_name=(str *)get_str(data,&p,datasize))==NULL)return 0;
    printf("str:%s\n",device_name);
    if((signal_name=(str *)get_str(data,&p,datasize))==NULL)return 0;
    printf("str:%s\n",signal_name);
    strcpy(pake.name,signal_name);
    str * s;
    while (p<(int)datasize-1)
    {
        if(a==0)
        {
            if((s=get_str(data,&p,datasize))==NULL)return 0;
            printf("par_str:%s\n",s);
            pak_add_par(&pake,s);
            a=1;
        }
        else if(a==1){
            int size=0;
            uint8_t *da;
            if((da=get_par_data(data,&p,&size,datasize))==NULL)return 0;
            printf("par_data:%x%x%x%x",*da,*da+1,*da+2,*da+3);  //
            pak_set_data(&pake,s,da,size);
            a=0;
        }
    }
    dev_id_t  nd_id;
    nd_id=dev_get_byname(device_name);
    if(nd_id==0)return 0;
    *dev_id=nd_id;
    printf("\ndev:%d\n",nd_id); //
    *pak=pake;
    printf("end\n");    //
    return 1;
}
