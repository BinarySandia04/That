#include <iostream>
#include <fstream>
#include <filesystem>

#include "builder.h"

using namespace That;

std::cout<<"JOMAMA SO FAT.. SHE.. UH.. THE SECURITY GUARD DIDNT WANT 2 SEE HER ON HTE CAMS"

std::filesystem::path Builder::GetPath(){
    return std::filesystem::current_path();
}

std::filesystem::path Builder::CreateWorkFolder(){
    std::filesystem::create_directory(GetPath() / ".that");
    return GetPath() / ".that";
}

void Builder::OverrideFile(std::filesystem::path file, std::string content){
    if(std::filesystem::exists(file)) std::filesystem::remove(file);
    std::ofstream ofs(file);
    ofs << content;
    ofs.close();
}

void Builder::AppendFile(std::filesystem::path file, std::string content){
    std::fstream outfile(file, outfile.out | outfile.app);

    outfile << content;
    outfile.close();
}

std::string Builder::GetExeName(std::string name){
    int r = name.size();
    for(int i = name.size() - 1; i > 0; i--) if(name[i] == '.') r = i;
    std::string res = "";
    for(int i = 0; i < r; i++) res += name[i];
    return res;
}

void Builder::BuildCode(std::string code, std::string name){
    std::filesystem::path workFolder = CreateWorkFolder();
    OverrideFile(workFolder / (GetExeName(name) + ".cpp"), code);
    CMProject cmp(name, workFolder);
    cmp.AddExecutable(name + ".cpp");
    cmp.Build();
    // Run(GetPath() / name);
}

// Classe CMProject
Builder::CMProject::CMProject(std::string name, std::filesystem::path path){
    this->projectName = name;
    this->path = path;
    std::string content = 
    "cmake_minimum_required(VERSION 3.16)\n"
    "project(" + name + ")\n";
    OverrideFile(path / "CMakeLists.txt", content);
}

void Builder::CMProject::AddExecutable(std::filesystem::path file){
    std::string content = "add_executable(build " + std::string(file.filename()) + ")\n";
    AppendFile(this->path / "CMakeLists.txt", content);
}

void Builder::CMProject::Build(){
    std::filesystem::path oldPath = std::filesystem::current_path();
    std::filesystem::current_path(this->path);

    // TODO: Linux???
    system("cmake CMakeLists.txt > /dev/null");
    system("cmake --build . > /dev/null");

    std::filesystem::copy_file(this->path / "build", oldPath / (std::string(this->projectName) + ".exe"));
    // TODO: SI es linux 
    std::filesystem::rename(oldPath / (std::string(this->projectName) + ".exe"), oldPath / (std::string(this->projectName)));

    std::filesystem::current_path(oldPath);
}

void Builder::Run(std::filesystem::path file){
    system(std::string(file).c_str());
}
