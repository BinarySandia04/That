#include "transpiler.h"

#include <ZagIR/Ast/node.h>
#include <ZagIR/Libs/packages.h>
#include <algorithm>
#include <iostream>

using namespace ZagCXX;
using namespace ZagIR;

Object::Object() { objType = OBJECT_EMPTY; }

Object::Object(std::string type) {
  objType = OBJECT_VARIABLE;
  this->type = type;
}

void Object::AddPackageCall(std::string path, ZagIR::PackCall packCall){

}

Transpiler::Transpiler() {
  typeMap.insert({"Int", {"int", ""}});
  typeMap.insert({"Str", {"std::string", "string"}});

  loadedPackages = ZagIR::FetchPackages();
}

void Transpiler::PushScope() { environment.push_back(Scope()); }

void Transpiler::PopScope() { environment.pop_back(); }

void Transpiler::AddToScope(std::string name, Object obj) {
  if (!ExistsInScope(name))
    environment.back().data.insert(std::make_pair(name, obj));
}

Object *Transpiler::FetchEnvironment(std::string key) {
  // Reverse direction for shadowing
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return &environment[i].data[key];
    }
  }

  AddToScope(key, Object());
}

bool Transpiler::ExistsInScope(std::string key) {
  return environment.back().data.find(key) != environment.back().data.end();
}

void Transpiler::AddPackageToScope(ZagIR::Package package){
  Object pack = Object();
  pack.objType = OBJECT_CONTAINER;
  
  for(auto &p : package.packMap){
    pack.AddPackageCall(p.first, p.second);
  }

  AddToScope(package.root, pack);
}

std::string Transpiler::GenerateSource(Node *ast) {
  std::string main = "int main(){";

  // ast->Debug(0);

  PushScope();
  std::string mainFunc = TranspileBlock(ast);
  PopScope();

  return GenerateIncludes() + main + mainFunc + "}";
}

void Transpiler::AddInclude(std::string include) {
  if (std::find(includes.begin(), includes.end(), include) == includes.end()) {
    includes.push_back(include);
  }
}

std::string Transpiler::ConvertType(std::string type) {
  if (typeMap.count(type)) {
    AddInclude(std::get<1>(typeMap[type]));
    return std::get<0>(typeMap[type]);
  }
  // Error
  std::cout << "Unexpected type";
}

std::string Transpiler::GenerateIncludes() {
  std::string res = "";
  for (int i = 0; i < includes.size(); i++) {
    if (includes[i] != "") {
      res += "#include <" + includes[i] + ">\n";
    }
  }
  return res;
}

std::string Transpiler::SanitizeIdentifier(std::string idName) {
  return "_" + idName;
}

std::string Transpiler::TranspileBlock(Node *space) {
  std::string content = "";
  for (int i = 0; i < space->children.size(); i++) {
    content += TranspileStatement(space->children[i]);
  }
  return content;
}

std::string Transpiler::TranspileStatement(Node *statement) {

  switch (statement->type) {
  case ZagIR::NODE_ASSIGNATION:
    return TranspileAssignation(statement) + ";";
  case ZagIR::NODE_IF:
    return TranspileIf(statement);
  case ZagIR::NODE_LUP:
    return TranspileLup(statement);
  case ZagIR::NODE_GET:
    return TranspileGet(statement);
  case ZagIR::NODE_EXPRESSION:
  case ZagIR::NODE_OP_BIN:
  case ZagIR::NODE_OP_UN:
  case ZagIR::NODE_IDENTIFIER:
  case ZagIR::NODE_YEP_VAL:
  case ZagIR::NODE_NOP_VAL:
  case ZagIR::NODE_NUMBER_VAL:
  case ZagIR::NODE_STRING_VAL:
    return TranspileExpression(statement) + ";";
  default:
    std::cout << "Undefined node " << statement->type << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileIdentifier(Node *identifier) {
  std::string idName = identifier->data;
  if(ExistsInScope(idName)){
    return SanitizeIdentifier(identifier->data);
  } else {
    std::cout << "Identifier doesnt exist!!!" << std::endl;
    return "";
  }
}

std::string Transpiler::TranspileAssignation(Node *assignation) {
  std::string ogType = "", ogIdentifier;
  std::string Ttype = "", Tidentifier, Texpression;

  ogIdentifier = assignation->children[0]->data;

  if (assignation->arguments.size() > 0) {
    // Es typed
    Node *type = assignation->arguments[0];
    ogType = type->data;
    Ttype = ConvertType(ogType);
    
    // Aqui es podrien detectar ja dobles definicions??
    AddToScope(ogIdentifier, Object(ogType));
  } else {
    AddToScope(ogIdentifier, Object());
  }

  Tidentifier = SanitizeIdentifier(ogIdentifier);
  Texpression = TranspileExpression(assignation->children[1]);

  return Ttype + " " + Tidentifier + " = " + Texpression;
}

std::string Transpiler::TranspileExpression(Node *expression) {
  switch (expression->type) {
  case NODE_NUMBER_VAL:
    return expression->data;
    break;
  case NODE_STRING_VAL:
    return "\"" + expression->data + "\"";
    break;
  case NODE_YEP_VAL:
    return "true";
    break;
  case NODE_NOP_VAL:
    return "false";
    break;
  case NODE_OP_BIN:
    return TranspileBinary(expression);
    break;
  case NODE_OP_UN:
    return TranspileUnary(expression);
    break;
  case NODE_IDENTIFIER:
    return TranspileIdentifier(expression);
    break;
  default:
    std::cout << "Unexpected node" << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileBinary(Node *binary) {
  return "(" + TranspileExpression(binary->children[0]) + " " + binary->data +
         " " + TranspileExpression(binary->children[1]) + ")";
}

std::string Transpiler::TranspileUnary(ZagIR::Node *unary) {
  return unary->data + TranspileExpression(unary->children[0]);
}

std::string Transpiler::TranspileIf(ZagIR::Node *ifNode) {
  // Arg has expressions childs are blocks. If args + 1 = childs then has else
  std::string res = "";
  int i;
  for (i = 0; i < ifNode->arguments.size(); i++) {
    if (i == 0)
      res += "if";
    else
      res += "else if";
    res += "(";
    res += TranspileExpression(ifNode->arguments[i]);
    res += "){";

    PushScope();
    res += TranspileBlock(ifNode->children[i]);
    PopScope();

    res += "}";
  }
  if (ifNode->children.size() > ifNode->arguments.size()) {
    res += "else {";

    PushScope();
    res += TranspileBlock(ifNode->children[i]);
    PopScope();

    res += "}";
  }
  return res;
}

std::string Transpiler::TranspileLup(ZagIR::Node *lup) {
  // data has identifier. Child is block. Three forms:
  // Args: LUP_ITERATORS
  // Args: ASSIGNATION, EXPRESSION, ASSIGNATION
  // Args: EXPRESSION
  std::string res = "";

  std::string block;
  PushScope();
  if (lup->arguments[0]->type == ZagIR::NODE_LUP_ITERATORS) {
    Node *iterator = lup->arguments[0];
    // Arguments are identifiers, childs are intervals
    // We have or one child or same identifiers as childs
    bool multiIterators = false;
    if (iterator->children.size() > 1) {
      multiIterators = true;
      if (iterator->children.size() != iterator->arguments.size()) {
        std::cout << "Invalid interval number" << std::endl;
        return "";
      }
    }
    // We get all arguments and we create consecutive fors nested
    for (int i = 0; i < iterator->arguments.size(); i++) {
      Node *interval;
      if (multiIterators)
        interval = iterator->children[i];
      else
        interval = iterator->children[0];
      std::string from = "0";
      std::string to = "0";
      std::string identifier = TranspileIdentifier(iterator->arguments[i]);
      if (interval->children.size() > 1) {
        from = TranspileExpression(interval->children[0]);
        to = TranspileExpression(interval->children[1]);
      } else {
        to = TranspileExpression(interval->children[0]);
      }

      res += "for(int " + identifier + " = ";
      res += from + "; " + identifier + " < " + to + "; " + identifier + "++){";
    }
    std::cout << iterator->arguments.size() << std::endl;
     
    block = TranspileBlock(lup->children[0]);
    PopScope();
    res += block + std::string(iterator->arguments.size(), '}');

  } else {
    // Simple while structure
    std::string fExpr = TranspileExpression(lup->arguments[0]);
    if (lup->arguments.size() > 1) {
      res += "for(" + TranspileAssignation(lup->arguments[0]) + "; " +
             TranspileExpression(lup->arguments[1]) + "; " +
             TranspileAssignation(lup->arguments[2]) + ")";
    } else {
      res += "while(" + fExpr + ")";
    }

    block = TranspileBlock(lup->children[0]);
    PopScope();
    res += "{" + block + "}";
  }

  return res;
}

std::string Transpiler::TranspileGet(ZagIR::Node *getNode) {
  std::string value = getNode->data;
  std::string realImport;
  bool lib;
  std::cout << "Value size: " << value.size() << std::endl;
  getNode->Debug(0);
  if (value[0] == '\'') {
    lib = true;
    realImport = value.substr(1, value.size() - 1);
    LoadLib(realImport);
  } else {
    lib = false;
    realImport = value.substr(1, value.size() - 2);
    // Normal import
  }

  // TODO: Hauria de carregar només tal package no mirar entre tots
  for (int i = 0; i < loadedPackages.size(); i++) {
    if (loadedPackages[i].name == realImport) {
      std::cout << "Found package" << std::endl;
      AddPackageToScope(loadedPackages[i]);
      // I ara aqui hauria de crear l'objecte i importar-lo
    }
  }
  return realImport;
}

void Transpiler::LoadLib(std::string libName) { ZagIR::FetchPackages(); }
