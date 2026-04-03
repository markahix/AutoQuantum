
#include "utilities.h"

// LOGGING FUNCTIONS - Normal, Error, Debug-flagged.
void error_log(std::string message, int exit_code)
{
    std::cerr << message << std::endl;
    exit(exit_code);
}
void normal_log(std::string message)
{
    std::cout << message << std::endl;
}
void debug_log(std::string message)
{
    if (DEBUG)
    {
        std::cout << "DEBUG: " << message << std::endl;
    }
}
void splash_screen()
{
    std::cout << "###########################" << std::endl;
    std::cout << "# AutoQuantum w/ TeraChem #" << std::endl;
    std::cout << "###########################" << std::endl;
}
std::string GetTimeAndDate()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer.str("");
    buffer << std::put_time(&tm, "%Y.%m.%d %H:%M:%S");
    return buffer.str();
}
void PrintUsage()
{
    std::string usagetext=R"(
#############################
# AutoQuantum with TeraChem #
#############################
When using AutoQuantum, the command line must follow this format:
    autoquantum --<flag> [<value>]
such that all flags have the '--' to differentiate from a potential 
single '-' such as seen when the formal charge is '-1'.  Some flags
do not require added values, however these are specific to AutoQuantum.
    --spe
    --bomd
    --freq
    --opt
The '--debug' flag will enable more verbose logging to the terminal, 
which may be useful if you're having trouble.  The '--spe', '--opt', 
'--freq', and '--bomd' flags are the major calculation types that
AutoQuantum is currently designed to handle.  All other flags must be 
immediately followed by a corresponding value.  During parsing, these 
flag/value pairs are compared to known TeraChem keyword defaults and 
updated before generating the input file.  Thus, if you want a purely
default TeraChem single-point energy calculation of a neutral singlet
molecule, you can use a simple command:
    autoquantum --spe --coordinates <molecule.xyz>

)";
    normal_log(usagetext);
}

// System Commands
void silent_shell(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
}
void silent_shell(std::string cmd)
{
    silent_shell(cmd.c_str());
}
std::string GetSysResponse(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}
std::string GetSysResponse(std::string cmd)
{
    return GetSysResponse(cmd.c_str());
}
bool CheckProgAvailable(const char* program)
{
        std::string result;
        std::stringstream cmd;
        cmd.str("");
        cmd << "which " << program;
        debug_log(cmd.str());
        result=GetSysResponse(cmd.str().c_str());
        if (result.empty())
        {
            std::cout << "Missing program: " << program << std::endl;
            return false;
        }
        return true;
}
bool CheckProgAvailable(const char* program, const char* module)
{
        std::string result;
        std::stringstream cmd;
        cmd.str("");
        cmd << "module load " << module << "; which " << program;
        debug_log(cmd.str());
        result=GetSysResponse(cmd.str().c_str());
        if (result.empty())
        {
            std::cout << "Missing program: " << program << std::endl;
            return false;
        }
        return true;
}
bool CheckProgAvailable(std::string program)
{
    return CheckProgAvailable(program.c_str());
}
bool CheckProgAvailable(std::string program, std::string module)
{
    return CheckProgAvailable(program.c_str(),module.c_str());
}

// File Handling
void write_to_file(std::string inputfilename, std::string buffer)
{
    std::ofstream outFile;
    outFile.open(inputfilename,std::ios::out);
    outFile << buffer;
    outFile.close();
}
void append_to_file(std::string inputfilename, std::string buffer)
{
    if (!fs::exists(inputfilename))
    {
        write_to_file(inputfilename,"");
    }
    std::ofstream outFile;
    outFile.open(inputfilename,std::ios::app);
    outFile << buffer;
    outFile.close();
}
std::string LastLineOfFile(std::string filename)
{
    std::ifstream fin;
    fin.open(filename);
    std::string line;
    std::vector<std::string> lines_in_order;
    while (std::getline(fin, line))
    {   
        if ( !is_empty(line.c_str()) )
        {
            lines_in_order.push_back(line);
        }
    }
    fin.close();
    return lines_in_order[lines_in_order.size()-1];
}
int count_lines_in_file(std::string filename)
{
    std::string dummy;
    int count = 0;
    std::ifstream file(filename);
    while (getline(file,dummy))
    {
        count ++;
    }
    return count;
}
void compress_and_delete(std::string directory)
{
    std::stringstream buffer;
    buffer.str("");
    buffer << "tar -czvf " << directory << ".tar.gz "<< directory << "/ && rm -r " << directory << "/";
    silent_shell(buffer.str().c_str());
}
std::vector<std::string> sort_files_by_timestamp(std::string directory,std::string pattern)
{
    std::set <fs::path> sort_by_name;
    for (fs::path p : fs::directory_iterator(directory))
    {
        if (p.extension() == pattern) 
        {
            sort_by_name.insert(p);
        }    
    }
    std::vector<std::string> file_list={};

    for (auto p : sort_by_name)
    {
        std::cout << p << std::endl;
        file_list.push_back(p);
    }
    
    return file_list;
}
std::string MakeIterativeDirectoryName(std::string dir_base, int num_zeros)
{
    std::stringstream new_dir_name;
    int num_dirs_found = 0;
    do 
    {
        num_dirs_found++;
        new_dir_name.str("");
        new_dir_name << dir_base << "."<< std::setw(num_zeros) << std::setfill('0') << num_dirs_found << "/";
    } while (fs::is_directory(new_dir_name.str()));
    if (fs::create_directory(new_dir_name.str()))
    {
        return new_dir_name.str();
    }
    else
    {
        error_log("Unable to create iterative directory.",1);
    }
    return "./";
}

// String Utilities
int is_empty(const char *s) 
{
    while (*s != '\0') 
    {
        if (!isspace((unsigned char)*s))
        return 0;
        s++;
    }
    return 1;
}
std::string string_between(std::string incoming, std::string first_delim, std::string second_delim)
{
    unsigned first = incoming.find(first_delim);
    if (first == std::string::npos)
    {
        return incoming;
    }
    incoming = incoming.substr(first+1,incoming.size() - first -1);

    unsigned last = incoming.find(second_delim);
    if (last == std::string::npos)
    {
        return incoming.substr(0, incoming.size());
    }
    return incoming.substr(0, last);
}
std::vector<std::string> split_string(std::string incoming, std::string delim)
{
    std::vector<std::string> chunks = {};
    unsigned int pos = incoming.find(delim);
    do
    {
        std::string piece = incoming.substr(0,pos);
        if (!is_empty(piece.c_str()))
        {
            chunks.push_back(piece);
        }       
        incoming = incoming.substr(pos+1,incoming.size()-pos-1);
        pos = incoming.find(delim);
    } while (pos != std::string::npos);
    chunks.push_back(incoming.substr(0,incoming.find_first_of(delim)));
    return chunks;
}

// Command Line Parser
void parse_command_line_arguments(std::map<std::string,std::vector<std::string>> &flags, int argc, char** argv)
{
    std::string key = "executable";
    for (int i = 0; i < argc; i++)
    {
        std::string flag_item(argv[i]);
        if (flag_item.substr(0,2) == "--")
        {
            key = flag_item.substr(2,flag_item.size()-2);
            flags[key] = {};
        }
        else
        {
            flags[key].push_back(flag_item);
        }
    }
    // clear executable flag
    flags.erase(flags.find("executable"));
    // Check for debug mode
    check_debug_mode(flags);

    if (DEBUG) // if in debug mode, report full list of keys and values from the command line parser.
    {
        for (std::map <std::string, std::vector<std::string>>::iterator iter = flags.begin(); iter != flags.end(); iter++)
        {
            std::string key = iter->first;
            std::vector<std::string> val_array = iter->second;

            std::stringstream buffer;
            buffer.str("");
            buffer << "(Key) " << key << " : ";
            buffer << "(Values) ";
            if (val_array.size() > 1)
            {
                for (unsigned int i = 0; i < val_array.size(); i++)
                {
                    buffer << val_array[i] << ", ";
                }
            }

            else if (val_array.size() == 1)
            {
                buffer << val_array[0] ;
            }
            else
            {
                buffer << " <EMPTY FLAG> ";
            }
            debug_log(buffer.str());
        }
    }
    debug_log("End of debug print statements.");
}

void check_debug_mode(std::map<std::string,std::vector<std::string>> &flags)
{
    if (flags.count("debug") > 0)
    {
        DEBUG = true;
        flags.erase(flags.find("debug"));
    }
    if (flags.count("DEBUG") > 0)
    {
        DEBUG = true;
        flags.erase(flags.find("DEBUG"));
    }
    if (flags.count("dryrun") > 0)
    {
        DRYRUN=true;
        flags.erase("dryrun");
    }
    if (flags.count("DRYRUN") > 0)
    {
        DRYRUN=true;
        flags.erase("DRYRUN");
    }

}