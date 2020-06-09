/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   common.h
 * Author: Administrator
 *
 * Created on 2020年4月2日, 下午4:54
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string.h>  
#include <iostream>
#include <sys/time.h>
#include <cstdlib>
#include <vector>
#include <sys/select.h>
#include <errno.h>
#include "iconv.h"
#include <string>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include "iconv.h"
#include <dirent.h>
#include <map>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

struct CPUStat {
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;
};

class CommonUnit {
public:
    CommonUnit();
    ~CommonUnit();
    static std::string GetTradingDay(std::string format = "%04d%02d%02d");    
    static long GetLocalTime();
    static int GetIdleCpu();
};

#endif /* COMMON_H */

