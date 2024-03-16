#include "error.h"

#include <cstdint>
#include "termcolor/termcolor.hpp"
#include <iostream>
#include <cmath>

#define ERROR_PADDING 1

using namespace ZagIR;

Error::Error(){
  fired = false;
}

Error::Error(int position, int length, std::string content, std::string fileName) {
  fired = true;
  this->position = position;
  this->length = length;
  this->content = content;
  this->fileName = fileName;
  this->lineCount = 0;
}

void Error::Print(std::string source) {

  this->source = source;
  CountLines();
  SetLineNum();

  std::cout << termcolor::bold << this->fileName << ":" << line << ":" << column << termcolor::reset << " ";
  std::cout << termcolor::red << termcolor::bold << "Error: " << termcolor::reset;
  std::cout << this->content << std::endl;

  PrintInline();
}

void Error::PrintInline(){

  int above = ERROR_PADDING;
  int below = ERROR_PADDING;

  if(lineCount < 2 * ERROR_PADDING + 1){
    above = line - 1;
    below = 2 * ERROR_PADDING - line;
  } else if(line - above < 0){
    above += line - above;
    below -= line - above;
  } else if((lineCount - line) - below < 0){
    below += lineCount - line;
    above -= lineCount - line;
  }

  for(int i = above; i > 0; i--){
    PrintSourceLine(source, line - i);
  }
  PrintErrorSourceLine(source, line, column, length);
  for(int i = 1; i <= below; i++){
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

void Error::SetLineNum() {
  int line = 1;
  int column = 0;

  if (position >= source.size()) {
    position = source.size() + 1;

    std::string hangling = "~ <- HERE";

    source = source + hangling;
    length = hangling.size();
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

void Error::CountLines(){
  for(int i = 0; i < source.size(); i++) if(source[i] == '\n') lineCount++;
}

