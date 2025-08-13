#ifndef utilities_h
#define utilities_h

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <experimental/filesystem>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <iomanip>
#include <ctime>
#include <set>
#include <cstdlib>
#include "globals.h"
#include "config.h"

namespace fs = std::experimental::filesystem;

// LOGGING FUNCTIONS - Normal, Error, Debug-flagged.

void error_log(std::string message,int exit_code);
void normal_log(std::string message);
void debug_log(std::string message);
void splash_screen();
std::string GetTimeAndDate();
void PrintUsage();

// System Commands
void silent_shell(const char* cmd);
void silent_shell(std::string cmd); //overload for above.

std::string GetSysResponse(const char* cmd);
std::string GetSysResponse(std::string cmd); //overload for above.

bool CheckProgAvailable(const char* program);
bool CheckProgAvailable(std::string program); //overload for above.

bool CheckProgAvailable(const char* program, const char* module);
bool CheckProgAvailable(std::string program, std::string module); //overload for above.

// File Handling
void write_to_file(std::string inputfilename, std::string buffer);
void append_to_file(std::string inputfilename, std::string buffer);
std::string LastLineOfFile(std::string filename);
int count_lines_in_file(std::string filename);
void compress_and_delete(std::string directory);
std::vector<std::string> sort_files_by_timestamp(std::string directory,std::string pattern);
std::string MakeIterativeDirectoryName(std::string dir_base, int num_zeros);

// String Utilities
int is_empty(const char *s);
std::string string_between(std::string incoming, std::string first_delim, std::string second_delim);
std::vector<std::string> split_string(std::string incoming, std::string delim);

// Command Line Parser
void parse_command_line_arguments(std::map<std::string,std::vector<std::string>> &flags, int argc, char** argv);
void check_debug_mode(std::map<std::string,std::vector<std::string>> &flags);



#endif