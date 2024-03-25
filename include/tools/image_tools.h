#ifndef IMAGE_TOOLS_H
#define IMAGE_TOOLS_H

// #include <openssl/bio.h>
// #include <openssl/evp.h>
// #include <openssl/buffer.h>
#include<string>
#include<cstring>
#include<vector>

class hansoloImageTools
{

public:
  std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

  std::string base64_encode(const std::string &in);

  std::string base64_decode(const std::string &in);
};

#endif