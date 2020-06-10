#include "memorymanager.h"

CMemoryManager* CMemoryManager::GetInstance() {
    static CMemoryManager manager;
    return &manager;
}

void CMemoryManager::Init() {
    index_ = 0;
    mmap_length_ = sysconf(_SC_PAGESIZE) * MMAPSIZE;
    while(!header_) {
        header_ = (TMemoryHeader*)InitMemory("share", "log", index_++, MMAPSIZE);
    }
    //std::string _tmp = fmt::format("{0} index {1}", "CMemoryManager::Init", index_);
}


TMemoryMessage* CMemoryManager::GetMemoryMessage(TMemoryHeader* header, int offset) {
    TMemoryMessage* temp = (TMemoryMessage*)((char*)header + offset);
    temp->field = (char*)((char*)header + offset + sizeof(TMemoryMessage));
    return temp;
}

void CMemoryManager::AllocateLogBuffer(std::string& loginfo) {
    int size = loginfo.length();
    int offset = 0;
    TMemoryMessage *current_message = nullptr;
    {
        std::lock_guard<std::mutex> lck(mtx);
        offset = header_->offset.load(memory_order_acquire);    
    
        current_message = (TMemoryMessage*)((char*)header_ + offset);        
        if ((offset + size + sizeof (TMemoryMessage)) < mmap_length_) {
            current_message = (TMemoryMessage*)((char*)header_ + offset);
        } else {
            header_->finished = true;
            munmap((void*)header_, mmap_length_);           
                        
            header_ = (TMemoryHeader*)InitMemory("share", "log", index_++, MMAPSIZE);
            offset = sizeof(TMemoryHeader);
            current_message = (TMemoryMessage*) ((char*)header_ + offset);
            printf("%s offset:%d  size: %d\n", __func__, header_->offset.load(memory_order_acquire), size);
        }
        header_->offset.fetch_add(size + sizeof(TMemoryMessage), memory_order_release);
    }
    memcpy((char*) current_message + sizeof (TMemoryMessage), loginfo.c_str(), loginfo.length());
    current_message->header.length = size;
    current_message->header.type = 0;    
}

TMemoryHeader* CMemoryManager::GetMemoryHeader() {
    return header_;
}

int CMemoryManager::GetFileNum(const char* dirname) {
    int num = 0;
    struct dirent *ptr;
    DIR *dir;
    dir = opendir(dirname);
    while ((ptr = readdir(dir)) != NULL) {
        //printf("%s\n", ptr->d_name);
        if (strlen(ptr->d_name) > 10) {
            num++;
        }
    }
    closedir(dir);
    return num;
}

void CMemoryManager::ReadLog() {
    int file_num = GetFileNum("share");
    int read_offset = sizeof (TMemoryHeader);
    int line = 0;
    TMemoryHeader* header;
    for (int Index = 0; Index < file_num; Index++) {
        header = (TMemoryHeader*) InitMemory("share", "log", Index, MMAPSIZE, true);
        while (read_offset < header->offset.load(memory_order_acquire)) {
            TMemoryMessage *message = GetMemoryMessage(header, read_offset);
            //printf("<%s> read_offset<%d> header<%x> <%x>\n", __FUNCTION__, read_offset, header, message);
            if (message->header.type == 0) {
                printf("line<%d> %s\n", line++, message->field);
            }
            read_offset += (message->header.length + sizeof (TMemoryMessage));
        }
        if (Index < file_num -1) {
            header = (TMemoryHeader*) InitMemory("share", "log", index_++, MMAPSIZE, true);
            read_offset = sizeof (TMemoryHeader);
        }
    }
}










