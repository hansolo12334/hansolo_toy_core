#ifndef HANSOLO_MSG_BASE_H
#define HANSOLO_MSG_BASE_H


class hansolo_msg_base
{
public:
  int x = 0;

  virtual void printMessage() const = 0;
};

#endif