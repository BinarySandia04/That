#include "internal_package.h"

#include "Utils/thatpath.h"
#include "packages.h"

using namespace That;

Package *FetchInternalPackage(std::string path) {
  fs::path p = THAT_PATH / "sources" / path;
  // TODO
  if (path == "_Internal") {
    // Això ho haurem de fer lleig i manual ja que no tenim arxius de config
    Package *pack = new Package(p);

    PackageInfo internalInfo;
    internalInfo.path = p;
    internalInfo.name = "_Internal";
    internalInfo.root = "";
    internalInfo.version = THAT_VERSION;
    internalInfo.import_name = "";
    internalInfo.display_name = "Internal library";

    pack->packInfo = internalInfo;

    // Ara hem d'esciure totes les binds manualment

    //
    // ----------------- TYPES -------------------
    //
    // Int
    CType *intType = new CType(pack, "Int");
    intType->parent = "int";
    intType->upgrades_to = "Num";
    intType->subpackage = "";
    intType->global = true;

    pack->binds.push_back(intType);

    // Bul
    CType *bulType = new CType(pack, "Bul");
    bulType->parent = "bool";
    bulType->upgrades_to = "Int";
    bulType->subpackage = "";
    bulType->global = true;

    pack->binds.push_back(bulType);

    // Num
    CType *numType = new CType(pack, "Num");
    numType->parent = "double";
    numType->subpackage = "";
    numType->global = true;

    pack->binds.push_back(numType);

    // Any
    CType *anyType = new CType(pack, "any");
    anyType->parent = "std::any";
    anyType->include.push_back("any");
    anyType->subpackage = "";
    anyType->global = true;

    pack->binds.push_back(anyType);

    // Nil
    CType *nilType = new CType(pack, "Nil");
    nilType->parent = "void";
    nilType->subpackage = "";
    nilType->global = true;

    pack->binds.push_back(nilType);

    // Str
    CType *strType = new CType(pack, "Str");
    strType->parent = "std::string";
    strType->include.push_back("string");
    strType->accessor_map["Int"] = "Chr";
    strType->subpackage = "";
    strType->global = true;

    // Str.size()
    CFunction *strSizeMethod = new CFunction(pack, "size");
    strSizeMethod->bind = "size";
    strSizeMethod->retType = "Int";

    strType->children.push_back(strSizeMethod);

    pack->binds.push_back(strType);

    // Chr
    CType *chrType = new CType(pack, "Chr");
    chrType->parent = "char";
    chrType->upgrades_to = "Int";
    chrType->subpackage = "";
    chrType->global = true;

    pack->binds.push_back(chrType);

    // Arr
    CType *arrType = new CType(pack, "Arr");
    arrType->parent = "std::vector";
    arrType->include.push_back("vector");
    arrType->templates = 1;
    arrType->subpackage = "";
    arrType->global = true;

    // Arr.push(e)
    CFunction *arrPushMethod = new CFunction(pack, "push");
    arrPushMethod->bind = "push_back";
    arrPushMethod->retType = "Nil";
    arrPushMethod->funcArgs.push_back("$1");

    arrType->children.push_back(arrPushMethod);

    // Arr.pop()
    CFunction *arrPopMethod = new CFunction(pack, "pop");
    arrPopMethod->bind = "pop_back";
    arrPopMethod->retType = "Nil";

    arrType->children.push_back(arrPopMethod);

    // Arr.size()
    CFunction *arrSizeMethod = new CFunction(pack, "size");
    arrSizeMethod->bind = "size";
    arrSizeMethod->retType = "Int";

    arrType->children.push_back(arrSizeMethod);

    pack->binds.push_back(arrType);

    // -------------------------------------------
    //
    // --------------- CONVERSIONS ---------------

    Conversion *intToBul = new Conversion(pack);
    intToBul->implicit = true;
    intToBul->fromType = "Int";
    intToBul->toType = "Bul";
    pack->binds.push_back(intToBul);

    Conversion *numToBul = new Conversion(pack);
    numToBul->implicit = true;
    numToBul->fromType = "Num";
    numToBul->toType = "Bul";
    pack->binds.push_back(numToBul);

    Conversion *intToNum = new Conversion(pack);
    intToNum->implicit = true;
    intToNum->fromType = "Int";
    intToNum->toType = "Num";
    pack->binds.push_back(intToNum);

    Conversion *intToStr = new Conversion(pack);
    intToStr->fromType = "Int";
    intToStr->toType = "Str";
    intToStr->bind = "IntToString";
    pack->binds.push_back(intToStr);

    Conversion *numToStr = new Conversion(pack);
    numToStr->fromType = "Int";
    numToStr->toType = "Bul";
    numToStr->bind = "NumToString";
    pack->binds.push_back(numToStr);

    // -------------------------------------------
    //
    // ---------------- OPERATIONS ---------------

    COperation *strPlusInt = new COperation(pack);
    strPlusInt->bind = "_internal::StrPlusInt";
    strPlusInt->lType = "Str";
    strPlusInt->rType = "Int";
    strPlusInt->op = "+";
    strPlusInt->resType = "Str";
    strPlusInt->headers.push_back("operations.h");
    pack->binds.push_back(strPlusInt);

    COperation *intPlusStr = new COperation(pack);
    intPlusStr->bind = "_internal::IntPlusStr";
    intPlusStr->lType = "Int";
    intPlusStr->rType = "Str";
    intPlusStr->op = "+";
    intPlusStr->resType = "Str";
    intPlusStr->headers.push_back("operations.h");
    pack->binds.push_back(intPlusStr);

    COperation *strPlusNum = new COperation(pack);
    strPlusNum->bind = "_internal::StrPlusNum";
    strPlusNum->lType = "Str";
    strPlusNum->rType = "Num";
    strPlusNum->op = "+";
    strPlusNum->resType = "Str";
    strPlusNum->headers.push_back("operations.h");
    pack->binds.push_back(strPlusNum);

    COperation *numPlusStr = new COperation(pack);
    numPlusStr->bind = "_internal::NumPlusStr";
    numPlusStr->lType = "Num";
    numPlusStr->rType = "Str";
    numPlusStr->op = "+";
    numPlusStr->resType = "Str";
    numPlusStr->headers.push_back("operations.h");
    pack->binds.push_back(numPlusStr);

    return pack;
  }
  throw std::runtime_error("No internal package named " + path);
}
