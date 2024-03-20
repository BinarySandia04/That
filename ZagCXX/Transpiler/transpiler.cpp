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
#include <stdexcept>

using namespace ZagCXX;
using namespace ZagIR;

Transpiler::Transpiler() {
  // We push the root scope
  env = new Environment();
  LoadPackage("_internal");

  currentFormat = 0;

  functionDefinition = "";
  functionDeclaration = "";
}

Transpiler::~Transpiler() {
  for (int i = 0; i < loadedPackages.size(); i++)
    delete loadedPackages[i];
  delete env;
}

void Transpiler::ThrowError(Node *node, std::string what) {
  // Hauria de canviar això en el futur
  Logs::Error(what);
}

std::string Transpiler::GenerateSource(Node *ast, std::string *cxxargs,
                                       std::vector<std::string> *libNames) {
  std::string main = "int main(){";

  std::string mainFunc = TranspileBlock(ast);

  std::string preFormat = env->GetIncludes() + functionDeclaration + main +
                          mainFunc + "}" + functionDefinition;
  *cxxargs = env->GetCXXArgs();
  env->GetLibNames(libNames);

  return preFormat;
}

std::string Transpiler::GenerateIncludes() {
  std::string res = "";
  for (int i = 0; i < includes.size(); i++) {
    if (includes[i] != "") {
      res += "#include " + includes[i] + "\n";
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
  std::string before = "";
  std::string exp;

  switch (statement->type) {
  case ZagIR::NODE_ASSIGNATION:
    exp = TranspileAssignation(statement, &before) + ";";
    break;
  case ZagIR::NODE_IF:
    exp = TranspileIf(statement, &before);
    break;
  case ZagIR::NODE_LUP:
    exp = TranspileLup(statement, &before);
    break;
  case ZagIR::NODE_GET:
    exp = TranspileGet(statement);
    break;
  case ZagIR::NODE_FUNCTION:
    exp = TranspileFunction(statement);
    break;
  case ZagIR::NODE_RET:
    exp = TranspileReturn(statement, &before);
    break;
  case ZagIR::NODE_EXPRESSION:
  case ZagIR::NODE_ARRAY:
  case ZagIR::NODE_OP_BIN:
  case ZagIR::NODE_OP_UN:
  case ZagIR::NODE_IDENTIFIER:
  case ZagIR::NODE_YEP_VAL:
  case ZagIR::NODE_NOP_VAL:
  case ZagIR::NODE_NUMBER_VAL:
  case ZagIR::NODE_STRING_VAL:
  case ZagIR::NODE_CALL:
  case ZagIR::NODE_GETTER:
    exp = TranspileExpression(statement, &evalType, &before) + ";";
    break;
  default:
    std::cout << "Undefined node " << statement->type << std::endl;
    break;
  }

  return before + exp;
}

std::string Transpiler::TranspileIdentifier(Node *identifier,
                                            ObjectType **retType) {
  std::string idName = identifier->data;
  if (env->Exists(idName)) {
    Object *obj = env->Fetch(idName);

    ObjectVariable *var = dynamic_cast<ObjectVariable *>(obj);
    if (var != nullptr) {
      *retType = var->GetType();
      return var->Transpile();
    } else {
      ThrowError(identifier, "Parsed identifier that is not variable");
    }

  } else {
    ThrowError(identifier, "Identifier " + idName + " does not exist");
  }
  return "";
}

std::string Transpiler::TranspileAssignation(Node *assignation,
                                             std::string *before) {

  std::string ogTypeStr = "", ogIdentifier;
  std::string TtypeStr = "", Texpression;

  ObjectType *expType;
  Texpression = TranspileExpression(assignation->children[1], &expType, before);

  if (assignation->children[0]->type == ZagIR::NODE_IDENTIFIER) {
    ogIdentifier = assignation->children[0]->data;

    if (!env->Exists(ogIdentifier)) {
      ObjectVariable *newVariable = new ObjectVariable(expType, ogIdentifier);
      newVariable->SetType(expType);

      if (assignation->arguments.size() > 0) {
        // Es typed
        Node *type = assignation->arguments[0];

        ogTypeStr = type->data;
        // Logs::Debug("Fetched type " + ogTypeStr);
        ObjectType *declType = dynamic_cast<ObjectType *>(env->FetchType(type));

        if (declType == nullptr) {
          std::cout << "NULLPTR! not found " << ogTypeStr << std::endl;
        }
        TtypeStr = declType->Transpile();
        newVariable->SetType(declType);

        // Aqui es podrien detectar ja dobles definicions??
        env->AddToScope(ogIdentifier, newVariable);
      } else {
        // "Any" type
        TtypeStr = expType->Transpile();
        env->AddToScope(ogIdentifier, newVariable);
      }
    }

    return TtypeStr + " " +
           dynamic_cast<ObjectVariable *>(env->Fetch(ogIdentifier))
               ->Transpile() +
           " " + assignation->data + " " + Texpression;
  } else if (assignation->children[0]->type == ZagIR::NODE_ACCESSOR) {
    // We are assigning an accessor
    // Podem tenir part de getters de moment ho ometem
    Node *accessor = assignation->children[0];
    ObjectType *accessorType;
    std::string accessorStr =
        TranspileAccessor(accessor, &accessorType, before);
    return accessorStr + " = " + Texpression;
  }
}

std::string Transpiler::TranspileExpression(Node *expression,
                                            ObjectType **retType,
                                            std::string *before) {
  switch (expression->type) {
    // Aquests casos estan hardcodejats per tal de transformar
    // els literals interns del llenguatge amb el paquet _internal
  case NODE_INT_VAL:
    *retType = env->FetchType("Int");
    return expression->data;
    break;
  case NODE_NUMBER_VAL:
    *retType = env->FetchType("Num");
    return expression->data;
    break;
  case NODE_STRING_VAL:
    *retType = env->FetchType("Str");
    return "std::string(\"" + expression->data + "\")";
    break;
  case NODE_YEP_VAL:
    *retType = env->FetchType("Bul");
    return "true";
    break;
  case NODE_NOP_VAL:
    *retType = env->FetchType("Bul");
    return "false";
    break;
  case NODE_OP_BIN:
    return TranspileBinary(expression, retType, before);
    break;
  case NODE_OP_UN:
    return TranspileUnary(expression, retType, before);
    break;
  case NODE_IDENTIFIER:
    return TranspileIdentifier(expression, retType);
    break;
  case NODE_CALL:
    return TranspileCall(expression, retType, before);
    break;
  case NODE_GETTER:
    return TranspileGetter(expression, retType, before);
    break;
  case NODE_ARRAY:
    return TranspileArray(expression, retType, before);
    break;
  case NODE_ACCESSOR:
    return TranspileAccessor(expression, retType, before);
    break;
  default:
    expression->Debug(0);
    break;
  }
  return "";
}

std::string Transpiler::TranspileBinary(Node *binary, ObjectType **retType,
                                        std::string *before) {
  // Ens hem d'assegurar que els tipus coincideixin
  ObjectType *lType, *rType, *returnType;
  std::string lExp = TranspileExpression(binary->children[0], &lType, before);
  std::string rExp = TranspileExpression(binary->children[1], &rType, before);

  bool typed = false;
  if (binary->data == "==" || binary->data == ">=" || binary->data == "<=" ||
      binary->data == "!=" || binary->data == "&&" || binary->data == "||" ||
      binary->data == ">" || binary->data == "<") {

    returnType = env->FetchType("Bul");
    *retType = returnType;

    typed = true;
  }

  // Call conversion if necessary
  if (!lType->Equals(rType)) {
    // Veiem si un dels dos es pot millorar
    ObjectType *upgrade;
    if (lType->upgrades_to != "") {
      upgrade = dynamic_cast<ObjectType *>(env->FetchType(lType->upgrades_to));
      if (upgrade->Equals(rType)) {
        if (!typed) {
          typed = true;
          *retType = upgrade;
        }
      }
    } else if (rType->upgrades_to != "") {
      upgrade = dynamic_cast<ObjectType *>(env->FetchType(rType->upgrades_to));
      if (upgrade->Equals(lType)) {
        if (!typed) {
          typed = true;
          *retType = upgrade;
        }
      }
    }

    if (!typed) {
      // Hem de veure si hi ha una operació de tipus
      // I si no n'hi ha hem de fer un error
      ObjectCOperation *operation = env->FetchOperation(lType, rType);

      if (operation != nullptr) {
        operation->Use(env);
        *retType = env->FetchType(operation->cOperationData->resType);
        return operation->GetName() + "(" + lExp + ", " + rExp + ")";
      } else {
        // No hi ha conversió
      }
    }
  } else {
    // typed = true;
    if (!typed) {
      typed = true;
      *retType = lType;
    }
  }

  // Es poden treure espais un cop fet bé

  return "(" + lExp + " " + binary->data + " " + rExp + ")";
}

std::string Transpiler::TranspileUnary(ZagIR::Node *unary, ObjectType **retType,
                                       std::string *before) {
  return unary->data + TranspileExpression(unary->children[0], retType, before);
}

std::string Transpiler::TranspileIf(ZagIR::Node *ifNode, std::string *before) {
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
    res += TranspileExpression(ifNode->arguments[i], &expType, before);

    res += "){";

    env->PushScope();
    res += TranspileBlock(ifNode->children[i]);
    env->PopScope();

    res += "}";
  }
  if (ifNode->children.size() > ifNode->arguments.size()) {
    res += "else {";

    env->PushScope();
    res += TranspileBlock(ifNode->children[i]);
    env->PopScope();

    res += "}";
  }
  return res;
}

std::string Transpiler::TranspileLup(ZagIR::Node *lup, std::string *before) {
  // data has identifier. Child is block. Three forms:
  // Args: LUP_ITERATORS
  // Args: ASSIGNATION, EXPRESSION, ASSIGNATION
  // Args: EXPRESSION
  std::string res = "";

  std::string block;
  env->PushScope();

  if (lup->arguments.size() > 0) {
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
        ObjectVariable *newVar = new ObjectVariable(
            env->FetchType("Int"), iterator->arguments[i]->data);
        env->AddToScope(iterator->arguments[i]->data, newVar);

        // TODO: Treure això
        ObjectType *t, *fType, *tType;
        std::string identifier =
            TranspileIdentifier(iterator->arguments[i], &t);

        if (interval->children.size() > 1) {
          // TODO: Podem fer check netament de si són Int!!!
          from = TranspileExpression(interval->children[0], &fType, before);
          to = TranspileExpression(interval->children[1], &tType, before);
        } else {
          to = TranspileExpression(interval->children[0], &fType, before);
        }

        res += "for(int " + identifier + " = ";
        res +=
            from + "; " + identifier + " < " + to + "; " + identifier + "++)";
      }
      // std::cout << iterator->arguments.size() << std::endl;
    } else {
      // Simple while structure
      if (lup->arguments.size() > 1) {
        res += "for(" + TranspileAssignation(lup->arguments[0], before) + "; ";

        // Un altre cop això hauria d'avaluar a Bul
        ObjectType *expType;
        res += TranspileExpression(lup->arguments[1], &expType, before) + "; ";
        res += TranspileAssignation(lup->arguments[2], before) + ")";
      } else {
        ObjectType *expType;
        std::string exp =
            TranspileExpression(lup->arguments[0], &expType, before);

        if (expType->Equals(env->FetchType("Int"))) {
          res += "for(int i = 0; i < " + exp + "; i++)";
        } else {
          res += "while(" + exp + ")";
        }
      }
    }
  } else {
    // Infinite lup
    res += "for(;;)";
  }
  block = TranspileBlock(lup->children[0]);

  env->PopScope();
  res += "{" + block + "}";

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

    /*
    std::string packageName = "", subpackageName = "";
    bool subpackage = false;
    for (int i = 0; i < realImport.size(); i++) {
      if (realImport[i] == '.' && !subpackage) {
        subpackage = true;
        continue;
      }
      if (!subpackage)
        packageName += realImport[i];
      else
        subpackageName += realImport[i];
    }
    */

    Package *loadedPackage = GetLoadedPackage(realImport);
    if (loadedPackage == nullptr) {
      loadedPackage = LoadPackage(realImport);
    }

    /*
    if (subpackage) {
      loadedPackage->LoadSubPackage(subpackageName);
      LoadSubPackage(loadedPackage, subpackageName);
    }
    */

  } else {
    lib = false;
    realImport = value.substr(1, value.size() - 2);
    // Normal import
  }

  return "";
}

std::string Transpiler::TranspileFunction(ZagIR::Node *function) {

  ObjectNativeFunction *func = new ObjectNativeFunction();
  func->returnType = "Nil"; // TODO: Canviar
  // Sanitization?
  std::string funcName = "_f_" + function->data;
  // For the function declaration we have 2 arguments: A Type of return value
  // and a list of args for the function. ej:
  // returnType -> int
  // arguments -> (int a, int b)
  std::string returnType = "", arguments = "";
  std::string argPiled;
  std::string argType, argIdentifier;

  func->identifier = function->data;
  env->AddToScope(function->data, func);

  env->PushScope();
  for (int i = 0; i < function->arguments.size(); i++) {
    if (function->arguments[i]->type == ZagIR::NODE_TYPE) {

      ObjectType *retType = env->FetchType(function->arguments[i]->data);
      func->returnType = retType->identifier;

    } else if (function->arguments[i]->type == ZagIR::NODE_ARGS) {
      Node *args = function->arguments[i];
      arguments = "(";
      for (int j = 0; j < args->children.size(); j++) {
        Node *arg = args->children[j];

        argIdentifier = arg->children[0]->data;

        ObjectType *argType = env->FetchType(arg->arguments[0]->data);
        ObjectVariable *argVar = new ObjectVariable(argType, argIdentifier);

        env->AddToScope(argIdentifier, argVar);

        func->functionArgs.push_back(argType->identifier); // TODO: Canviar

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

  returnType = env->FetchType(func->returnType)->Transpile();

  std::string defBlock = TranspileBlock(function->children[0]);
  env->PopScope();

  functionDeclaration += returnType + " " + funcName + arguments + ";";
  functionDefinition +=
      returnType + " " + funcName + arguments + "{" + defBlock + "}";

  return "";
}

std::string Transpiler::TranspileReturn(ZagIR::Node *ret, std::string *before) {
  // Aqui podriem comprovar si el tipus del return és vàlid
  ObjectType *returnType = env->FetchType("Nil");
  std::string exp = "";
  if (ret->children.size() > 0) {
    // Suport per més expressions?
    exp = TranspileExpression(ret->children[0], &returnType, before);
  }

  return "return " + exp + ";";
}

std::string Transpiler::TranspileCall(ZagIR::Node *call,
                                      ObjectType **returnType,
                                      std::string *before) {
  // TODO: Comprovar que la funció existeix lmao
  ObjectFunction *func = dynamic_cast<ObjectFunction *>(env->Fetch(call->data));
  return TranspileGCall(func, call, returnType, before);
}

std::string Transpiler::TranspileGCall(ObjectFunction *func, ZagIR::Node *call,
                                       ObjectType **returnType,
                                       std::string *before) {
  std::string args = "";

  std::vector<ObjectType *> argTypes;
  for (int i = 0; i < call->arguments.size(); i++) {
    ObjectType *argType;

    args += TranspileExpression(call->arguments[i], &argType, before);
    argTypes.push_back(argType);

    if (i < call->arguments.size() - 1)
      args += ",";
  }

  if (!func->CheckArgs(argTypes, env)) {
    // ERROR
    Logs::Error("Invalid arguments");
    return "";
  }

  ObjectCFunction *cFunc = dynamic_cast<ObjectCFunction *>(func);
  if (cFunc != nullptr) {
    cFunc->Use(env);
  }

  *returnType = env->FetchType(func->returnType);
  return func->GetName() + "(" + args + ")";
}

std::string Transpiler::TranspileGetter(ZagIR::Node *getter,
                                        ObjectType **returnType,
                                        std::string *before) {
  if (env->Exists(getter->arguments[0]->data)) {
    Object *scoped = env->Fetch(getter->arguments[0]->data);
    // scoped->Print();
    ZagIR::Node *currentGetter = getter;

    ObjectContainer *scopedContainer = dynamic_cast<ObjectContainer *>(scoped);
    ObjectVariable *scopedVariable = dynamic_cast<ObjectVariable *>(scoped);

    if (scopedContainer != nullptr) {
      scoped = NavigateContainer(&currentGetter, scopedContainer);

      // En principi estariem al final de la cadena de getters.
      // Podriem tenir una call o un identificador
      if (currentGetter->type == NODE_CALL) {
        // ZagIR::PackCall pack = scoped->GetCFunctionData();
        // Add fileDeps and return
        ObjectFunction *ofunc = dynamic_cast<ObjectFunction *>(scoped);
        if (ofunc != nullptr) {
          return TranspileGCall(ofunc, currentGetter, returnType, before);
        } else {
          Logs::Error("Object is not a func (idk what happened)");
          return "";
        }
      }
      // Hem de carregar fileDeps
    } else if (scopedVariable != nullptr) {
      scopedContainer = scopedVariable->GetType()->constructor->typeMethods;
      scoped = NavigateContainer(&currentGetter, scopedContainer);

      if (currentGetter->type == NODE_CALL) {
        ObjectFunction *ofunc = dynamic_cast<ObjectFunction *>(scoped);
        if (ofunc != nullptr) {
          ofunc->SetInheritedType(scopedVariable->GetType());
          return scopedVariable->Transpile() + "." + TranspileGCall(ofunc, currentGetter, returnType, before);
        } else {
          // A part, afegir mètodes aquí també
          Logs::Error("Object is not a func (idk what happened)");
          return "";
        }
      }
    }
  }
  Logs::Error("Getter not cached");
  return "";
}

Object *Transpiler::NavigateContainer(Node **getter,
                                      ObjectContainer *scopedContainer) {

  Object *scoped;
  while ((scopedContainer != nullptr) && (*getter)->children.size() > 0) {

    *getter = (*getter)->children[0];
    // (*getter)->Debug(0);
    if (!((*getter)->type == ZagIR::NODE_GETTER ||
          (*getter)->type == ZagIR::NODE_CALL)) {
      // Ens hem quedat sense getters
      std::cout << "Not reached till the end" << std::endl;
    }

    if ((*getter)->type == ZagIR::NODE_CALL)
      scoped = scopedContainer->GetObject((*getter)->data);
    else
      scoped = scopedContainer->GetObject((*getter)->arguments[0]->data);

    scopedContainer = dynamic_cast<ObjectContainer *>(scoped);
  }

  if ((*getter)->type == NODE_GETTER) {
    // Més getters que el que hi ha al objecte
    // Podria donar support a constants???
    std::cout << "Invalid" << std::endl;
    throw std::logic_error("Error invalid getter");
  }

  return scoped;
}

std::string Transpiler::TranspileArray(ZagIR::Node *array,
                                       ObjectType **returnType,
                                       std::string *before) {
  std::string result = "";
  std::vector<ObjectType *> argTypes;

  result += "{";
  for (int i = 0; i < array->children.size(); i++) {
    ObjectType *elementType;
    std::string exp =
        TranspileExpression(array->children[i], &elementType, before);
    argTypes.push_back(elementType);
    result += exp;
    if (i < array->children.size() - 1)
      result += ",";
  }
  result += "}";

  // std::cout << result << std::endl;

  bool equals = true;
  // Polimorfismo?
  ObjectType *firstType;
  if (argTypes.size() == 0) {
    firstType = env->FetchType("Any");
  } else {
    firstType = argTypes[0];
    for (int i = 1; i < argTypes.size(); i++) {
      if (!argTypes[i]->Equals(firstType)) {
        equals = false;
        break;
      }
    }

    if (!equals) {
      firstType = env->FetchType("Any");
    }
  }

  *returnType = env->FetchProtoType("Arr")->Construct({firstType}, env);

  return result;
}

std::string Transpiler::TranspileAccessor(Node *accessor,
                                          ObjectType **returnType,
                                          std::string *before) {
  // Uhh això pot ser un array!
  ObjectType *containerType, *indexType;
  std::string content =
      TranspileExpression(accessor->children[0], &containerType, before);
  std::string index =
      TranspileExpression(accessor->arguments[0], &indexType, before);

  // std::cout << "LeftType: " << containerType->Transpile() << " IndexType: "
  // << indexType->Transpile() << std::endl;
  if (containerType->constructor == env->FetchProtoType("Arr")) {
    // std::cout << "Is from array" << std::endl;
    *returnType = containerType->children[0];
  }
  return content + "[" + index + "]";
}

ZagIR::Package *Transpiler::LoadPackage(std::string packageName) {
  Package *package = ZagIR::FetchPackage(packageName);
  loadedPackages.push_back(package);
  env->AddPackageToScope(package);
  return package;
}

ZagIR::Package *Transpiler::GetLoadedPackage(std::string packageName) {
  for (int i = 0; i < loadedPackages.size(); i++) {
    if (loadedPackages[i]->name == packageName)
      return loadedPackages[i];
  }
  return nullptr;
}
