
#ifndef CMEMORYMANAGER_H
#define CMEMORYMANAGER_H
#include <fcntl.h> 
#include <atomic>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <pthread.h>
#include <mutex>
#include <dirent.h>

#include "fmt/format.h"
#include "common.h"

using namespace std;

#define MMAPSIZE 256
#define LEFT 1024

struct TMemoryHeader {
    std::atomic<int> msg_count;
    std::atomic<int> offset;
    int length; 
    bool finished;
};

struct TMessageHeader {
    int type;
    int length;
};

struct TMemoryMessage {
    TMessageHeader header;
    char *field;
};


static char* InitMemory(const char *dir_name, const char* file_name, int index, int shmsize, bool read = false) {
    umask(0000);
    char file[128] = {0};
    sprintf(file, "%s/mmap_%s_%s_%d", dir_name, file_name, CommonUnit::GetTradingDay().c_str(), index);
    int fd = 0;
    fd = open(file, O_RDWR | O_CREAT, 0666);
    ftruncate(fd, sysconf(_SC_PAGESIZE) * shmsize);
    //ftruncate(fd, LENGTH);
    struct stat st;
    fstat(fd, &st);
    char* mapped_mem = (char*) mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE | MAP_NONBLOCK, fd, 0);
    if (MAP_FAILED == mapped_mem) {
        //perror("error mapping file to buffer");
        exit(EXIT_FAILURE);
    }
    if (madvise(mapped_mem, st.st_size, MADV_WILLNEED) != 0 && mlock(mapped_mem, st.st_size) != 0) {
        munmap(mapped_mem, st.st_size);
        close(fd);
        //perror("error in madvise or mlock!\n");
        exit(EXIT_FAILURE);
    }
    TMemoryHeader* first = (TMemoryHeader*)mapped_mem;
    if (first->offset.load(memory_order_acquire) == 0) {
        first->offset.fetch_add(sizeof(TMemoryHeader), memory_order_release);
        first->length = shmsize * sysconf(_SC_PAGESIZE);
        memset(mapped_mem + sizeof(TMemoryHeader), 0, first->length - sizeof(TMemoryHeader));
    }
    close(fd);
    if(first->finished && read == false) {
        return nullptr;
    }    
    return mapped_mem; 
}


class CMemoryManager {
public:
    static CMemoryManager* GetInstance();
    
    void Init();    
    //直接写日志
    void AllocateLogBuffer(std::string& loginfo);    
    
    void ReadLog();
    int GetFileNum(const char* dir);    
    TMemoryHeader* GetMemoryHeader();
    TMemoryMessage* GetMemoryMessage(TMemoryHeader* header, int offset);      
    
private:
    CMemoryManager(){};    
    ~CMemoryManager(){};    
    
private:    
    int mmap_length_;
    int index_ = 0;    
    std::mutex mtx;
    TMemoryHeader* header_ = nullptr;
};

#endif /* CMEMORYMANAGER_H */

