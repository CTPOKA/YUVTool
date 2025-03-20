#include "ArgumentParser.h"

ArgumentParser::ArgumentParser(int argc, char *argv[])
{
    parse(argc, argv);
}

std::string ArgumentParser::getParam(size_t index) const
{
    if (index < params.size()) {
        return params[index];
    }
    printUsage();
    exit(1);
}

int ArgumentParser::getIntParam(size_t index) const
{
    try {
        return std::stoi(getParam(index));
    } catch (const std::exception &) {
        printUsage();
        exit(1);
    }
}

bool ArgumentParser::hasOption(const std::string &option) const
{
    return options.find(option) != options.end();
}

std::string ArgumentParser::getOption(const std::string &option,
                                      const std::string &defaultValue) const
{
    auto it = options.find(option);
    return (it != options.end() && !it->second.empty()) ? it->second : defaultValue;
}

float ArgumentParser::getFloatOption(const std::string &option, float defaultValue) const
{
    auto it = options.find(option);
    return (it != options.end() && !it->second.empty()) ? std::stof(it->second) : defaultValue;
}

int ArgumentParser::getIntOption(const std::string &option, int defaultValue) const
{
    auto it = options.find(option);
    return (it != options.end() && !it->second.empty()) ? std::stoi(it->second) : defaultValue;
}

void ArgumentParser::printUsage(const std::string &programName)
{
    std::cerr << "Usage: " << programName
              << " input.yuv output.yuv width height image.bmp [-x X] [-y Y] [-a OPACITY] [--yes]\n"
              << " input.yuv    - input YUV file\n"
              << " output.yuv   - output YUV file\n"
              << " width        - video width\n"
              << " height       - video height\n"
              << " image.bmp    - image (BMP format)\n"
              << "Options:\n"
              << " -x X         - X offset (default: 0)\n"
              << " -y Y         - Y offset (default: 0)\n"
              << " -a OPACITY   - image opacity (default: 1.0)\n"
              << " --yes        - Automatically confirm all prompts\n"
              << "  -h, --help   - Show this help message\n";
}

void ArgumentParser::parse(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
        } else if (arg.rfind("-", 0) == 0) {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options[arg] = argv[++i];
            } else {
                options[arg] = "";
            }
        } else {
            params.push_back(arg);
        }
    }
}
