#include "formatter.h"

#include <exception>
#include <string>
#include <iostream>
#include <unordered_map>

using namespace ZagCXX;

std::string Formatter::Format(std::string preFormat, std::unordered_map<int, std::string> formatList){
  std::string formatted = "";
  std::string key = "";

  int k;
  for(int i = 0; i < preFormat.size(); i++){
    if(i < preFormat.size() - 1){
      switch(preFormat[i]){
        case '{':
          if(preFormat[i + 1] == '{'){
            i++;
          } else {
            k = i + 1;
            key = "";
            while(preFormat[k] != '}' && k < preFormat.size()){
              key += preFormat[k];
              k++;
            }
            i = k + 1;
            try {
              formatted += formatList[std::stoi(key)];
            } catch (std::exception) {
              std::cout << "Error formatting" << std::endl;
            }
          }
          break;
        case '}':
          if(preFormat[i + 1] == '}'){
            i++;
          } 
          break;
        default:
          break;
      }
    }
    formatted += preFormat[i];
  }
  return formatted;
}
