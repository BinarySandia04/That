#include "flags.h"

That::Flag::RunInfo That::Flag::GetRunInfo(std::vector<std::string> args){
    Flag::RunInfo info;

    std::vector<std::string> files;
    Flags flags = Flags::RUN;

    std::map<std::string, Flags> flagMap = {
        {"-a", Flags::ASSEMBLY},
        {"-d", Flags::DEBUG},
        {"-c", Flags::COMPILE}
    };

    for(int i = 0; i < args.size(); i++){
        if (flagMap.find(args[i]) == flagMap.end()) {
            files.push_back(args[i]);
        } else {
            flags = (Flags)(flags | flagMap[args[i]]);
        }
    }

    info.flags = flags;
    info.files = files;

    return info;
}