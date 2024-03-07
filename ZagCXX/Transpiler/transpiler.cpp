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

std::string Transpiler::GenerateSource(Node *ast, std::string *cxxargs) {
  std::string main = "int main(){{";

  env->PushScope();
  std::string mainFunc = TranspileBlock(ast);

  env->DumpEnvironment();
  env->PopScope();

  env->PopScope();

  std::string preFormat = env->GetIncludes() + functionDeclaration + main +
                          mainFunc + "}}" + functionDefinition;
  *cxxargs = env->GetCXXArgs();
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
  env->DumpEnvironment();
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

std::string Transpiler::TranspileAssignation(Node *assignation) {

  std::string ogTypeStr = "", ogIdentifier;
  std::string TtypeStr = "", Texpression;

  ogIdentifier = assignation->children[0]->data;
  bool declaring = false;

  ObjectType *expType;
  Texpression = TranspileExpression(assignation->children[1], &expType);

  if (!env->Exists(ogIdentifier)) {
    ObjectVariable *newVariable = new ObjectVariable(expType, ogIdentifier);
    newVariable->SetType(expType);

    declaring = true;
    if (assignation->arguments.size() > 0) {
      // Es typed
      Node *type = assignation->arguments[0];

      ogTypeStr = type->data;
      ObjectType *declType = dynamic_cast<ObjectType *>(env->Fetch(ogTypeStr));

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
         dynamic_cast<ObjectVariable *>(env->Fetch(ogIdentifier))->Transpile() +
         " " + assignation->data + " " + Texpression;
}

std::string Transpiler::TranspileExpression(Node *expression,
                                            ObjectType **retType) {
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
  ObjectType *lType, *rType, *returnType;
  std::string lExp = TranspileExpression(binary->children[0], &lType);
  std::string rExp = TranspileExpression(binary->children[1], &rType);

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

    env->PushScope();
    res += TranspileBlock(ifNode->children[i]);
    env->PopScope();

    res += "}}";
  }
  if (ifNode->children.size() > ifNode->arguments.size()) {
    res += "else {{";

    env->PushScope();
    res += TranspileBlock(ifNode->children[i]);
    env->PopScope();

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
        std::cout << "ADDED TO SCEOPE " << iterator->arguments[i]->data << std::endl;

        // TODO: Treure això
        ObjectType *t, *fType, *tType;
        std::string identifier =
            TranspileIdentifier(iterator->arguments[i], &t);

        if (interval->children.size() > 1) {
          // TODO: Podem fer check netament de si són Int!!!
          from = TranspileExpression(interval->children[0], &fType);
          to = TranspileExpression(interval->children[1], &tType);
        } else {
          to = TranspileExpression(interval->children[0], &fType);
        }

        res += "for(int " + identifier + " = ";
        res += from + "; " + identifier + " <= " + to + "; " + identifier +
               "++){{";
      }
      // std::cout << iterator->arguments.size() << std::endl;
std::cout << "SHOULD EXISTS" << std::endl;
env->DumpEnvironment();
      block = TranspileBlock(lup->children[0]);
      env->PopScope();
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
        std::string exp = TranspileExpression(lup->arguments[0], &expType);

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
  res += "{{" + block + "}}";

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

    Package *loadedPackage = GetLoadedPackage(packageName);
    if (loadedPackage == nullptr) {
      loadedPackage = LoadPackage(packageName);
      Logs::Debug("Loaded package " + packageName);
    }

    if (subpackage) {
      loadedPackage->LoadSubPackage(subpackageName);
      LoadSubPackage(loadedPackage, subpackageName);
      Logs::Debug("Loaded subpackage " + subpackageName);
    }

    loadedPackage->ComputeBinds();
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
  std::cout << "Pushed function" << std::endl;
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

        func->functionArgs.push_back(argType); // TODO: Canviar

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

  std::cout << "Should remain" << std::endl;
  std::string defBlock = TranspileBlock(function->children[0]);
  env->PopScope();

  functionDeclaration += returnType + " " + funcName + arguments + ";";
  functionDefinition +=
      returnType + " " + funcName + arguments + "{{" + defBlock + "}}";

  return "";
}

std::string Transpiler::TranspileReturn(ZagIR::Node *ret) {
  // Aqui podriem comprovar si el tipus del return és vàlid
  ObjectType *returnType = env->FetchType("Nil");
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
  ObjectFunction *func = dynamic_cast<ObjectFunction *>(env->Fetch(call->data));
  return TranspileGCall(func, call, returnType);
}

std::string Transpiler::TranspileGCall(ObjectFunction *func, ZagIR::Node *call,
                                       ObjectType **returnType) {
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

  ObjectCFunction *cFunc = dynamic_cast<ObjectCFunction *>(func);
  if (cFunc != nullptr) {
    cFunc->Use(env);
  }

  *returnType = env->FetchType(func->returnType);
  return func->GetName() + "(" + args + ")";
}

std::string Transpiler::TranspileGetter(ZagIR::Node *getter,
                                        ObjectType **returnType) {
  if (env->Exists(getter->arguments[0]->data)) {
    Object *scoped = env->Fetch(getter->arguments[0]->data);
    // scoped->Print();
    ZagIR::Node *currentGetter = getter;

    ObjectContainer *scopedContainer = dynamic_cast<ObjectContainer *>(scoped);

    while ((scopedContainer != nullptr) && currentGetter->children.size() > 0) {

      scoped->Print(0);
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
      scopedContainer = dynamic_cast<ObjectContainer *>(scoped);
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
      ObjectFunction *ofunc = dynamic_cast<ObjectFunction *>(scoped);
      if (ofunc != nullptr) {
        return TranspileGCall(ofunc, currentGetter, returnType);
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
  env->AddPackageToScope(package);
  return package;
}

void Transpiler::LoadSubPackage(ZagIR::Package *package,
                                std::string subpackage) {
  env->AddSubPackageToScope(package, subpackage);
}

ZagIR::Package *Transpiler::GetLoadedPackage(std::string packageName) {
  for (int i = 0; i < loadedPackages.size(); i++) {
    if (loadedPackages[i]->name == packageName)
      return loadedPackages[i];
  }
  return nullptr;
}
