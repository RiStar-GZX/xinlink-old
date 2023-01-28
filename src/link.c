#include <link.h>

struct ins_shm *shm_create(int id)
{
    int shmid;
    shmid= shmget(id,sizeof(struct ins_shm),SHM_W|SHM_R|IPC_CREAT|IPC_EXCL);
    if(shmid==-1){
        //perror("shm create");
        printf("create\n");
    }
    struct ins_shm * shm;
    shm=(uint8_t *)shmat(shmid,0,0);
    return shm;
}

struct ins_shm *shm_get(int id)
{
    int shmid;
    shmid= shmget(id,sizeof(struct ins_shm),SHM_W|SHM_R|IPC_EXCL);
    if(shmid==-1){
        printf("create\n");
    }
    struct ins_shm * shm;
    shm=(uint8_t *)shmat(shmid,0,0);
    return shm;
}


uint8_t shm_del()
{

}
