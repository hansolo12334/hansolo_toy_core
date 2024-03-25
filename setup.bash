#! /usr/bin/bash

echo "生成消息头文件..."  
cd ./scripts  
python ./genMsgCpp.py
cd ..

cmake -B ./build
cmake --build ./build