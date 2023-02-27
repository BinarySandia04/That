#pragma once

#include <string>
#include "../flags/flags.h"
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
            void Send(char filename[]);
            void SendScript(std::string file, Flag::Flags flags);
            void SendByteScript(char name[]);

            void PrintVersion();
            void Cli();
    };
}