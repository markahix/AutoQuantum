#include "tcinterface.h"

//identify known terachem flags/keywords
std::map<std::string, std::string> TC_ANY_DEFAULTS = {{"coordinates"       , "input.xyz" },
                                                      {"charge"            , "0" },
                                                      {"spinmult"          , "1" },
                                                      {"basis"             , "6-31gss" },
                                                      {"method"            , "b3lyp" },
                                                      {"convthre"          , "1e-7" },
                                                      {"threall"           , "1e-14" },
                                                      {"precision"         , "mixed" },
                                                      {"maxit"             , "200"},
                                                      {"scf"               , "diis+a" },
                                                      {"gpus"              , "1" },
                                                      {"gpumem"            , "256" },
                                                      {"scrdir"            , "scr/" },};
std::map<std::string, std::string> TC_OPT_DEFAULTS = {{"run"               , "minimize"},
                                                      {"new_minimizer"     , "no"},
                                                      {"min_coordinates"   , "cartesian"}};
std::map<std::string, std::string> TC_SPE_DEFAULTS = {{"run"               , "energy"},};
std::map<std::string, std::string> TC_FREQ_DEFAULTS ={{"run"               , "frequencies"},
                                                      {"mincheck"          , "false"}};
std::map<std::string, std::string> TC_BOMD_DEFAULTS ={{"run"               , "bomd"},
                                                      {"nstep"             , "1000"},
                                                      {"min_maxallowedstep", "5.0"},
                                                      {"timestep"          , "1.0"},
                                                      {"mdbc"              , "spherical"},
                                                      {"orbitalswrtfrq"    , "100"},};
std::map<std::string, std::string> TC_CASSCF_DEFAULTS={ {"casscf"              , "no"},
                                                        {"alphacas"            , "yes"},
                                                        {"alpha"               , "0.64"},
                                                        {"castarget"           , "0"},
                                                        {"castargetmult"       , "1"},
                                                        {"cassinglets"         , "3"},
                                                        {"casscfmacromaxiter"  , "0"},
                                                        {"casscfmaxiter"       , "100"},
                                                        {"casscftrustmaxiter"  , "0"},
                                                        {"casscfmicroconvthre" , "100.0"},
                                                        {"casscfmacroconvthre" , "100.0"},
                                                        {"casscfconvthre"      , "1e-04"},
                                                        {"casscfenergyconvthre", "1e-04"},
                                                        {"cpsacasscfmaxiter"   , "100"},
                                                        {"cpsacasscfconvthre"  , "0.001"},
                                                        {"closed"              , "85"},
                                                        {"active"              , "3"},
                                                        {"casguess"            , "c0.casscf"},
                                                        {"cascharges"          , "yes"},
                                                        {"ci_solver"           , "explicit"},};

// Identify calculation type
bool USE_CASSCF = false;
std::string CALC_TYPE = "NONE";
std::string TC_FILENAME = "tc.in";
std::string TC_OUTFILE = "tc.in";
std::string TC_ERRFILE = "tc.in";
void get_calc_type(std::map<std::string,std::vector<std::string>> &flags)
{   
    // After parsing the flags, there should only be one calculation type selected.
    int n_calc_types_found = 0;

    // SPE
    if (flags.count("spe") > 0)
    {
        CALC_TYPE = "SPE";
        TC_FILENAME = "tc_spe.in";
        TC_OUTFILE = "tc_spe.out";
        TC_ERRFILE = "tc_spe.err";
        n_calc_types_found++;
        flags.erase(flags.find("spe"));
    }

    // OPT
    if (flags.count("opt") > 0)
    {
        CALC_TYPE = "OPT";
        TC_FILENAME = "tc_opt.in";
        TC_OUTFILE = "tc_opt.out";
        TC_ERRFILE = "tc_opt.err";
        n_calc_types_found++;
        flags.erase(flags.find("opt"));
    }

    // FREQ
    if (flags.count("freq") > 0)
    {
        CALC_TYPE = "FREQ";
        TC_FILENAME = "tc_freq.in";
        TC_OUTFILE = "tc_freq.out";
        TC_ERRFILE = "tc_freq.err";
        n_calc_types_found++;
        flags.erase(flags.find("freq"));
    }

    // BOMD
    if (flags.count("bomd") > 0)
    {
        CALC_TYPE = "BOMD";
        TC_FILENAME = "tc_bomd.in";
        TC_OUTFILE = "tc_bomd.out";
        TC_ERRFILE = "tc_bomd.err";
        n_calc_types_found++;
        flags.erase(flags.find("bomd"));
    }

    // Error Checking for calculation types.
    if (n_calc_types_found > 1)
    {   
        PrintUsage();
        error_log("Multiple calculation types requested.  Please resubmit with single calculation type.", 1);
    }
    if (n_calc_types_found == 0)
    {
        PrintUsage();
        error_log("No calculation type requested.  Please resubmit with single calculation type.", 1);
    }

    // Check CASSCF usage
    if (flags.count("casscf") > 0)
    {
        USE_CASSCF = true;
    }
    // Done!
}

// Identify maximum keyword length, then pad for writing to file.
unsigned int MAX_KEY_LEN = 19; // current maximum from all defaults in file.
void get_max_keyword_length(std::map<std::string,std::vector<std::string>> flags)
{
    for(std::map<std::string, std::vector<std::string>>::iterator iter = flags.begin(); iter != flags.end(); iter++)
    {
        std::string k =  iter->first;
        if (k.size() > MAX_KEY_LEN)
        {
            MAX_KEY_LEN = k.size();
        }
    }
    MAX_KEY_LEN+=4; // Add 4 characters of padding to the end just to make the input file nice and tidy.
}

// Write TeraChem Input
void generate_full_keyword_set(std::map<std::string,std::vector<std::string>> &flags, std::map<std::string,std::string> &keywords)
{   
    std::map <std::string,std::string> defaults;
    if (CALC_TYPE == "SPE")
    {
        defaults = TC_SPE_DEFAULTS;
    }
    if (CALC_TYPE == "OPT")
    {
        defaults = TC_OPT_DEFAULTS;
    }
    if (CALC_TYPE == "FREQ")
    {
        defaults = TC_FREQ_DEFAULTS;
    }
    if (CALC_TYPE == "BOMD")
    {
        defaults = TC_BOMD_DEFAULTS;
    }    
    
    // Assign base TeraChem default settings.
    for (std::map<std::string,std::string>::iterator iter = TC_ANY_DEFAULTS.begin(); iter != TC_ANY_DEFAULTS.end(); iter++)
    {
        std::string key = iter->first;
        std::string val = iter->second;
        keywords[key] = val;
    }

    // Assign type-specific TeraChem default settings.
    for (std::map<std::string,std::string>::iterator iter = defaults.begin(); iter != defaults.end(); iter++)
    {
        std::string key = iter->first;
        std::string val = iter->second;
        keywords[key] = val;
    }

    // Assign user-defined TeraChem settings.
    for (std::map<std::string,std::vector<std::string>>::iterator iter = flags.begin(); iter != flags.end(); iter++)
    {
        std::string key = iter->first;
        std::vector<std::string> val_array = iter->second;
        keywords[key] = val_array[0];        
    }
}

void move_to_jobdir(std::map<std::string,std::string> keywords, std::string jobdir)
{
    std::stringstream buffer;
    buffer.str("");
    if (keywords.count("qmindices")>0)
    {
        buffer << "cp " << keywords["qmindices"] << " " << jobdir << "/" <<std::endl;
    }
    if (keywords.count("prmtop") > 0)
    {
        buffer << "cp " << keywords["prmtop"] << " " << jobdir << "/" <<std::endl;
    }
    if (keywords.count("coordinates") > 0)
    {
        buffer << "cp " << keywords["coordinates"] << " " << jobdir << "/" <<std::endl;
    }
    silent_shell(buffer.str());
}

std::string tc_input_keyword_line(std::map<std::string,std::string> &keywords, std::string key)
{
    std::stringstream buffer;
    buffer.str("");
    if (keywords.count(key) > 0)
    {
        buffer << std::left << std::setw(MAX_KEY_LEN) << std::setfill(' ') << key;
        buffer << std::left << keywords[key] << std::endl;
        keywords.erase(keywords.find(key));
    }        
    return buffer.str();
}
std::string SystemKeywordSection(std::map<std::string,std::string> &keywords)
{
    std::stringstream buffer;
    buffer.str("");
    
    buffer << "# Inputs " << std::endl;
    buffer << tc_input_keyword_line(keywords,"prmtop");
    buffer << tc_input_keyword_line(keywords,"coordinates");
    buffer << tc_input_keyword_line(keywords,"qmindices");
    buffer << tc_input_keyword_line(keywords,"charge");
    buffer << tc_input_keyword_line(keywords,"spinmult");
    buffer << std::endl;
    
    buffer << "# Methods " << std::endl;
    buffer << tc_input_keyword_line(keywords,"method");
    buffer << tc_input_keyword_line(keywords,"basis");
    buffer << std::endl;
    
    return buffer.str();
}
std::string CalcTypeKeywordSection(std::map<std::string,std::string> &keywords)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "# Calculation Settings " << std::endl;
    buffer << tc_input_keyword_line(keywords,"run");
    buffer << tc_input_keyword_line(keywords,"new_minimizer");
    buffer << tc_input_keyword_line(keywords,"min_coordinates");
    buffer << tc_input_keyword_line(keywords,"mincheck");
    buffer << tc_input_keyword_line(keywords,"nstep");
    buffer << tc_input_keyword_line(keywords,"min_maxallowedstep");
    buffer << tc_input_keyword_line(keywords,"timestep");
    buffer << tc_input_keyword_line(keywords,"mdbc");
    buffer << tc_input_keyword_line(keywords,"orbitalswrtfrq");
    buffer << std::endl;

    return buffer.str();

}
std::string ConvergenceKeywordSection(std::map<std::string,std::string> &keywords)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "# Convergence Criteria " << std::endl;
    buffer << tc_input_keyword_line(keywords,"threall");
    buffer << tc_input_keyword_line(keywords,"convthre");
    buffer << tc_input_keyword_line(keywords,"precision");
    buffer << tc_input_keyword_line(keywords,"maxit");
    buffer << tc_input_keyword_line(keywords,"scf");
    buffer << std::endl;

    return buffer.str();

}
std::string ComputingKeywordSection(std::map<std::string,std::string> &keywords)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "# Computing Resources Information " << std::endl;
    buffer << tc_input_keyword_line(keywords,"gpus");
    buffer << tc_input_keyword_line(keywords,"gpumem");
    buffer << tc_input_keyword_line(keywords,"scrdir");
    buffer << std::endl;
    return buffer.str();
}
std::string CASSCFKeywordSection(std::map<std::string,std::string> &keywords)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "# CASSCF Keywords " << std::endl;
    buffer << tc_input_keyword_line(keywords,"casscf");
    buffer << tc_input_keyword_line(keywords,"alphacas");
    buffer << tc_input_keyword_line(keywords,"alpha");
    buffer << tc_input_keyword_line(keywords,"castarget");
    buffer << tc_input_keyword_line(keywords,"castargetmult");
    buffer << tc_input_keyword_line(keywords,"cassinglets");
    buffer << tc_input_keyword_line(keywords,"casscfmacromaxiter");
    buffer << tc_input_keyword_line(keywords,"casscfmaxiter");
    buffer << tc_input_keyword_line(keywords,"casscftrustmaxiter");
    buffer << tc_input_keyword_line(keywords,"casscfmicroconvthre");
    buffer << tc_input_keyword_line(keywords,"casscfmacroconvthre");
    buffer << tc_input_keyword_line(keywords,"casscfconvthre");
    buffer << tc_input_keyword_line(keywords,"casscfenergyconvthre");
    buffer << tc_input_keyword_line(keywords,"cpsacasscfmaxiter");
    buffer << tc_input_keyword_line(keywords,"cpsacasscfconvthre");
    buffer << tc_input_keyword_line(keywords,"closed");
    buffer << tc_input_keyword_line(keywords,"active");
    buffer << tc_input_keyword_line(keywords,"casguess");
    buffer << tc_input_keyword_line(keywords,"cascharges");
    buffer << tc_input_keyword_line(keywords,"ci_solver");
    buffer << std::endl;
    return buffer.str();
}
std::string RemainingKeywordSection(std::map<std::string,std::string> &keywords)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "# Uncategorized Keywords " << std::endl;
    for (std::map<std::string,std::string>::iterator iter=keywords.begin(); iter != keywords.end(); iter++)
    {
        buffer << tc_input_keyword_line(keywords,iter->first);
    }
    buffer << std::endl;
    return buffer.str();
}

void Write_TC_Input(std::map<std::string,std::vector<std::string>> &flags, std::map<std::string,std::string> &keywords)
{
    // identify calculation type
    get_calc_type(flags);
    // identify maximum keyword length
    get_max_keyword_length(flags);
    // parse all the keywords from defaults and command line into a single set.
    generate_full_keyword_set(flags,keywords);

    // Prepare working directory
    std::string job_dir = MakeIterativeDirectoryName("AutoQuantum", 4);
    move_to_jobdir(keywords,job_dir);
    fs::current_path(job_dir);
    normal_log("Copied relevant input files to " + job_dir);

    std::ofstream ofile(TC_FILENAME,std::ios::out);
    
    // Check that file successfully opened for writing.
    if (! ofile.is_open())
    {
        error_log("Unable to open " + TC_FILENAME + " for writing.  Check permissions", 1);
    }
    
    // Include comment line for input file that identifies it was generated with AutoQuantum.
    ofile << "# Generated by AutoQuantum for use with TeraChem on " << GetTimeAndDate() << std::endl;
    
    // Write all keywords and their associated values to the input file.
    ofile << SystemKeywordSection(keywords);
    ofile << CalcTypeKeywordSection(keywords);
    ofile << ConvergenceKeywordSection(keywords);
    ofile << ComputingKeywordSection(keywords);
    if (USE_CASSCF)
    {
        ofile << CASSCFKeywordSection(keywords);
    }
    ofile << RemainingKeywordSection(keywords);
    
    ofile << std::endl;
    
    // Close the TeraChem input file.
    ofile.close();
}

void SubmitSlurmJob(std::map<std::string,std::string> keywords)
{
    std::string buffer=R"(#!/bin/bash
#SBATCH -t 120:00:00
#SBATCH -q )" + (std::string)DEFAULT_SLURM_GPU_JOB_QUEUE + R"(
#SBATCH -p )" + (std::string)DEFAULT_SLURM_GPU_JOB_PARTITION + R"(
#SBATCH -N 1
#SBATCH -n 3
#SBATCH -o slurm_)" + TC_OUTFILE + R"(
#SBATCH -e slurm_)" + TC_ERRFILE + R"(
#SBATCH --job-name AutoQuantum_TC_)" + CALC_TYPE + R"(
#SBATCH --gres=gpu:1
#SBATCH --mem=20GB

module load )" + (std::string)DEFAULT_TERACHEM_MODULE + R"(
cp ./* /tmp/
cd /tmp/
terachem -i )" + TC_FILENAME + R"( 1> )" + TC_OUTFILE + R"( 2> )" + TC_ERRFILE + R"(
cp -r ./* $SLURM_SUBMIT_DIR/

)";
    std::ofstream ofile("AutoQuantum_TC_Job.sh",std::ios::out);
    if (! ofile.is_open())
    {
        error_log("Unable to open batch submission script for writing. ",1);
    }
    ofile << buffer;
    ofile.close();
    silent_shell("sbatch AutoQuantum_TC_Job.sh");
}

void RunTeraChem()
{
    // Initialize buffer.
    std::stringstream buffer;
    buffer.str("");

    // See if a TeraChem module has already been loaded, then add to buffer if not.
    std::string module_list = GetSysResponse("module list");
    if (module_list.find("Terachem") == std::string::npos)
    {
        buffer << "module load " << DEFAULT_TERACHEM_MODULE << "; ";
    }
    buffer << "terachem -i " << TC_FILENAME << " 1> " << TC_OUTFILE << " 2> " << TC_ERRFILE;

    debug_log(buffer.str());
    silent_shell(buffer.str());    
}