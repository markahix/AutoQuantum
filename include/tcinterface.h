#ifndef TCINTERFACE_H
#define TCINTERFACE_H

#include "utilities.h"

//identify known terachem flags/keywords
// std::map<std::string, std::string> TC_ANY_DEFAULTS;
// std::map<std::string, std::string> TC_OPT_DEFAULTS;
// std::map<std::string, std::string> TC_SPE_DEFAULTS;
// std::map<std::string, std::string> TC_FREQ_DEFAULTS;
// std::map<std::string, std::string> TC_BOMD_DEFAULTS;

// Identify calculation type
// std::string CALC_TYPE;
// std::string TC_FILENAME;
// void get_calc_type(std::map<std::string,std::vector<std::string>> &flags);

// // Identify maximum keyword length, then pad for writing to file.
// int MAX_KEY_LEN;
// void get_max_keyword_length(std::map<std::string,std::vector<std::string>> flags);

// Write TeraChem Input
// void generate_full_keyword_set(std::map<std::string,std::vector<std::string>> &flags, std::map<std::string,std::string> &keywords);

void Write_TC_Input(std::map<std::string,std::vector<std::string>> &flags, std::map<std::string,std::string> &keywords);
void SubmitSlurmJob(std::map<std::string,std::string> keywords);
void RunTeraChem();


#endif