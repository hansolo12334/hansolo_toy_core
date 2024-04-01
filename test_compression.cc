#include"data_compression.h"

#include <iostream>
#include <string>
#include <fstream>


#include <opencv2/opencv.hpp>
#include"image_tools.h"
#include"hansolo_image_msg.h"


int main()
{
  hansoloImageTools tool{};
  hansolo_imageMsg img= tool.getImage("/home/hansolo/CplusplusLearn/hansolo_toy_core/test/cat2.jpg");

  dataCompression compressTool;

  auto re = img.data;
  
  auto copy=tool.base64_encode(img.data);
  std::cout << copy.length() << ' ' << re.length() << '\n';
  
  compressTool.compressData(copy, re);

  std::cout << copy.length() << ' ' << re.length() << '\n';

  compressTool.uncompressData(re, copy);

  auto copyOut = tool.base64_decode(copy);
  std::cout << copyOut.length() << ' ' << re.length() << '\n';

  return 0;
}
