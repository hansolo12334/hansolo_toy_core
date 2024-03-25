#include "image_tools.h"

//  #include <openssl/ssl.h>

//  int SSL_get_error(const SSL *ssl, int ret);

std::string hansoloImageTools::base64_encode(const std::string &in)
{
  std::string out;
  int val = 0;
  int valb = -6;
  for (unsigned char c : in)
  {
    val = (val << 8) + c;
    valb += 8;
    while (valb >= 0)
    {
      out.push_back(base64_chars[(val >> valb) & 0x3F]);
      valb -= 6;
    }
  }
  if (valb > -6)
  {
    out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }
  while (out.size() % 4)
  {
    out.push_back('=');
  }
  return out;
}

std::string hansoloImageTools::base64_decode(const std::string &in)
{
  std::string out;

  std::vector<int> T(256, -1);
  for (int i = 0; i < 64; i++)
    T[base64_chars[i]] = i;

  int val = 0, valb = -8;
  for (unsigned char c : in)
  {
    if (T[c] == -1)
      break;
    val = (val << 6) + T[c];
    valb += 6;
    if (valb >= 0)
    {
      out.push_back(char((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return out;
}

// std::string hansoloImageTools::base64_encode(const std::string &in)
// {
//   SSL_library_init();
//   BIO *bio, *b64;
//   BUF_MEM *bufferPtr;

//   b64 = BIO_new(BIO_f_base64());
//   bio = BIO_new(BIO_s_mem());
//   bio = BIO_push(b64, bio);

//   BIO_write(bio, in.c_str(), in.length());
//   BIO_flush(bio);
//   BIO_get_mem_ptr(bio, &bufferPtr);
//   BIO_set_close(bio, BIO_NOCLOSE);
//   BIO_free_all(bio);

//   std::string output(bufferPtr->data, bufferPtr->length);
//   BUF_MEM_free(bufferPtr);
//   return output;
// }

// std::string hansoloImageTools::base64_decode(const std::string &in)
// {
//   BIO *bio, *b64;
//   size_t decodeLen = in.length();
//   char *decode = new char[decodeLen];
//   memset(decode, 0, decodeLen);

//   bio = BIO_new_mem_buf(in.c_str(), -1);
//   b64 = BIO_new(BIO_f_base64());
//   bio = BIO_push(b64, bio);

//   decodeLen = BIO_read(bio, decode, in.length());
//   BIO_free_all(bio);

//   std::string output(decode, decodeLen);
//   delete[] decode;
//   return output;
// }