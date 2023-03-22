#pragma once

#include <string>
#include <filesystem>

namespace That {
    namespace Builder {
        void BuildCode(std::string code, std::string outName);
        std::string GetExeName(std::string name);
        std::filesystem::path GetPath();
        std::filesystem::path CreateWorkFolder();

        void OverrideFile(std::filesystem::path file, std::string content);
        void AppendFile(std::filesystem::path file, std::string content);
        
        void Run(std::filesystem::path file);

        class CMProject {
            public:
                CMProject(std::string name, std::filesystem::path path);
                void AddExecutable(std::filesystem::path fileName);
                void Build();
            private:
                std::string projectName;
                std::filesystem::path path;
        };
    }
}