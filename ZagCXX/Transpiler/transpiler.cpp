#include "transpiler.h"
#include "environment.h"

#include <ZagIR/Ast/node.h>
#include <ZagIR/Libs/packages.h>
#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace ZagCXX;
using namespace ZagIR;

Transpiler::Transpiler() {
  typeMap.insert({"Int", {"int", ""}});
  typeMap.insert({"Str", {"std::string", "string"}});
  typeMap.insert({"Bul", {"bool", ""}});
  typeMap.insert({"Num", {"double", ""}});;

  loadedPackages = ZagIR::FetchPackages();
  currentFormat = 0;

  functionDefinition = "";
  functionDeclaration = "";

  includes.push_back("string");
}

void Transpiler::PushScope() { environment.push_back(Scope()); }

void Transpiler::PopScope() { environment.pop_back(); }

void Transpiler::AddToScope(std::string name, Object obj) {
  if (!ExistsInScope(name))
    environment.back().data.insert(std::pair<std::string, Object>(name, obj));
}

Object *Transpiler::FetchEnvironment(std::string key) {
  // Reverse direction for shadowing
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return &environment[i].data[key];
    }
  }

  AddToScope(key, Object());
  return &environment.back().data[key];
}

bool Transpiler::ExistsInEnv(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return true;
    }
  }
  return false;
}

bool Transpiler::ExistsInScope(std::string key) {
  return environment.back().data.find(key) != environment.back().data.end();
}

void Transpiler::AddPackageToScope(ZagIR::Package *package) {
  Object pack = Object(package);

  for (auto &p : package->packMap) {
    pack.AddChild(Object(p.second), p.first);
  }

  AddToScope(package->root, pack);
}

std::string Transpiler::WriteFormat(std::string text) {
  formatList.insert(std::make_pair(currentFormat, text));
  std::string res = "{" + std::to_string(currentFormat) + "}";
  currentFormat++;
  return res;
}

std::string Transpiler::GenerateSource(Node *ast) {
  std::string main = "int main(){{";

  // ast->Debug(0);

  PushScope();
  std::string mainFunc = TranspileBlock(ast);
  PopScope();

  std::string preFormat = GenerateIncludes() + functionDeclaration + main +
                          mainFunc + "}}" + functionDefinition;
  std::string globFileDeps = GlobFileDeps();
  return globFileDeps + "\n" + formatter.Format(preFormat, formatList);
}

std::string Transpiler::GlobFileDeps() {
  std::ostringstream ss;
  for (std::string s : fileDeps) {
    // std::cout << "filedep " << s << std::endl;
    std::ifstream f(s);
    if (f) {
      ss << f.rdbuf();
    }
    f.close();
  }
  return ss.str();
}

void Transpiler::AddInclude(std::string include) {
  if (std::find(includes.begin(), includes.end(), include) == includes.end()) {
    includes.push_back(include);
  }
}

std::string Transpiler::TranspileType(std::string type) {
  if (typeMap.count(type)) {
    AddInclude(std::get<1>(typeMap[type]));
    return std::get<0>(typeMap[type]);
  }
  // Error
  std::cout << "Unexpected type " << type << std::endl;
  return "";
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
  case ZagIR::NODE_FUNCTION:
    return TranspileFunction(statement);
  case ZagIR::NODE_RET:
    return TranspileReturn(statement);
  case ZagIR::NODE_EXPRESSION:
  case ZagIR::NODE_OP_BIN:
  case ZagIR::NODE_OP_UN:
  case ZagIR::NODE_IDENTIFIER:
  case ZagIR::NODE_YEP_VAL:
  case ZagIR::NODE_NOP_VAL:
  case ZagIR::NODE_NUMBER_VAL:
  case ZagIR::NODE_STRING_VAL:
  case ZagIR::NODE_CALL:
  case ZagIR::NODE_GETTER:
    return TranspileExpression(statement) + ";";
  default:
    std::cout << "Undefined node " << statement->type << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileIdentifier(Node *identifier) {
  std::string idName = identifier->data;
  if (ExistsInEnv(idName)) {
    return SanitizeIdentifier(identifier->data);
  } else {
    std::cout << "Identifier " << idName << " doesnt exist!!!" << std::endl;
    return "";
  }
}

std::string Transpiler::TranspileAssignation(Node *assignation) {
  std::string ogType = "", ogIdentifier;
  std::string Ttype = "", Tidentifier, Texpression;

  ogIdentifier = assignation->children[0]->data;
  bool declaring = false;

  if (!ExistsInEnv(ogIdentifier)) {
    declaring = true;
    if (assignation->arguments.size() > 0) {
      // Es typed
      Node *type = assignation->arguments[0];
      ogType = type->data;
      Ttype = TranspileType(ogType);

      // Aqui es podrien detectar ja dobles definicions??
      AddToScope(ogIdentifier, Object(ogType));
    } else {
      AddToScope(ogIdentifier, Object());
    }
  }
  Tidentifier = SanitizeIdentifier(ogIdentifier);
  Texpression = TranspileExpression(assignation->children[1]);

  if (declaring) {
    if (Ttype == "")
      Ttype = "auto";

    return Ttype + " " + Tidentifier + " " + assignation->data + " " +
           Texpression;

  } else {
    return Tidentifier + " " + assignation->data + " " + Texpression;
  }
}

std::string Transpiler::TranspileExpression(Node *expression) {
  switch (expression->type) {
  case NODE_NUMBER_VAL:
    return expression->data;
    break;
  case NODE_STRING_VAL:
    return "std::string(\"" + expression->data + "\")";
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
  case NODE_CALL:
    return TranspileCall(expression);
    break;
  case NODE_GETTER:
    return TranspileGetter(expression);
    break;
  default:
    // std::cout << "Unexpected node:" << std::endl;
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
    res += "){{";

    PushScope();
    res += TranspileBlock(ifNode->children[i]);
    PopScope();

    res += "}}";
  }
  if (ifNode->children.size() > ifNode->arguments.size()) {
    res += "else {{";

    PushScope();
    res += TranspileBlock(ifNode->children[i]);
    PopScope();

    res += "}}";
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
      // De moment posem només ints
      AddToScope(iterator->arguments[i]->data, Object("Int"));
      std::string identifier = TranspileIdentifier(iterator->arguments[i]);
      if (interval->children.size() > 1) {
        from = TranspileExpression(interval->children[0]);
        to = TranspileExpression(interval->children[1]);
      } else {
        to = TranspileExpression(interval->children[0]);
      }

      res += "for(int " + identifier + " = ";
      res +=
          from + "; " + identifier + " <= " + to + "; " + identifier + "++){{";
    }
    // std::cout << iterator->arguments.size() << std::endl;

    block = TranspileBlock(lup->children[0]);
    PopScope();
    res += block;
    for (int i = 0; i < iterator->arguments.size(); i++)
      res += "}}";

  } else {
    // Simple while structure
    if (lup->arguments.size() > 1) {
      res += "for(" + TranspileAssignation(lup->arguments[0]) + "; ";
      res += TranspileExpression(lup->arguments[1]) + "; ";
      res += TranspileAssignation(lup->arguments[2]) + ")";
    } else {
      res += "while(" + TranspileExpression(lup->arguments[0]) + ")";
    }

    block = TranspileBlock(lup->children[0]);
    PopScope();
    res += "{{" + block + "}}";
  }

  return res;
}

std::string Transpiler::TranspileGet(ZagIR::Node *getNode) {
  std::string value = getNode->data;
  std::string realImport;
  bool lib;
  // std::cout << "Value size: " << value.size() << std::endl;
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
      // std::cout << "Found package" << std::endl;
      AddPackageToScope(&loadedPackages[i]);
      // I ara aqui hauria de crear l'objecte i importar-lo
    }
  }
  return "";
}

std::string Transpiler::TranspileFunction(ZagIR::Node *function) {

  // Sanitization?
  std::string funcName = "_f_" + function->data;
  // For the function declaration we have 2 arguments: A Type of return value
  // and a list of args for the function. ej:
  // returnType -> int
  // arguments -> (int a, int b)
  std::string returnType = "", arguments = "";
  std::string argPiled;
  std::string argType, argIdentifier;

  PushScope();
  for (int i = 0; i < function->arguments.size(); i++) {
    if (function->arguments[i]->type == ZagIR::NODE_TYPE) {
      returnType = TranspileType(function->arguments[i]->data);
    } else if (function->arguments[i]->type == ZagIR::NODE_ARGS) {
      Node *args = function->arguments[i];
      arguments = "(";
      for (int j = 0; j < args->children.size(); j++) {
        Node *arg = args->children[j];

        argType = arg->arguments[0]->data;
        argIdentifier = arg->children[0]->data;

        AddToScope(argIdentifier, Object(argType));

        argPiled =
            TranspileType(argType) + " " + SanitizeIdentifier(argIdentifier);
        arguments += argPiled;
        if (j < args->children.size() - 1)
          arguments += ",";
      }
      arguments += ")";
    } else {
      std::cout << "Bad formatted AST" << std::endl;
    }
  }

  std::string defBlock = TranspileBlock(function->children[0]);
  PopScope();

  if (returnType == "")
    returnType = "void";
  functionDeclaration += returnType + " " + funcName + arguments + ";";
  functionDefinition +=
      returnType + " " + funcName + arguments + "{{" + defBlock + "}}";

  return "";
}

std::string Transpiler::TranspileReturn(ZagIR::Node *ret) {
  if (ret->children.size() > 0)
    return "return " + TranspileExpression(ret->children[0]) + ";";
  else
    return "return;";
}

std::string Transpiler::TranspileCall(ZagIR::Node *call) {
  // TODO: Comprovar que la funció existeix lmao
  std::string funcName = "_f_" + call->data;
  return TranspileGCall(funcName, call);
}

std::string Transpiler::TranspileGCall(std::string callName,
                                       ZagIR::Node *call) {
  std::string args = "";

  for (int i = 0; i < call->arguments.size(); i++) {
    args += TranspileExpression(call->arguments[i]);
    if (i < call->arguments.size() - 1)
      args += ",";
  }

  // We should check if args are valid
  return callName + "(" + args + ")";
}

std::string Transpiler::TranspileGetter(ZagIR::Node *getter) {
  if (ExistsInEnv(getter->arguments[0]->data)) {
    Object *scoped = FetchEnvironment(getter->arguments[0]->data);
    // scoped->Print();
    ZagIR::Node *currentGetter = getter;
    if (scoped->objType == OBJECT_PACKAGE) {
      Object *packageObject = scoped;
      // visit recursively until we get our PackCall and then call

      while ((scoped->objType == OBJECT_PACKAGE ||
              scoped->objType == OBJECT_CCONTAINER) &&
             currentGetter->children.size() > 0) {
        currentGetter = currentGetter->children[0];
        if (!(currentGetter->type == ZagIR::NODE_GETTER ||
              currentGetter->type == ZagIR::NODE_CALL)) {
          // Ens hem quedat sense getters
          std::cout << "Not reached till the end" << std::endl;
        }
        if (currentGetter->type == ZagIR::NODE_CALL) {
          scoped = scoped->GetObject(currentGetter->data);
        } else
          scoped = scoped->GetObject(currentGetter->arguments[0]->data);
      }

      if (currentGetter->type == NODE_GETTER) {
        // Més getters que el que hi ha al objecte
        // Podria donar support a constants???
        std::cout << "Invalid" << std::endl;
      }

      // scoped->Print();
      ZagIR::PackCall pack = scoped->GetCFunctionData();
      if (currentGetter->type == NODE_CALL) {
        // Add fileDeps and return

        for (int i = 0; i < pack.fileDeps.size(); i++) {
          fileDeps.insert(packageObject->GetPackage()->path + "/" +
                          pack.fileDeps[i]);
        }

        return TranspileGCall(packageObject->GetPackage()->space +
                                  "::" + pack.funcName,
                              currentGetter);
      }
      // Hem de carregar fileDeps

    } else {
      std::cout << "Undefined object" << std::endl;
    }
  }
  return "";
}

void Transpiler::LoadLib(std::string libName) { ZagIR::FetchPackages(); }
