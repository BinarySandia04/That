#pragma once

#include <string>
#include "../flags/flags.h"
#include "../compiler/lexer.h"
#define DEBUG

/*
En principi el Kernel és l'objecte capaç de compilar i interpretar el codi suposo
*/
namespace That {
    class Kernel {
        public:
            Kernel();
            ~Kernel();

            void Compile(std::string code, Flag::Flags flags);
            void Run(std::string filename);
            void Run();

            void CompileScript(std::string file, Flag::Flags flags);
            void SendByteScript(char name[]);

            void PrintVersion();
            void Cli();
        private:
            void DebugTokens(std::vector<That::Token> tokens);
    };
}