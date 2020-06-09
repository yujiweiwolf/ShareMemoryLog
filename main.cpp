#include <cstdlib>
#include "memorymanager.h"
#define NUM_THREADS 5

struct TPositionInfo {
    char instrument_id[16];
    int volume;
    int price;
    long local_time;
    pthread_t tid;
};

void Write() {
    for (int Index = 0; Index < 10000; Index++) {
        TPositionInfo info;
        memset(&info, 0, sizeof (info));
        sprintf(info.instrument_id, "600%03d", Index);
        info.volume = Index;
        info.price = Index * 2;
        info.local_time = CommonUnit::GetLocalTime();
        std::string _temp = fmt::format("instrument_id<{0}> volume<{1}> price<{2}> local_time<{3}> pthreadid<{4}>", info.instrument_id, info.volume, info.price, info.local_time, info.tid);
        CMemoryManager::GetInstance()->AllocateLogBuffer(_temp);
    }
}


void* Pthread_Write(void* args) {
    for (int Index = 0; Index < 10000; Index++) {
        TPositionInfo info;
        memset(&info, 0, sizeof (info));
        sprintf(info.instrument_id, "600%03d", Index);
        info.volume = Index;
        info.price = Index * 2;
        info.local_time = CommonUnit::GetLocalTime();
        info.tid = pthread_self();
        std::string _temp = fmt::format("instrument_id<{0}> volume<{1}> price<{2}> local_time<{3}> pthreadid<{4}>", info.instrument_id, info.volume, info.price, info.local_time, info.tid);
        CMemoryManager::GetInstance()->AllocateLogBuffer(_temp);
        usleep(1);
    }
}


int main(int argc, char** argv) {
    printf("MemoryHeader<%d> MemoryMessage<%d> TPositionInfo<%d>\n", sizeof(TMemoryHeader), sizeof(TMemoryMessage), sizeof(TPositionInfo));
    
    std::string s = fmt::format("{0}is{1}", "abra",12);
    std::cout << s <<std::endl;
      
    CMemoryManager::GetInstance()->Init();
    pthread_t tids[NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; ++i) {
        int ret = pthread_create(&tids[i], NULL, Pthread_Write, NULL);
        if (ret != 0) {
           cout << "pthread_create error: error_code=" << ret << endl;
        }
    }       
    sleep(20);
    
//    Write(); 
//    sleep(2);
    
    //CMemoryManager::GetInstance()->ReadLog();   
}

