#include "transpiler.h"
#include "Objects/container.h"
#include "environment.h"

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

  std::string preFormat = env->GetIncludes() + functionDeclaration + "\n" +
                          classDeclaration + "\n" + classDefinition + "\n" +
                          main + mainFunc + "}" + functionDefinition + "\n";

  // env->DumpEnvironment();
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

  ObjectNativeFunction *func;

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
    func =
        TranspileFunction(statement, &functionDeclaration, &functionDefinition);
    env->AddToScope(statement->data, func);
    exp = "";
    break;
  case ZagIR::NODE_RET:
    exp = TranspileReturn(statement, &before);
    break;
  case ZagIR::NODE_BRK:
    exp = TranspileBrk(statement, &before);
    break;
  case ZagIR::NODE_KIN:
    exp = TranspileKin(statement, &before);
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
  case ZagIR::NODE_ACCESSOR:

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
    } 

    if(obj != nullptr){
      return idName;
    }
    
    std::cout << idName << std::endl;
    ThrowError(identifier, "Parsed identifier that is not variable");
    

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

  if (assignation->children.size() > 1) {
    Texpression =
        TranspileExpression(assignation->children[1], &expType, before);
  }

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

        // std::cout << ogTypeStr << std::endl;
        // type->Debug(0);
        // env->DumpEnvironment();
        env->FetchType(type);

        ObjectType *declType =
            dynamic_cast<ObjectType *>(env->FetchType(type)); // TODO: Aixo peta
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
  }

  bool firstAccessor = true;
  Node *currentNode = assignation->children[0];
  // assignation->Debug(0);

  std::string accessorTranspiled = "";
  while (currentNode->type != NODE_IDENTIFIER) {
    if (currentNode->type == NODE_GETTER) {

      if (firstAccessor) {
        accessorTranspiled += dynamic_cast<ObjectVariable *>(
                                  env->Fetch(currentNode->arguments[0]->data))
                                  ->Transpile();
        firstAccessor = false;
      } else
        accessorTranspiled += currentNode->arguments[0]->data;
      accessorTranspiled += ".";

      currentNode = currentNode->children[0];
      // currentNode->Debug(0);
    } else {
      std::cout << "NO! Type is " << currentNode->type;
    }
  }
  return accessorTranspiled + currentNode->data + " " + assignation->data +
         " " + Texpression;
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
  case NODE_GETTER:
  case NODE_ACCESSOR:
    ObjectContainer* c;
    std::cout << "-------------------------------------------------------" << std::endl;
    return TranspileInstruction(expression, retType, &c, env->GetTopScope(), before);
    // return TranspileGetter(expression, retType, before);
    break;
  case NODE_ARRAY:
    return TranspileArray(expression, retType, before);
    break;
  default:
    // expression->Debug(0);
    std::cout << termcolor::red << "Err?" << termcolor::reset << std::endl;
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
        // operation->Use(env);
        env->Use(operation);
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

  // Implement label
  res += "{" + block + "}\n";
  if (lup->data != "") {
    res += lup->data + ":\n";
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

    Package *loadedPackage = GetLoadedPackage(realImport);
    if (loadedPackage == nullptr) {
      loadedPackage = LoadPackage(realImport);
    }

  } else {
    lib = false;
    realImport = value.substr(1, value.size() - 2);
    // Normal import
  }

  return "";
}

ObjectNativeFunction *
Transpiler::TranspileFunction(ZagIR::Node *function,
                              std::string *functionDeclaration,
                              std::string *functionDefinition) {

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

  // TODO: Fer retornar per lo de les classes
  // env->AddToScope(function->data, func);

  env->PushScope();
  if (function->arguments.size() == 0) {
    arguments = "()";
  }
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

  if (functionDeclaration != nullptr)
    *functionDeclaration += returnType + " " + funcName + arguments + ";";
  if (functionDefinition != nullptr)
    *functionDefinition +=
        returnType + " " + funcName + arguments + "{" + defBlock + "}";

  return func;
}

std::string Transpiler::TranspileKin(Node *kin, std::string *before) {
  // kin->Debug(0);
  std::string kinName = kin->data;

  classDeclaration += "class c_" + kinName + ";\n";

  ObjectContainer *kinDef = new ObjectContainer();

  std::string privatePart = "", publicPart = "";

  classDefinition += "class c_" + kinName + " {\n";

  for (int i = 0; i < kin->children.size(); i++) {
    Object *addedObj;

    Privacy p = PUBLIC;
    if (kin->children[i]->data.size() > 0)
      switch (kin->children[i]->data[0]) {
      case '$':
        p = PRIVATE;
        break;
      case '@':
        p = STATIC;
        break;
      default:
        std::cout << "Undefined accessor?" << std::endl;
        return "";
      }
    std::string methodName;
    std::string *writeTo;
    if (p == PUBLIC)
      writeTo = &publicPart;
    else
      writeTo = &privatePart;

    methodName = TranspileMethod(kin->children[i], writeTo, &addedObj, before);
    if (methodName == "")
      return "";

    kinDef->AddObject(methodName, addedObj, p);
  }

  classDefinition +=
      "public:\n" + publicPart + "\nprivate:\n" + privatePart + "\n};\n";

  // TODO: Canviar a root?
  env->AddToRoot(kinName, new ObjectProtoType(kinDef, kinName));
  return "";
}

std::string Transpiler::TranspileMethod(Node *method, std::string *writeTo,
                                        Object **methodObject,
                                        std::string *before) {
  std::string methodName;

  Node *attribute = method->children[0];

  ObjectType *transpileType;

  switch (attribute->type) {
  case ZagIR::NODE_ASSIGNATION:
    transpileType = env->FetchType(attribute->arguments[0]);
    *methodObject =
        new ObjectVariable(transpileType, attribute->children[0]->data);
    methodName = attribute->children[0]->data;

    *writeTo += transpileType->Transpile() + " " + methodName + ";\n";
    break;
  case ZagIR::NODE_FUNCTION:
    methodName = attribute->data;
    *methodObject = TranspileFunction(attribute, nullptr, writeTo);
    *writeTo += ";\n";
    break;
  case ZagIR::NODE_IDENTIFIER:
    // Igual que assignation pero amb any
    transpileType = env->FetchType("Any");
    *methodObject = new ObjectVariable(transpileType, attribute->data);
    methodName = attribute->data;

    *writeTo += transpileType->Transpile() + " " + methodName + ";\n";
    break;
  default:
    // std::cout << "Method has no name?" << std::endl;
    *methodObject = new ObjectContainer();
    return "_";
  }
  return methodName;
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

std::string Transpiler::TranspileBrk(Node *brk, std::string *before) {
  // TODO
  std::string brkBody;

  if (brk->data == "")
    brkBody = "break;";
  else
    brkBody = "goto " + brk->data + ";";

  if (brk->children.size() > 0) {
    ObjectType *type; // Hauriem de veure que avalua a bool?
    return "if(" + TranspileExpression(brk->children[0], &type, before) + ") " +
           brkBody;
  }

  return brkBody;
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
    env->Use(cFunc);
  }

  *returnType = env->FetchType(func->returnType);
  return func->GetName() + "(" + args + ")";
}

std::string Transpiler::TranspileInstruction(Node *instruction,
                                             ObjectType **returnType,
                                             ObjectContainer** scope,
                                             Scope* root,
                                             std::string *before) {
  std::string res = "";
  std::cout << "Debugging:" << std::endl;
  instruction->Debug(0);
  root->data->Print(0);
  std::cout << "------" << std::endl;
  ObjectType *callRetType = nullptr; // ???


  if (instruction->type == NODE_GETTER) {
    std::string getterName = instruction->arguments[0]->data;

    ObjectContainer* recievedContainer;
    res += TranspileInstruction(instruction->children[0], returnType, &recievedContainer, root, before);
    res += "." + getterName; // Translate?
                             //
    std::cout << "RES: " << res << std::endl;

    if(recievedContainer->Exists(getterName)){
      Object* gettedObject = recievedContainer->GetObject(getterName); 
      // Pot ser un container o no. Si no ho és, agafem el tipus que tenim

      // Sabem que no es nullptr perque existeix
      ObjectContainer* gettedContainer = dynamic_cast<ObjectContainer*>(gettedObject);
      if(gettedContainer == nullptr){
        gettedContainer = (*returnType)->constructor->typeMethods;
      }

      *scope = gettedContainer;
    } else {
      std::cout << termcolor::red << "Invalid getter" << termcolor::reset << std::endl;
      return res;
    }
  } else if (instruction->type == NODE_CALL) {
    // Funció dins de qualsevol cosa
    // té un children que es getter i te el nom de la func i el children del children és el seguent

    /*
      // En cas de tenir variable actualitzem els tempaltes a la funció
      std::vector<std::string> oldParams;
      if(scopedVariable != nullptr){
        oldParams = ofunc->functionArgs;
        ofunc->SetInheritedType(scopedVariable->GetType());
      }

      res += TranspileGCall(ofunc, currentGetter, returnType, before);

      // Desfem els canvis
      if(scopedVariable != nullptr) ofunc->functionArgs = oldParams;
    */
    ObjectContainer *recievedContainer;

    root->data->Print(0);
    std::cout << "Miau " << std::endl;
    instruction->Debug(0);

    std::string functionName;
    if(instruction->children[0]->children.size() > 0){
      functionName = instruction->children[0]->arguments[0]->data;
      res += TranspileInstruction(instruction->children[0]->children[0], &callRetType, &recievedContainer, root, before);
    } else {
      functionName = instruction->children[0]->data; 
      recievedContainer = root->data;
    }


    std::cout << "HJDI=SJADSIOUHA"<< std::endl;
    if(recievedContainer == nullptr){
      std::cout << termcolor::red << "1" << termcolor::reset << std::endl;
      return res;
    }
    recievedContainer->Print(0);


    Object* objFunction = recievedContainer->Fetch(functionName);
    if(objFunction == nullptr){
      std::cout << termcolor::red << "2" << termcolor::reset << std::endl;
      return res; 
    }

    ObjectFunction* function = dynamic_cast<ObjectFunction*>(recievedContainer->Fetch(functionName));
    if(function == nullptr){
      std::cout << termcolor::red << "3" << termcolor::reset << std::endl;
      return res; 
    }

    std::vector<std::string> oldParams;
    if(callRetType != nullptr){
      // Tipus que tenim de sota doncs el sobreescrivim amb el template
      oldParams = function->functionArgs;
      function->SetInheritedType(callRetType);
    }
    
    // S'hauria de sobreescriure res amb la funció de verdad
    ObjectCFunction* cfunction = dynamic_cast<ObjectCFunction*>(function);
    if(cfunction != nullptr){
      if(callRetType == nullptr) res = cfunction->cFunctionData->bind;
      else res += "." + cfunction->cFunctionData->bind;
      env->Use(cfunction);
    }

    ObjectNativeFunction* nativeFunction = dynamic_cast<ObjectNativeFunction*>(function);
    if(nativeFunction != nullptr){
      res += nativeFunction->GetName();
    }

    ObjectType *funcType = env->FetchType(function->returnType), *argType;
    std::vector<ObjectType *> argTypes;
    // Check args
    res += "(";
    for (int i = 0; i < instruction->arguments.size(); i++) {
     res += TranspileExpression(instruction->arguments[i], &argType, before);
     argTypes.push_back(argType);
      if(i < instruction->arguments.size() - 1) res += ",";
    }
    res += ")";

    if(!function->CheckArgs(argTypes, env)){
      std::cout << termcolor::red << "Invalid types" << termcolor::reset << std::endl;
      return res; 
    }

    // Desfem els canvis de template
    if(callRetType != nullptr) function->functionArgs = oldParams;
    
    *scope = funcType->constructor->typeMethods;
    *returnType = funcType;

  } else if (instruction->type == NODE_ACCESSOR) {
    ObjectType *indexType = nullptr, *accessorType = nullptr;
    ObjectContainer *beforeContainer;

    res += TranspileInstruction(instruction->children[0], &callRetType, &beforeContainer, root, before);
    if (callRetType->constructor == env->FetchProtoType("Arr")) {
      accessorType = callRetType->children[0];
    }
  
    res += "[" + TranspileExpression(instruction->arguments[0], &indexType, before) + "]";

    std::cout << accessorType << std::endl;
    if(accessorType == nullptr){
      std::cout << termcolor::red << "Accessor nullptr" << termcolor::reset << std::endl;
      return res;
    }
    *returnType = accessorType;
    *scope = accessorType->constructor->typeMethods;
  } else {
    // Com que tenim una expressió el instruction container serà el del tipus
    // Pot ser també un identifier nose
    ObjectType* expressionType;
    res = TranspileExpression(instruction, &expressionType, before);

    if(instruction->type == NODE_IDENTIFIER){
      // Es un identifier, no té childs, hem d'aconseguir l'objecte des de root
      // i retornar-lo
      std::cout << "Ñe hauria de ser array" << std::endl;
      Object* identifierObject = root->GetObject(instruction->data);
      if(identifierObject == nullptr){
        std::cout << termcolor::red << "Identifier '" + instruction->data + "' doesn't exists" << termcolor::reset << std::endl;
        root->data->Print(0);
        return res;
      }
      // Aqui ara hem de veure com assignar el tipus a containers, funcions, etc..
      // Tots els tipus fills de Object
      *returnType = nullptr;
     
      ObjectContainer* identifierContainer = dynamic_cast<ObjectContainer*>(identifierObject);
      if(identifierContainer != nullptr){
        *scope = identifierContainer;
        (*scope)->Print(0);
        std::cout << "Eso es ara eso" << std::endl;
        return res;
      }
      ObjectVariable* identifierVariable = dynamic_cast<ObjectVariable*>(identifierObject);
      if(identifierVariable != nullptr){
        ObjectType* varType = identifierVariable->GetType();
        *scope = varType->constructor->typeMethods;
        *returnType = varType; 
      }
    } else {
      // Ha de ser una expressió, de la qual podem determinar el tipus
      *scope = expressionType->constructor->typeMethods;
      *returnType = expressionType;
    }
  }
  return res;
}

/*
std::string Transpiler::TranspileGetter(ZagIR::Node *getter,
                                        ObjectType **returnType,
                                        std::string *before) {

  // Potser tenim a un amic accessor!1!

  if (!env->Exists(getter->arguments[0]->data)) {
    Logs::Error("Object does not exists");
    return "";
  }

  Object *scoped = env->Fetch(getter->arguments[0]->data);
  ZagIR::Node *currentGetter = getter;

  ObjectContainer *scopedContainer = dynamic_cast<ObjectContainer *>(scoped);
  ObjectVariable *scopedVariable = dynamic_cast<ObjectVariable *>(scoped);

  // Si ens adreçem a un container serà tal cual, si és una variable,
  // ens adrecem al container del seu prototip
  std::string res = "";
  if (scopedVariable != nullptr) {
    scopedContainer = scopedVariable->GetType()->constructor->typeMethods;
    res += scopedVariable->Transpile() + ".";
  }

  while (getter->children.size() > 0) {

    getter = getter->children[0];

    if (getter->type == ZagIR::NODE_CALL) {
      scoped = scopedContainer->GetObject(getter->data);
      res += getter->data;
    } else {
      scoped = scopedContainer->GetObject(getter->arguments[0]->data);
      res += getter->arguments[0]->data;
    }
    res += ".";

    scopedContainer = dynamic_cast<ObjectContainer *>(scoped);
  }

  if (scoped == nullptr) {
    throw std::logic_error("Error invalid getter");
  }

  // std::cout << "scoped: " << std::endl;
  // scoped->Print(0);

  if (currentGetter->type == NODE_CALL) {
    ObjectFunction *ofunc = dynamic_cast<ObjectFunction *>(scoped);
    if (ofunc != nullptr) {

      // En cas de tenir variable actualitzem els tempaltes a la funció
      std::vector<std::string> oldParams;
      if(scopedVariable != nullptr){
        oldParams = ofunc->functionArgs;
        ofunc->SetInheritedType(scopedVariable->GetType());
      }

      res += TranspileGCall(ofunc, currentGetter, returnType, before);

      // Desfem els canvis
      if(scopedVariable != nullptr) ofunc->functionArgs = oldParams;
      return res;
    } else {
      // A part, afegir mètodes aquí també
      Logs::Error("Object is not a func (idk what happened)");
      return "";
    }
  } else {
    // Només pot ser un getter que ens queda de l'últim identificador
    currentGetter = currentGetter->arguments[0];
    res += currentGetter->data;
    return res;
  }
}
*/

// TODO: Carregar-se això igual
std::string Transpiler::NavigateContainer(Node **getter, Object **finalObject,
                                          ObjectContainer *scopedContainer) {

  Object *scoped;
  std::string path = "";
  while ((scopedContainer != nullptr) && (*getter)->children.size() > 0) {

    *getter = (*getter)->children[0];

    if ((*getter)->type == ZagIR::NODE_CALL) {
      scoped = scopedContainer->GetObject((*getter)->data);
      path += (*getter)->data;
    } else {
      scoped = scopedContainer->GetObject((*getter)->arguments[0]->data);
      path += (*getter)->arguments[0]->data;
    }
    path += ".";

    scopedContainer = dynamic_cast<ObjectContainer *>(scoped);
  }

  if (scoped == nullptr) {
    if ((*getter)->type == NODE_GETTER) {
      // Més getters que el que hi ha al objecte
      // Podria donar support a constants???
      std::cout << "Invalid" << std::endl;

      // env->DumpEnvironment();
      throw std::logic_error("Error invalid getter");
    }
  }

  // std::cout << "scoped: " << std::endl;
  // scoped->Print(0);
  *finalObject = scoped;

  return path;
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

  *returnType = env->Construct(env->FetchProtoType("Arr"), {firstType});

  return result;
}

// TODO: Em vull carregar això
std::string Transpiler::TranspileAccessor(Node *accessor,
                                          ObjectType **returnType,
                                          std::string *before) {
  // Uhh això pot ser un array!
  ObjectType *containerType, *indexType;
  std::string content =
      TranspileExpression(accessor->children[0], &containerType, before);
  std::string index =
      TranspileExpression(accessor->arguments[0], &indexType, before);

  if (containerType->constructor == env->FetchProtoType("Arr")) {
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
