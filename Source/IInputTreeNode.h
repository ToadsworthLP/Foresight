#pragma once
#include "NoteContext.h"

class IInputTreeNode {
public:
	virtual ~IInputTreeNode() = default;
	virtual NoteContext& visit(NoteContext& context) = 0;
};