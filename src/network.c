#include <network.h>

int8_t net_pthread(void)   //使用UDP,接收数据包并且提取出数据
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
        serveraddr.sin_addr.s_addr = inet_addr("192.168.1.23");
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
            uint8_t * data=malloc(sizeof(uint8_t)*PAK_SIZE);
            struct sockaddr_in clientaddr;
            if(recvfrom(sockfd, data, 128, 0, (struct sockaddr *)&clientaddr, &addrlen) < 0)
            {
                perror("fail to recvfrom");
                exit(1);
            }

            printf("[%s - %d]: %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), data);
            show_data(data);
            //printf("du_pak_size:%d\n",du_pak_size(data));
            netdev_id_t id;
            XLsig_pak pak;
            if(net_get_pak(data,du_pak_size(data),&id,&pak)<=0)printf("pak error!\n");
            else{
                printf("pak id=%d pak sig:%s\n",id,pak.name);
                printf("par name:%s\n data:",pak.sig_par_h->name);
                show_d(pak.sig_par_h->data,pak.sig_par_h->datasize);
                printf("\npar name:%s\n data:",pak.sig_par_h->next->name);
                show_d(pak.sig_par_h->next->data,pak.sig_par_h->next->datasize);
                printf("\n\n");
                sig_send(id,pak.name,&pak);
            }
        }

        //关闭文件描述符
        close(sockfd);
     }
}

pthread_t net_init(void) //网络初始化（建立数据包的接收线程）
{
    pthread_t tid;

    if(pthread_create(&tid,NULL,net_pthread,&tid))
    {
        perror("FAil to create thread:");
        return 0;
    }
    return tid;
}

int8_t net_send(netdev_id_t id,uint8_t * data,uint32_t size)    //发送数据包
{
    XLnet * net=&netdev_get(id)->net;
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

    while(1)
    {
        int i;
        scanf("%d",&i);
        if(sendto(sockfd, data, size, 0, (struct sockaddr *)&serveraddr, addrlen) == -1)
        {
            perror("fail to sendto");
            exit(1);
        }
    }

    //第四步：关闭套接字文件描述符
    close(sockfd);
    return 0;
}
/*int8_t net_receive(uint8_t * data,uint32_t size)     //接受数据包
{

}*/

/*PAK*/
int get_length(char *str)       //获得字符串长度
{
    char *p = str;
    int count = 0;
    while (*p++ != '\0')
    {
        count++;
    }
    return count+1;
}
void show_data(char *str)       //展示数据包
{
    char *p = str;//uint8_t a=255;
    while (*p !=(char)PAK_END)
    {
        if(*p==(char)DATA_END)printf("*");
        else if(*p=='\0')printf(" ");
        else printf("%c",*p);
        p++;
    }
    printf("!\n");
}
void show_d(uint8_t *data,uint32_t datasize)          //展示par_data
{
    uint8_t *p = data,i=0;
    while (i!=datasize)
    {
        printf("%d",*p);
        p++;
        i++;
    }
}

int16_t du_pak_size(uint8_t *data)
{
    uint8_t *p = data,i=1;
    while (*p!=PAK_END)
    {
        p++;
        i++;
    }
    return i;
}

int8_t net_send_pak(netdev_id_t netdev_id,XLsig_pak * pak)      //将参数打包（基本完成）
{
    //度量大小
    uint32_t datasize=0;
    str * dev_name=netdev_get(netdev_id)->name;
    str * sig_name=pak->name;
    datasize+=get_length(dev_name);
    datasize+=get_length(sig_name);
    if(pak->sig_par_h!=NULL)
    {
        XLsig_par * par_now=pak->sig_par_h;
        while(par_now!=NULL)
        {
            datasize+=get_length(par_now->name)+1;
            if(par_now->data!=NULL) datasize+=par_now->datasize;
            par_now=par_now->next;
        }
    }
    datasize++;
    //创建数据
    uint8_t * data_p=malloc(datasize);
    uint8_t * data;

    data=data_p;
    *data_p=(uint8_t)*dev_name;
    strcpy((char *)data_p,(char *)dev_name);
    data_p+=get_length(dev_name);           //写入设备名

    strcpy((char *)data_p,(char *)sig_name);
    data_p+=get_length(sig_name);           //写入信号名
    XLsig_par * par_now=pak->sig_par_h;
    int a=0; //调试
    while(par_now!=NULL)
    {
        a++; //调试
        strcpy((str *)data_p,par_now->name);
        data_p+=get_length(par_now->name);      //写入参数名
        if(par_now->data!=NULL)
        {
            //写入参数
            uint8_t * par_data=par_now->data;
            for(uint8_t i=0;i<par_now->datasize;i++)
            {
                *data_p=*par_data;
                par_data++;
                data_p++;
            }
        }
        *data_p=DATA_END;
        data_p++;
        par_now=par_now->next;      //写入参数尾部
    }
    *data_p=PAK_END;                //写入尾部

    //调试
    printf("--------------send------------------\n");
    printf("b=%d",a);
    printf("data is:%s\n",data);
    printf("showdata is:");
    show_data((str *)data);
    printf("sizeof pak:%d\n",datasize);
    net_send(netdev_id,data,datasize);        //发送DATA
    return 1;
}


int8_t net_get_pak(uint8_t * data,uint32_t datasize,netdev_id_t * netdev_id,XLsig_pak * pake)
{
    //uint8_t * data
    uint8_t size=0;
    //uint32_t datasize;
    //net_receive(data,&datasize);
    //uint8_t * data=malloc(sizeof(uint8_t)*64);
    uint8_t *data_p=data,a=0;
    str * dev_name=malloc(sizeof(str)*DEVICE_NAME_LENGTH),*dev_name_p=dev_name;
    str * sig_name=malloc(sizeof(str)*sig_NAME_LENGTH),*sig_name_p=sig_name;
    str * par_name=malloc(sizeof(str)*PAR_NAME_LENGTH),*par_name_p=par_name;

    XLsig_pak * pak=malloc(sizeof(struct XLsig_pak));
    XLsig_par * par_now=pak->sig_par_h;
    while (*data_p!=PAK_END) {              //循环解包
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
                while(*data_pp!=DATA_END){   //参数的大小
                    par_datasize++;
                    data_pp++;
                }
                par_now->data=malloc(sizeof(uint8_t)*par_datasize);
                uint8_t *par_data_p=par_now->data;
                while(*data_p!=DATA_END){   //写入参数
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
    return 1;
}
