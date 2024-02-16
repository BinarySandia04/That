#include <string>
#include <vector>

#pragma once

namespace ZagIR {

enum NodeType {
  NODE_UNDEF, // Undef node (?)
  NODE_SPACE, // Top file node
  NODE_BLOCK, // Child has various nodes
  NODE_FUNCTION, // Data is identifier, args have NODE_ARGS and/or NDOE_TYPE return. Child is block
  NODE_CALL, // Child is callee, args are expression arguments
  NODE_GET, // 
  NODE_GETTER,
  NODE_RET, // Childs are expressions to return
  NODE_BRK, // Data has identifier, child is the expression to break
  NODE_KIN, // Contains KIN_ENTRY as childs
  NODE_KIN_ENTRY, // Data has the accessor. Child has the thing
  NODE_ARG, // Children has identifier, args has type if typed
  NODE_ARGS, // Multiple NODE_ARG as children
  NODE_IF, // Arg has expressions childs are blocks. If args + 1 = childs then has else
  NODE_LUP, // data has identifier. Child is block. Three forms: 
            // Args: LUP_ITERATORS
            // Args: ASSIGNATION, EXPRESSION, ASSIGNATION
            // Args: EXPRESSION
  NODE_OP_BIN, // Data contains op
  NODE_OP_UN, // Data contains op
  NODE_EXPRESSION, // Undef expression. Should not appear in final ast
  NODE_YEP_VAL, // yep
  NODE_NOP_VAL, // nop
  NODE_NIL_VAL, // nil
  NODE_ASSIGNATION, // Assignation as = or +=, -=, *=. If not '=' maybe is not declaration
  NODE_TYPE, // Childs are nested types <>. Data is the identifier of type
  NODE_ARRAY, // Childs are expressions
  NODE_INTERVAL, // Children are one to two expressions
  NODE_LUP_ITERATORS, // Arguments are identifiers, childs are intervals
  NODE_NUMBER_VAL, // Data is the number value
  NODE_STRING_VAL, // Data is the string value
  NODE_IDENTIFIER, // Data is the identifier name
};

class Node {
public:
  NodeType type;

  std::vector<Node *> children;
  std::vector<Node *> arguments;

  std::string data;

  Node();
  ~Node();
  Node(NodeType);
  Node(NodeType, std::string);

  void Debug(int);
private:
  void PrintTabs(int);
};

}; // namespace Zag
