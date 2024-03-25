#! /usr/bin/bash

if [ -f "./include/tools/msg_type_map.h" ]; then
  echo "头文件存在"
else  
  echo "生成消息头文件..."  
  cd ./scripts  
  python ./genMsgCpp.py
  cd ..
fi

if [ -d "./build" ]; then
  echo "build文件夹存在"
else
  cmake -B ./build
  cmake --build ./build
fi
