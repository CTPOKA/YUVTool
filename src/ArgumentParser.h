#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

class ArgumentParser
{
public:
    ArgumentParser(int argc, char *argv[]);

    std::string getInPath() { return getParam(0); }
    std::string getOutPath() { return getParam(1); }
    int getWidth() { return getIntParam(2); }
    int getHeight() { return getIntParam(3); }
    std::string getBMPPath() { return getParam(4); }

    bool hasOption(const std::string &option) const;
    std::string getOption(const std::string &option, const std::string &defaultValue = "") const;
    float getFloatOption(const std::string &option, float defaultValue = 1.0f) const;
    int getIntOption(const std::string &option, int defaultValue = 0) const;

    static void printUsage(const std::string &programName = "YUVTool");

private:
    std::vector<std::string> params;
    std::map<std::string, std::string> options;

    void parse(int argc, char *argv[]);
    std::string getParam(size_t index) const;
    int getIntParam(size_t index) const;
};
