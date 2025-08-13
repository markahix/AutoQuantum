#include "utilities.h"
#include "tcinterface.h"

int main (int argc, char** argv)
{
    splash_screen();

    // Variable declarations
    std::map<std::string,std::vector<std::string>> flags = {};
    std::map<std::string, std::string> keywords = {};

    // Command Line Parse, includes checking for debug mode.
    parse_command_line_arguments(flags, argc, argv);
    debug_log("Parsed command line arguments to 'flags' variable.");

    // Write TeraChem input for given flags
    Write_TC_Input(flags, keywords);
    debug_log("Write_TC_Input() completed.");

    // If the dryrun flag was included on the command line, we'll just generate the input files and work directory, but not run the TeraChem calculation.
    if (DRYRUN)
    {
        normal_log("DRYRUN flag was invoked.  Input files have been generated, but TeraChem will not be run at this time.");
        return 0;
    }

    // Check if on warrior, then either submit a TC Job script or run directly.
    std::string hostname = GetSysResponse("hostname");
    if (hostname.find("warrior") != std::string::npos)
    {
        debug_log("Submitting batch job to SLURM queue.");
        // Submit SLURM job.
        SubmitSlurmJob(keywords);
    }
    else
    {
        debug_log("Running terachem directly.");
        // Run TeraChem input file directly.
        RunTeraChem();
    }
    
    // And we're done.
    return 0;
}