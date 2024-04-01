#ifndef DATA_COMPRESSION_H
#define DATA_COMPRESSION_H


#include"zlib.h"
#include<string>
#include <string.h>
#include<iostream>


class dataCompression
{
  // dataCompression();
  // ~dataCompression();
public:
  bool compressData(const std::string &src,std::string &re)
  {
    Byte compr[src.length()];
    uLong compressedLen = sizeof(compr) / sizeof(compr[0]);

    const char *srcChar = src.c_str();
    uLong srcLen = strlen(srcChar) + 1;

    compress(compr, &compressedLen, (const Bytef *)srcChar, srcLen);
 
    re = std::string{reinterpret_cast<char *>(compr), compressedLen};

    return true;
  }

  bool uncompressData(const std::string &src,std::string &re)
  {
    Byte uncompr[6000000];
    uLong uncompressedLen = sizeof(uncompr) / sizeof(uncompr[0]);

    const char *srcChar = src.c_str();
    uLong srcLen = strlen(srcChar);
    uncompress(uncompr, &uncompressedLen, (const Bytef*)srcChar,srcLen);

    re= std::string{reinterpret_cast<char *>(uncompr), uncompressedLen};

    return true;
  }
};

#endif