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
        TranspileFunction(statement, "", &functionDeclaration, &functionDefinition, true);
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

  Node *currentNode = assignation->children[0];
  // assignation->Debug(0);

  std::string accessorTranspiled = "";
  if (currentNode->type != NODE_IDENTIFIER) {
    // Ok canviar això i adaptar-ho a la instrucció TranspileInstruction
    ObjectType* lType;
    ObjectContainer* fromContainer;
    accessorTranspiled = TranspileInstruction(currentNode, &lType, &fromContainer, env->GetTopScope(), before);
    /*
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
    */
  }
  return accessorTranspiled + " " + assignation->data +
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
                              std::string nameSpace,
                              std::string *functionDeclaration,
                              std::string *functionDefinition, // Actual content
                              bool transpileContent) {

  ObjectNativeFunction *func = new ObjectNativeFunction();
  func->returnType = "Nil"; // TODO: Canviar
  // Sanitization?
  std::string funcName = "_f_" + function->data;
  if(nameSpace != "") funcName.insert(0, nameSpace + "::");
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

  std::string defBlock;
  if(transpileContent) defBlock = TranspileBlock(function->children[0]);
  env->PopScope();

  if (functionDeclaration != nullptr)
    *functionDeclaration += returnType + " " + funcName + arguments + ";";
  if (functionDefinition != nullptr && transpileContent){
    *functionDefinition +=
        returnType + " " + funcName + arguments + "{" + defBlock + "}";
  }

  return func;
}

std::string Transpiler::TranspileKin(Node *kin, std::string *before) {
  // kin->Debug(0);
  std::string kinName = kin->data;

  classDeclaration += "class c_" + kinName + ";\n";

  std::string privatePart = "", publicPart = "";

  classDefinition += "class c_" + kinName + " {\n";

  std::string methodsDefinition = "";

  env->PushScope();
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

    methodName = PreTranspileMethod(kin->children[i], kinName, writeTo, &addedObj, before);
    if (methodName == "")
      return "";

    env->GetTopScope()->AddObject(methodName, addedObj, p);
  }

  for(int i = 0; i < kin->children.size(); i++){
    PostTranspileMethod(kin->children[i]->children[0], kinName, &methodsDefinition);
  }

  classDefinition +=
      "public:\n" + publicPart + "\nprivate:\n" + privatePart + "\n};\n" + methodsDefinition;

  // TODO: Canviar a root?
  env->AddToRoot(kinName, new ObjectProtoType(env->GetTopScope()->data, kinName));
  env->GetTopScope()->Detach();
  env->PopScope();
  return "";
}

std::string Transpiler::PreTranspileMethod(Node *method, std::string className, std::string *writeTo,
                                        Object **methodObject,
                                        std::string *before) {
  std::string methodName;

  Node *attribute = method->children[0];

  ObjectType *transpileType;
  ObjectVariable *newVar;

  switch (attribute->type) {
  case ZagIR::NODE_ASSIGNATION:
    transpileType = env->FetchType(attribute->arguments[0]);
    newVar = new ObjectVariable(transpileType, attribute->children[0]->data);
    *methodObject = newVar;
    methodName = attribute->children[0]->data;

    *writeTo += transpileType->Transpile() + " " + newVar->Transpile() + ";\n";
    break;
  case ZagIR::NODE_FUNCTION:
    methodName = attribute->data;
    *methodObject = TranspileFunction(attribute, "", writeTo, nullptr, false);
    *writeTo += "\n";
    break;
  case ZagIR::NODE_IDENTIFIER:
    // Igual que assignation pero amb any
    transpileType = env->FetchType("Any");
    newVar = new ObjectVariable(transpileType, attribute->data);
    *methodObject = newVar;
    methodName = attribute->data;

    *writeTo += transpileType->Transpile() + " " + newVar->Transpile() + ";\n";
    break;
  default:
    // std::cout << "Method has no name?" << std::endl;
    *methodObject = new ObjectContainer();
    return "_";
  }
  return methodName;
}

void Transpiler::PostTranspileMethod(Node* method, std::string kinName, std::string *after){
  std::cout << "NJDHSAKJHKJH:::::: " << *after << std::endl;
  if(method->type != NODE_FUNCTION) return;
  Object* o = TranspileFunction(method, "c_" + kinName, nullptr, after, true);
  delete o;
  *after += "\n";
  std::cout << "NJDHSAKJHKJH:::::: " << *after << std::endl;
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
    res += "."; // Translate?
                             //
    std::cout << "RES: " << res << std::endl;

    if(recievedContainer->Exists(getterName)){
      Object* gettedObject = recievedContainer->GetObject(getterName); 
      // Pot ser un container o no. Si no ho és, agafem el tipus que tenim

      // Sabem que no es nullptr perque existeix
      ObjectContainer* gettedContainer = dynamic_cast<ObjectContainer*>(gettedObject);
      if(gettedContainer != nullptr){
        *scope = gettedContainer;
        res += getterName;
      }

      ObjectVariable* gettedVariable = dynamic_cast<ObjectVariable*>(gettedObject);
      if(gettedVariable != nullptr){
        *scope = gettedVariable->GetType()->constructor->typeMethods;
        res += gettedVariable->Transpile();
        *returnType = gettedVariable->GetType();

        // gettedContainer = (*returnType)->constructor->typeMethods;
      }

    } else {
      std::cout << termcolor::red << "Invalid getter" << termcolor::reset << std::endl;
      return res;
    }
  } else if (instruction->type == NODE_CALL) {
    // Funció dins de qualsevol cosa
    // té un children que es getter i te el nom de la func i el children del children és el seguent

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
      res += ".";
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
      if(callRetType != nullptr){
        std::string typeName = callRetType->constructor->cTypeInfo->accessor_map[indexType->identifier];
        *returnType = env->FetchType(typeName);
        *scope = (*returnType)->constructor->typeMethods;
      }
      // std::cout << termcolor::red << "Accessor nullptr" << termcolor::reset << std::endl;
      // return res;
    } else {
      *returnType = accessorType;
      *scope = accessorType->constructor->typeMethods;
    }
  } else {
    // Com que tenim una expressió el instruction container serà el del tipus
    // Pot ser també un identifier nose
    ObjectType* expressionType;
    res = TranspileExpression(instruction, &expressionType, before);
    std::cout << "RES::: " << res << std::endl;

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
