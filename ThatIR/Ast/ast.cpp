#include "ast.h"

bool ThatIR::GenerateAst(std::string code, std::string fileName, Node *ast, bool debug){
  Error error;

  Scanner scanner = Scanner(code, fileName);
  std::vector<Token> tokens;

  if(!scanner.ScanTokens(&tokens, &error)){
    error.Print(code);
    return false;
  }

  if(debug){
    for(int i = 0; i < tokens.size(); i++){
      std::cout << termcolor::yellow << tokens[i].ToString() << termcolor::reset << std::endl;
    }
  }

  Parser parser(fileName);
  if(!parser.GenerateAST(&tokens, &ast, &error)){
    error.Print(code);
    return false;
  }

  if(debug) ast->Debug(0);
  return true;
}
