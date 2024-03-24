#ifndef HANSOLO_TIME_H
#define HANSOLO_TIME_H


#include <ctime>
#include <unistd.h>
#include"hansolo_msg_base.h"

class hansoloTime : public hansolo_msg_base
{
public:
  time_t seconds{};


  inline int getYear() const
  {
    // time_t raw_time = msg.timestamp().seconds();
    std::tm time_info;
    // localtime_r(&raw_time, &time_info);
    localtime_r(&seconds, &time_info);
    return time_info.tm_year + 1900;
  }
  inline int getMonth() const
  {
    // time_t raw_time = msg.timestamp().seconds();
    std::tm time_info;
    // localtime_r(&raw_time, &time_info);
    localtime_r(&seconds, &time_info);
    return time_info.tm_mon + 1;
  }
  inline int getDay() const
  {
    // time_t raw_time = msg.timestamp().seconds();
    std::tm time_info;
    // localtime_r(&raw_time, &time_info);
    localtime_r(&seconds, &time_info);
    return time_info.tm_mday;
  }
  inline int getHour() const
  {
    // time_t raw_time = msg.timestamp().seconds();
    std::tm time_info;
    // localtime_r(&raw_time, &time_info);
    localtime_r(&seconds, &time_info);
    return time_info.tm_hour;
  }
  inline int getMinute() const
  {
   // time_t raw_time = msg.timestamp().seconds();
    std::tm time_info;
    // localtime_r(&raw_time, &time_info);
    localtime_r(&seconds, &time_info);
    return time_info.tm_min;
  }
  inline int getSecond() const
  {
    // time_t raw_time = msg.timestamp().seconds();
    std::tm time_info;
    // localtime_r(&raw_time, &time_info);
    localtime_r(&seconds, &time_info);
    return time_info.tm_sec;
  }
};

#endif