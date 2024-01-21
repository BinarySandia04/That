#include "error.h"

#include "libs/termcolor.hpp"
#include <iostream>
#include <cmath>

#define ZAG_ERROR_PADDING_LINES 1

using namespace Zag;

Error::Error(int position, int length, std::string content, std::string fileName) {
  this->position = position;
  this->length = length;
  this->content = content;
  this->fileName = fileName;
}

void Error::Print(std::string source) {

  SetLineNum(source);

  std::cout << termcolor::bold << this->fileName << ":" << line << ":" << column << termcolor::reset << " ";
  std::cout << termcolor::red << termcolor::bold << "Error: " << termcolor::reset;
  std::cout << this->content << std::endl;

  PrintInline();
}

void Error::PrintInline(){
  for(int i = ZAG_ERROR_PADDING_LINES; i > 0; i--){
    PrintSourceLine(source, line - i);
  }
  PrintErrorSourceLine(source, line, column, length);
  for(int i = 1; i <= ZAG_ERROR_PADDING_LINES; i++){
    PrintSourceLine(source, line + i);
  }
}

void Error::PrintSourceLine(std::string source, int line){
  if(line <= 0) return;

  std::cout << " " << line;
  int trailing = 5 - int(log10(line) + 1);
  for(int i = 0; i < trailing; i++) std::cout << " ";
  std::cout << "| ";

  int start = 0;
  int end = 0;
  int c = 1;
  while(c < line && start < source.size()){
    if(source[start] == '\n') c++;
    start++;
  }

  end = start;
  while(source[end] != '\n' && end < source.size()){
    end++;
  }

  std::cout << source.substr(start, end - start) << std::endl;
}

void Error::PrintErrorSourceLine(std::string source, int line, int column, int length){
  if(line <= 0) return;

  std::cout << " " << termcolor::bold << line << termcolor::reset;
  int trailing = 5 - int(log10(line) + 1);
  for(int i = 0; i < trailing; i++) std::cout << " ";
  std::cout << "| ";

  int start = 0;
  int end = 0;
  int c = 1;
  while(c < line && start < source.size()){
    if(source[start] == '\n') c++;
    start++;
  }

  end = start;
  while(source[end] != '\n' && end < source.size()){
    end++;
  }

  std::cout << source.substr(start, column - 1);
  std::cout << termcolor::red << termcolor::bold << source.substr(start + column - 1, length) << termcolor::reset;
  std::cout << source.substr(start + column + length - 1, end - start - column - length + 1) << std::endl;
}

void Error::SetLineNum(std::string source) {
  this->source = source;
  int line = 1;
  int column = 0;

  if (position >= source.size()) {
    std::cout << termcolor::red << termcolor::bold << "Internal Error"
              << termcolor::reset;
    std::cout << "Bad formatted error. Aborting" << termcolor::reset
              << std::endl;
    exit(1);
  }

  for (int i = 0; i < position; i++) {
    if (source[i] == '\n') {
      line++;
      column = 0;
    } else
      column++;
  }

  this->line = line;
  this->column = column;
}
