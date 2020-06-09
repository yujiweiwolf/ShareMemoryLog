/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   common.cpp
 * Author: Administrator
 * 
 * Created on 2020年4月2日, 下午4:54
 */

#include "common.h"

bool GetAllCpuState(map<string, CPUStat>& cpuid_stat_map) {
    ifstream ifile("/proc/stat", ios::in);
    if (!ifile.is_open()) {
        ifile.close();
        return false;
    }
    string line_str;
    while (getline(ifile, line_str)) {
        if (line_str.find("cpu ") == -1 && line_str.find("cpu") != -1) {
            stringstream ss(line_str);
            string str;
            vector<string> r;
            while (getline(ss, str, ' ')) {
                r.push_back(str);
            }
            CPUStat stat;
            memset(&stat, 0, sizeof (stat));
            int index = 1;
            stat.user = stol(r[index++]);
            stat.nice = stol(r[index++]);
            stat.system = stol(r[index++]);
            stat.idle = stol(r[index++]);
            stat.iowait = stol(r[index++]);
            stat.irq = stol(r[index++]);
            stat.softirq = stol(r[index++]);
            cpuid_stat_map[r[0]] = stat;
        }
    }
    ifile.close();
    return true;
}


CommonUnit::CommonUnit() {

}

CommonUnit::~CommonUnit() {

}

std::string CommonUnit::GetTradingDay(std::string format) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (timeinfo->tm_hour > 15) {
        rawtime += 86400;
        timeinfo = localtime(&rawtime);
    }
    while (timeinfo->tm_wday == 0 || timeinfo->tm_wday == 6) {
        rawtime += 86400;
        timeinfo = localtime(&rawtime);
    }
    timeinfo = localtime(&rawtime);
    char buf[128];
    sprintf(buf, format.c_str(), timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
    return std::string(buf);
}

long CommonUnit::GetLocalTime() {
    struct timeval _t;
    gettimeofday(&_t, NULL);
    long time = long(_t.tv_sec)*1000000 + long(_t.tv_usec);
    return time;
}

int CommonUnit::GetIdleCpu() {
    map<string, CPUStat> first_cpuid_stat_map;
    map<string, CPUStat> second_cpuid_stat_map;
    if (!GetAllCpuState(first_cpuid_stat_map)) {
        return 0;
    }
    sleep(1);
    if (!GetAllCpuState(second_cpuid_stat_map)) {
        return 0;
    }

    map<int, int> usage_cpuid_map;
    long cpu_total1 = 0;
    long cpu_use1 = 0;
    long cpu_total2 = 0;
    long cpu_use2 = 0;
    long usage = 0;
    for (auto it : first_cpuid_stat_map) {
        cpu_total1 = it.second.user + it.second.nice + it.second.system + it.second.idle + it.second.iowait + it.second.irq + it.second.softirq;
        cpu_use1 = it.second.user + it.second.nice + it.second.system + it.second.irq + it.second.softirq;
        CPUStat& stat = second_cpuid_stat_map[it.first];
        cpu_total2 = stat.user + stat.nice + stat.system + stat.idle + stat.iowait + stat.irq + stat.softirq;
        cpu_use2 = stat.user + stat.nice + stat.system + stat.irq + stat.softirq;
        usage = (cpu_use2 - cpu_use1) * 100 / (cpu_total2 - cpu_total1);

        usage_cpuid_map[(int) usage] = stoi(it.first.substr(3));
        //printf("<%d> Usage<%d%>", it.first.c_str(), usage);
    }
    return usage_cpuid_map.begin()->second;
}




