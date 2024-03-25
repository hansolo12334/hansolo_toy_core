#include<iostream>
#include<string>
#include<fstream>
#include <cstring>
#include<openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include<opencv2/opencv.hpp>


using namespace std;


std::string base64_encode(const std::string &in) {

    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, in.c_str(), in.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string output(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);
    return output;
}

std::string base64_decode(const std::string &in) {

    

      BIO *bio, *b64;
      size_t decodeLen = in.length();
      char* decode = new char[decodeLen];
      memset(decode, 0, decodeLen);

      bio = BIO_new_mem_buf(in.c_str(), -1);
      b64 = BIO_new(BIO_f_base64());
      bio = BIO_push(b64, bio);

      decodeLen = BIO_read(bio, decode, in.length());
      BIO_free_all(bio);

      std::string output(decode, decodeLen);
      delete [] decode;
    return output;
}

int main()
{
  string imagePath{"/home/hansolo/CplusplusLearn/hansolo_toy_core/test/cat2.jpg"};
  ifstream  input(imagePath, ios::binary);
  if (!input) {
    std::cerr << "无法打开图片文件 " << imagePath << std::endl;
    return 1;
  }
  string imageData((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

  vector<uchar> imageDataVec(imageData.begin(), imageData.end());
  cv::Mat img = cv::imdecode(imageDataVec, cv::IMREAD_COLOR);
   if (img.empty()) {
    std::cerr << "无法加载图片。" << std::endl;
    return 1;
  }

  cv::namedWindow("Image",cv::WINDOW_NORMAL);

  cv::resize(img, img, cv::Size(img.size[1]/6,img.size[0]/6));
  cv::imshow("Image", img);

  // 等待用户关闭窗口
  cv::waitKey(0);

  std::string imageBase64 = base64_encode(imageData);

  // 打印 base64
  std::cout << imageBase64.length() << std::endl;

  return 0;
}