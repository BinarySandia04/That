#include "transpiler.h"

#include <algorithm>
#include <iostream>

using namespace ZagCXX;
using namespace ZagIR;

Transpiler::Transpiler() {
  typeMap.insert({"Int", {"int", ""}});
  typeMap.insert({"Str", {"std::string", "string"}});
}

std::string Transpiler::GenerateSource(Node *ast) {
  std::string main = "int main(){";

  ast->Debug(0);

  std::string mainFunc = TranspileSpace(ast);

  return GenerateIncludes() + main + mainFunc + "}";
}

void Transpiler::AddInclude(std::string include) {
  if (std::find(includes.begin(), includes.end(), include) == includes.end()) {
    includes.push_back(include);
  }
}

std::string Transpiler::ConvertType(std::string type) {
  if (typeMap.count(type)){
    AddInclude(std::get<1>(typeMap[type]));
    return std::get<0>(typeMap[type]);
  }
  // Error
  std::cout << "Unexpected type";
}

std::string Transpiler::GenerateIncludes(){
  std::string res = "";
  for(int i = 0; i < includes.size(); i++){
    if(includes[i] != ""){
      res += "#include <" + includes[i] + ">\n";
    }
  }
  return res;
}

std::string Transpiler::TranspileSpace(Node *space) {
  std::string content = "";
  for (int i = 0; i < space->children.size(); i++) {
    content += TranspileStatement(space->children[i]);
  }
  return content;
}

std::string Transpiler::TranspileStatement(Node *statement) {

  switch (statement->type) {
  case ZagIR::NODE_ASSIGNATION:
    return TranspileAssignation(statement);
    break;
  default:
    std::cout << "Undefined node" << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileAssignation(Node *assignation) {
  std::string Ttype = "int", Tidentifier, Texpression;
  if (assignation->arguments.size() > 0) {
    // Es typed
    Node *type = assignation->arguments[0];
    Ttype = TranspileType(type);
  }

  Tidentifier = TranspileIdentifier(assignation->children[0]);
  Texpression = TranspileExpression(assignation->children[1]);

  return Ttype + " " + Tidentifier + " = " + Texpression + ";";
}

// TODO: Nested types
std::string Transpiler::TranspileType(Node *type) {
  std::string defType = type->data;
  // Aqui hauria de posar un map aware de totes les variables i tal
  return ConvertType(defType);
}

std::string Transpiler::TranspileIdentifier(Node *identifier) {
  return identifier->data;
}

std::string Transpiler::TranspileExpression(Node *expression) {
  switch (expression->type) {
  case NODE_NUMBER_VAL:
    return expression->data;
    break;
  case NODE_STRING_VAL:
    return "\"" + expression->data + "\"";
    break;
  case NODE_OP_BIN:
    return TranspileBinary(expression);
    break;
  default:
    std::cout << "Unexpected node" << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileBinary(Node *binary) {
  return "(" + TranspileExpression(binary->children[0]) + " " + binary->data + " " +
         TranspileExpression(binary->children[1]) + ")";
}
