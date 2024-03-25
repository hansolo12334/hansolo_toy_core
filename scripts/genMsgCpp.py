import sys
import os
import re


header="#ifndef MSG_TYPE_MAP_H\n#define MSG_TYPE_MAP_H\n#include<functional>\n#include <google/protobuf/any.pb.h>\n#include \"hansolo_std_msg.h\"\n#include \"hansolo_image_msg.h\"\n#include <string>\n"

header_comp="#include\"{}\"\n"

endif="#endif"

prefix="static std::map<std::string, std::function<void(google::protobuf::Any &)>> topic_type_map{\n"

compoment1="    {{\"{}\", [](google::protobuf::Any &any)\n      {{    \n        {} data_msg;\n        any.UnpackTo(&data_msg.msg);\n        data_msg.write_msg();\n        data_msg.printMessage();\n      }}\n   }},\n"
    
end="};\n\n\n\n\n"




def getMessageType():
  msgPath="../msgs/"
  
  MsgFilePath=[]
  for file in os.listdir(msgPath):
    if "Msg" in file:
      MsgFilePath.append(os.path.abspath(msgPath+file))
  
    prefix_names=[]
    sub_names=[]
    for file in MsgFilePath:
      with open(file,'r') as f:
        lines=f.readlines()
        prefix_name=[]
        sub_name=[]
        for line in lines:
          if "package" in line and "//" not in line:
            match = re.search(r'package\s+(\w+);', line)
            if match:
              prefix_name.append(match.group(1))
          if "message" in line and "//" not in line:
            match = re.search(r'message\s+(\w+)\s+{', line)
            match1 = re.search(r'message\s+(\w+){', line)
            if match:
              sub_name.append(match.group(1))
            if match1:
              sub_name.append(match1.group(1))
      prefix_names.append(prefix_name)
      sub_names.append(sub_name)
    f.close()
    
    

    
  headPath="../include/msg/"
  headFilePath=[]
  for file in os.listdir(headPath):
    if "_msg.h" in file:
      headFilePath.append(os.path.abspath(headPath+file))
  
  finalTypes=[]
  classTypes=[]
  for headpth in headFilePath:
    with open(headpth) as f:
      lines=f.readlines()
      # class hansolo_imageMsg : public
      
      for line in lines:
        if "class" in line and "//" not in line:
          match = re.search(r'class\s+(\w+)\s+\:\s+public', line)
          match1= re.search(r'class\s+(\w+)\s+\:public', line)
          match2 = re.search(r'class\s+(\w+):\s+public', line)
          if match:
            classTypes.append(match.group(1))
        
          elif match1:
            classTypes.append(match1.group(1))
        
          elif match2:
            classTypes.append(match2.group(1))
          
  
        find=False 
        for pref, sub in zip(prefix_names,sub_names):
          ff=pref[0]+"::"+sub[0]
          if ff in line:
            find=True
            finalTypes.append([pref[0],sub[0],classTypes[-1]])
        
        if find:
          # finalTypes.append(classTypes[-1])
          break
          
  return finalTypes,headFilePath

    
    
    
def main():
  gen_cpp_code("./msg_type_map.h")


def gen_cpp_code(filename):
  finalTypes,headFilePath=getMessageType()
  with open(filename,'w') as f:
    f.write(header)
    for head in headFilePath:
      f.write(header_comp.format(os.path.basename(head)))
    f.write("\n\n\n\n")
    
    f.write(prefix)
    for package_name, message_name,className in finalTypes:
      f.write(compoment1.format(package_name+"."+message_name,className))
    f.write(end)
    f.write(endif)

  f.close()

if __name__=="__main__":
  main()