#include"hansolo_topic.h"

ABSL_FLAG(std::string, option, "",
                "hansolotopic选项");



int main(int argc,char **argv)
{
  absl::SetProgramUsageMessage(absl::StrCat("简单使用方法:\n", argv[0]," option=''"));

  auto posArgv=absl::ParseCommandLine(argc, argv);
  // absl::GetFlag(FLAGS_option);

  if(posArgv.size()<=1){
    std::cout << absl::StrCat("简单使用方法:\n", argv[0], " option=''") << std::endl;
    return 0;
  }

  hansolo_topic topic{};

  if(posArgv.size()==2 && std::string(posArgv[1],strlen(posArgv[1])) =="list"){
    topic.GetTopics();
  }
  if(posArgv.size()==3 && std::string(posArgv[1],strlen(posArgv[1])) =="echo"){
    topic.EchoTopic(posArgv[2]);
  }

  return 0;
}