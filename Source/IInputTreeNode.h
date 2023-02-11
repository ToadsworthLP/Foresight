#pragma once
#include "NoteContext.h"

class IInputTreeNode {
  public:
    ~IInputTreeNode() {}
    virtual NoteContext& visit(NoteContext& context) = 0;
};