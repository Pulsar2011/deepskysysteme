//
//  OptionManager.h
//  Calibration
//
//  Created by GILLARD William on 30/04/15.
//  Fork from DeepSkyLib by GILLARD William on 12/06/22.
//  Centre de Physic des Particules de Marseille
//	Copyright (c) 2015, All rights reserved
//

#ifndef __DSL__OptionManager__
#define __DSL__OptionManager__

#include<vector>
#include<map>
#include<string>
#include<algorithm>
#include<cstdio>
#include <stdexcept>

#if __cplusplus >= 201703L
#include<filesystem>
namespace fs = std::filesystem;
#else
#include <boost/filesystem.hpp>
namespace fs boost::filesystem;
#endif

namespace DSL
{
#pragma mark - DOption class definition
    /**
     *  @namespace DSS
     *  @brief DeepSkyTools namespace
     *  @details DSS namespace stand for DeepSkySys. This namespace regroup a collection libraies that help to interact with system.
     */
    class DOption
    {

#ifdef _gTEST_version_
        friend class gTEST_manager;
        FRIEND_TEST(gTEST_manager, constructor);
        FRIEND_TEST(gTEST_manager, cleaner);
        FRIEND_TEST(gTEST_manager, add_option);
        FRIEND_TEST(gTEST_manager, add_param);
        FRIEND_TEST(gTEST_manager, add_dscrp);
#endif  
        /**
         * @class DSS::DOption OptionManager.h "DSTCore/OptionManager.h"
         * @author GILLARD William
         * @version 1.0
         * @date 30/04/2015
         * @details Manage optional parameters of analyses framworks. This class should be called by the main programs to reads and initialize the analyses optional parameters. The object created is a singleton and is therefore unique for each analyses. Providing the Qoption.h header file is included, Qoption singleton can be reads in any analyses codes referenced by the main programs.
         */
        
    private:
        std::vector<std::string> filelist;  ///< List of input files
        std::string logfile;                ///< Path and name of the logfile to be used when redirecting standard output to a logfile
        
        int16_t OUTfd, ERRfd;
        fpos_t OUTpos, ERRpos;

        bool cleanLog(const std::string) const;
        
        
        std::string prg_description;                                  ///< The program description that will be display when the '-h' or '--help' option will be given.
        std::map<unsigned int, std::vector<std::string> > prg_option; ///< List of optional parameter that the programme can accept
        std::map<unsigned int, std::vector<std::string> > prg_param;  ///< List of required parameters that the program need to run
        
    protected:
        
        std::string output_file;            ///< Path and name of the output if any output are generated
        int16_t level;                      ///< Debug level from 0 (no debug) to 10 (max debug information)
        
        void Usage();                       ///< Print program usage onto the standard output
        void PrintSynopsys();
        void PrintOptions();
        
        void AddOption(std::string, std::string, std::string); ///< Add optional parameter
        void AddOption(std::string, std::string);              ///< Add optional parameter
        void AddOption(unsigned int, std::string, std::string, std::string); ///< Add optional parameter
        void AddOption(unsigned int, std::string, std::string);              ///< Add optional parameter
        void AddParam (std::string, std::string);              ///< Add required parameter
        void AddDescription(std::string);                      ///< Add programme description
        
        void format_string(std::string &, bool isSynopsis = false);        ///< Format string befor printing on standard display
        
        /**
         *  @details Virtual methods to reads input parameters and identify parameters require for an analyses.
         *  @param index : position of the current input parameter in the parameters list
         *  @param argv  : input parameter's list
         *  @param argc  : max number of parameters
         *  @return The methods should return true if the parameters is defined. Unknow parameters should return false.
         */
        virtual bool UserParameters(std::vector<std::string>::const_iterator& it, const std::vector<std::string>& arg) = 0;
        
    public:
        
        DOption():filelist(),logfile(),prg_description(),prg_option(),prg_param(),output_file(),level(0)
        {

#if defined __linux__ && __cplusplus >= 201103L
	        ERRpos.__pos    = -1;
	        OUTpos.__pos    = -1;
#else
	        ERRpos    = -1;
            OUTpos    = -1;
#endif
            
            prg_description = std::string("DOption class reads and manage optional parameters given as input to the main executable. DOption allows to parse those parameters to analyses chain. DOption is a singleton class that can be overwritten to customize optional parameters specific to a given analyses. This can be done by overwritting the UserParameters function to initiate the parameters. In that case, \033[31m do not forget to add your optional parameter and required parameter to the DOption class in order that PrintSynopsys() as well as PrintOption() are able to parse specific documentationto the users\033[0m.");
            
            AddOption(0,"-o/--output","FileName", "Path and name of the output file produced by the analyses, if any will be created.");
            AddOption(664,"-l/--log","FileName", "Redirect standard output to a logfile defined by \033[31mFileName\033[0m parameter value.");
            AddOption(665,"-d/--debug","int","Activate high level debug.");
            AddOption(666,"-h/--help","Show this message.");
        };						///< Default constructor;
        
        /**
         *  @details Default constructor. Private to avoid deletion from random stangers.
         */
        virtual ~DOption();
        
        bool InitParameters(int argc, char** argv);
        
        /**
         *  @brief List files to be analysed
         *  @details List all files given in input that will analysed.
         *  @return List of files.
         */
        inline const std::vector<std::string> GetFiles() const {return filelist;}
        
        /**
         *  @brief Get the path and name of the output file.
         *  @details Get the path and name of the output file.
         *  @return output file name.
         */
        inline const std::string GetOutput() const {return output_file;}
        
        /**
         *  @brief Get debug mode status.
         *  @details Check either or not debug option have been activated.
         *  @return this\f$\rightarrow\f$debug.
         */
        inline const int16_t& DebugLevel() const {return level;}
        inline const bool Debug() const {return (level>0);}
        
        virtual void SetLogfile(std::string filename){logfile = filename;}
        inline  std::string Logfile() const {return logfile;}
        
        static std::string prg_name;        
    };

    bool mkdir(const std::string&);
}

#endif /* defined(__Calibration__OptionManager__) */
