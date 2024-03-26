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

_hansolotopic()
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="list echo"
    # opts="$(./hansolotopic list)"

    if [[ ${cur} == * ]] ; then
        COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
        return 0
    fi
}
complete -F _hansolotopic ./hansolotopic