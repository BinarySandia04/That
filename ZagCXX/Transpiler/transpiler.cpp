#include "transpiler.h"
#include "environment.h"
#include "object.h"

#include <ZagIR/Ast/node.h>
#include <ZagIR/Libs/packages.h>
#include <ZagIR/Logs/logs.h>
#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace ZagCXX;
using namespace ZagIR;

Transpiler::Transpiler() {
  // We push the root scope
  PushScope();
  LoadPackage("_internal");

  currentFormat = 0;

  functionDefinition = "";
  functionDeclaration = "";
}

Transpiler::~Transpiler() {}

void Transpiler::ThrowError(Node *node, std::string what) {
  // Hauria de canviar això en el futur
  Logs::Error(what);
}

void Transpiler::PushScope() { environment.push_back(Scope()); }

void Transpiler::PopScope() { environment.pop_back(); }

void Transpiler::AddToRoot(std::string name, Object *obj) {
  if (!ExistsInRootScope(name))
    environment[0].data.insert(std::pair<std::string, Object *>(name, obj));
}

void Transpiler::AddToScope(std::string name, Object *obj) {
  if (!ExistsInScope(name))
    environment.back().data.insert(std::pair<std::string, Object *>(name, obj));
}

Object *Transpiler::FetchEnvironment(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data.find(key) != environment[i].data.end()) {
      return environment[i].data[key];
    }
  }
  return nullptr;
}

Object *Transpiler::FetchRootEnvironment(std::string key) {
  return environment[0].data[key];
}

ObjectType *Transpiler::FetchType(std::string key) {
  // TODO: Suport per Type<> i que sigui safe?
  return dynamic_cast<ObjectType *>(FetchRootEnvironment(key));
}

bool Transpiler::ExistsInEnv(std::string key) {
  for (int i = environment.size() - 1; i >= 0; i--) {
    if (environment[i].data[key] != nullptr) {
      return true;
    }
  }
  return false;
}

bool Transpiler::ExistsInScope(std::string key) {
  return environment.back().data[key] != nullptr;
}

bool Transpiler::ExistsInRootScope(std::string key) {
  return environment[0].data[key] != nullptr;
}

void Transpiler::AddPackageToScope(ZagIR::Package *package) {
  ObjectContainer *packContainer = new ObjectContainer();

  for (int i = 0; i < package->binds.size(); i++) {
    Binding *b = package->binds[i];
    AddBindingToScope(b);
  }
}

void Transpiler::AddBindingToScope(Binding *bind) {
  CType *ctype = dynamic_cast<CType *>(bind);
  CFunction *cfunction = dynamic_cast<CFunction *>(bind);
  Conversion *conversion = dynamic_cast<Conversion *>(bind);

  if (ctype != nullptr) {
    AddCTypeToScope(ctype);
  } else if (cfunction != nullptr) {
    AddCFunctionToScope(cfunction);
  } else if (conversion != nullptr) {
    AddConversionToScope(conversion);
  }
}

void Transpiler::AddCTypeToScope(CType *ctype) {
  ObjectCType *newType = new ObjectCType(ctype);
  AddToRoot(newType->identifier, newType);
}

void Transpiler::AddCFunctionToScope(CFunction *cfunction) {
  ObjectCFunction *newConversion = new ObjectCFunction(cfunction);
  AddToRoot(cfunction->name, newConversion);
}

void Transpiler::AddConversionToScope(Conversion *conversion) {
  ObjectConversion *newConversion = new ObjectConversion(conversion);
  AddToRoot(newConversion->identifier, newConversion);
}

std::string Transpiler::WriteFormat(std::string text) {
  formatList.insert(std::make_pair(currentFormat, text));
  std::string res = "{" + std::to_string(currentFormat) + "}";
  currentFormat++;
  return res;
}

std::string Transpiler::GenerateSource(Node *ast) {
  std::string main = "int main(){{";

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

std::string Transpiler::GenerateIncludes() {
  std::string res = "";
  for (int i = 0; i < includes.size(); i++) {
    if (includes[i] != "") {
      res += "#include <" + includes[i] + ">\n";
    }
  }
  return res;
}

std::string Transpiler::TranspileBlock(Node *space) {
  std::string content = "";
  for (int i = 0; i < space->children.size(); i++) {
    content += TranspileStatement(space->children[i]);
  }
  return content;
}

std::string Transpiler::TranspileStatement(Node *statement) {

  ObjectType *evalType;

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
    return TranspileExpression(statement, &evalType) + ";";
  default:
    std::cout << "Undefined node " << statement->type << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileIdentifier(Node *identifier,
                                            ObjectType **retType) {
  std::string idName = identifier->data;
  if (ExistsInEnv(idName)) {
    Object *obj = FetchEnvironment(idName);

    ObjectVariable *var = dynamic_cast<ObjectVariable *>(obj);
    if (var != nullptr) {
      *retType = var->GetType();
      return var->Transpile();
    } else {
      ThrowError(identifier, "Parsed identifier that is not variable");
    }

  } else {
    ThrowError(identifier, "Identifier" + idName + " does not exist");
  }
  return "";
}

std::string Transpiler::TranspileAssignation(Node *assignation) {
  std::string ogTypeStr = "", ogIdentifier;
  std::string TtypeStr = "", Texpression;

  ogIdentifier = assignation->children[0]->data;
  bool declaring = false;

  ObjectType *expType;
  Texpression = TranspileExpression(assignation->children[1], &expType);

  if (!ExistsInEnv(ogIdentifier)) {
    ObjectVariable *newVariable = new ObjectVariable(expType);
    newVariable->SetType(expType);

    declaring = true;
    if (assignation->arguments.size() > 0) {
      // Es typed
      Node *type = assignation->arguments[0];

      ogTypeStr = type->data;
      ObjectType *declType =
          dynamic_cast<ObjectType *>(FetchEnvironment(ogTypeStr));

      TtypeStr = declType->Transpile();
      newVariable->SetType(declType);

      // Aqui es podrien detectar ja dobles definicions??
      AddToScope(ogIdentifier, newVariable);
    } else {
      // "Any" type
      TtypeStr = expType->Transpile();
      AddToScope(ogIdentifier, newVariable);
    }
  }

  return TtypeStr + " " +
         dynamic_cast<ObjectVariable *>(FetchEnvironment(ogIdentifier))
             ->Transpile() +
         " " + assignation->data + " " + Texpression;
}

std::string Transpiler::TranspileExpression(Node *expression,
                                            ObjectType **retType) {
  switch (expression->type) {
    // Aquests casos estan hardcodejats per tal de transformar
    // els literals interns del llenguatge amb el paquet _internal
  case NODE_NUMBER_VAL:
    *retType = FetchType("Num");
    return expression->data;
    break;
  case NODE_STRING_VAL:
    *retType = FetchType("Str");
    return "std::string(\"" + expression->data + "\")";
    break;
  case NODE_YEP_VAL:
    *retType = FetchType("Bul");
    return "true";
    break;
  case NODE_NOP_VAL:
    *retType = FetchType("Bul");
    return "false";
    break;
  case NODE_OP_BIN:
    return TranspileBinary(expression, retType);
    break;
  case NODE_OP_UN:
    return TranspileUnary(expression, retType);
    break;
  case NODE_IDENTIFIER:
    return TranspileIdentifier(expression, retType);
    break;
  case NODE_CALL:
    return TranspileCall(expression, retType);
    break;
  case NODE_GETTER:
    return TranspileGetter(expression, retType);
    break;
  default:
    // std::cout << "Unexpected node:" << std::endl;
    break;
  }
  return "";
}

std::string Transpiler::TranspileBinary(Node *binary, ObjectType **retType) {
  // Ens hem d'assegurar que els tipus coincideixin
  ObjectType *lType, *rType;
  std::string lExp = TranspileExpression(binary->children[0], &lType);
  std::string rExp = TranspileExpression(binary->children[1], &rType);

  // Call conversion if necessary
  if (!lType->Equals(rType)) {
  }

  // Es poden treure espais un cop fet bé
  return "(" + lExp + " " + binary->data + " " + rExp + ")";
}

std::string Transpiler::TranspileUnary(ZagIR::Node *unary,
                                       ObjectType **retType) {
  return unary->data + TranspileExpression(unary->children[0], retType);
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

    // Hauria d'utilitzar això? XD
    ObjectType *expType;
    res += TranspileExpression(ifNode->arguments[i], &expType);

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
      ObjectVariable *newVar = new ObjectVariable(FetchType("Int"));
      AddToScope(iterator->arguments[i]->data, newVar);

      // TODO: Treure això
      ObjectType *t, *fType, *tType;
      std::string identifier = TranspileIdentifier(iterator->arguments[i], &t);

      if (interval->children.size() > 1) {
        // TODO: Podem fer check netament de si són Int!!!
        from = TranspileExpression(interval->children[0], &fType);
        to = TranspileExpression(interval->children[1], &tType);
      } else {
        to = TranspileExpression(interval->children[0], &fType);
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

      // Un altre cop això hauria d'avaluar a Bul
      ObjectType *expType;
      res += TranspileExpression(lup->arguments[1], &expType) + "; ";
      res += TranspileAssignation(lup->arguments[2]) + ")";
    } else {
      ObjectType *expType;
      res += "while(" + TranspileExpression(lup->arguments[0], &expType) + ")";
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
  Package *package;
  bool lib;
  // std::cout << "Value size: " << value.size() << std::endl;
  if (value[0] == '\'') {
    lib = true;
    realImport = value.substr(1, value.size() - 1);
    LoadPackage(realImport);
  } else {
    lib = false;
    realImport = value.substr(1, value.size() - 2);
    // Normal import
  }

  return "";
}

std::string Transpiler::TranspileFunction(ZagIR::Node *function) {

  ObjectNativeFunction *func = new ObjectNativeFunction();
  func->SetReturnType(FetchType("Nil"));
  // Sanitization?
  std::string funcName = "_f_" + function->data;
  // For the function declaration we have 2 arguments: A Type of return value
  // and a list of args for the function. ej:
  // returnType -> int
  // arguments -> (int a, int b)
  std::string returnType = "", arguments = "";
  std::string argPiled;
  std::string argType, argIdentifier;

  AddToScope(function->data, func);

  PushScope();
  for (int i = 0; i < function->arguments.size(); i++) {
    if (function->arguments[i]->type == ZagIR::NODE_TYPE) {

      ObjectType *retType = FetchType(function->arguments[i]->data);
      func->SetReturnType(retType);

    } else if (function->arguments[i]->type == ZagIR::NODE_ARGS) {
      Node *args = function->arguments[i];
      arguments = "(";
      for (int j = 0; j < args->children.size(); j++) {
        Node *arg = args->children[j];

        argIdentifier = arg->children[0]->data;

        ObjectType *argType = FetchType(arg->arguments[0]->data);
        ObjectVariable *argVar = new ObjectVariable(argType);

        AddToScope(argIdentifier, argVar);

        func->functionArgs.push_back(argType);

        argPiled = argType->Transpile() + " " + argVar->Transpile();
        arguments += argPiled;
        if (j < args->children.size() - 1)
          arguments += ",";
      }
      arguments += ")";
    } else {
      std::cout << "Bad formatted AST" << std::endl;
    }
  }

  returnType = func->GetReturnType()->Transpile();

  std::string defBlock = TranspileBlock(function->children[0]);
  PopScope();

  functionDeclaration += returnType + " " + funcName + arguments + ";";
  functionDefinition +=
      returnType + " " + funcName + arguments + "{{" + defBlock + "}}";

  return "";
}

std::string Transpiler::TranspileReturn(ZagIR::Node *ret) {
  // Aqui podriem comprovar si el tipus del return és vàlid
  ObjectType *returnType = FetchType("Nil");
  std::string exp = "";
  if (ret->children.size() > 0) {
    // Suport per més expressions?
    exp = TranspileExpression(ret->children[0], &returnType);
  }

  return "return " + exp + ";";
}

std::string Transpiler::TranspileCall(ZagIR::Node *call,
                                      ObjectType **returnType) {
  // TODO: Comprovar que la funció existeix lmao
  ObjectFunction *func =
      dynamic_cast<ObjectFunction *>(FetchEnvironment(call->data));
  std::string funcName = func->GetName();
  *returnType = func->GetReturnType();
  return TranspileGCall(func, call);
}

std::string Transpiler::TranspileGCall(ObjectFunction *func,
                                       ZagIR::Node *call) {
  std::string args = "";

  std::vector<ObjectType *> argTypes;
  for (int i = 0; i < call->arguments.size(); i++) {
    ObjectType *argType;

    args += TranspileExpression(call->arguments[i], &argType);
    argTypes.push_back(argType);

    if (i < call->arguments.size() - 1)
      args += ",";
  }

  if (!func->CheckArgs(argTypes)) {
    // ERROR
    Logs::Error("Invalid arguments");
    return "";
  }

  return func->GetName() + "(" + args + ")";
}

std::string Transpiler::TranspileGetter(ZagIR::Node *getter,
                                        ObjectType **returnType) {
  if (ExistsInEnv(getter->arguments[0]->data)) {
    Object *scoped = FetchEnvironment(getter->arguments[0]->data);
    // scoped->Print();
    ZagIR::Node *currentGetter = getter;

    ObjectContainer *scopedContainer = dynamic_cast<ObjectContainer *>(scoped);

    while ((scopedContainer != nullptr) && currentGetter->children.size() > 0) {
      currentGetter = currentGetter->children[0];
      if (!(currentGetter->type == ZagIR::NODE_GETTER ||
            currentGetter->type == ZagIR::NODE_CALL)) {
        // Ens hem quedat sense getters
        std::cout << "Not reached till the end" << std::endl;
      }

      if (currentGetter->type == ZagIR::NODE_CALL) {
        scoped = scopedContainer->GetObject(currentGetter->data);
      } else
        scoped = scopedContainer->GetObject(currentGetter->arguments[0]->data);
    }

    if (currentGetter->type == NODE_GETTER) {
      // Més getters que el que hi ha al objecte
      // Podria donar support a constants???
      std::cout << "Invalid" << std::endl;
    }

    // En principi estariem al final de la cadena de getters.
    // Podriem tenir una call o un identificador
    if (currentGetter->type == NODE_CALL) {
      // ZagIR::PackCall pack = scoped->GetCFunctionData();
      // Add fileDeps and return
      ObjectFunction* ofunc = dynamic_cast<ObjectFunction*>(scoped);
      if(ofunc != nullptr){
        return TranspileGCall(ofunc, currentGetter);
      } else {
        Logs::Error("Object is not a func (idk what happened)");
        return "";
      }
      
      /*
      for (int i = 0; i < pack.fileDeps.size(); i++) {
        fileDeps.insert(packageObject->GetPackage()->path + "/" +
                        pack.fileDeps[i]);
      }
      */

    }
    // Hem de carregar fileDeps
  }
  Logs::Error("Getter not cached");
  return "";
}

ZagIR::Package *Transpiler::LoadPackage(std::string packageName) {
  Package *package = ZagIR::FetchPackage(packageName);
  loadedPackages.push_back(package);
  AddPackageToScope(package);
  return package;
}
