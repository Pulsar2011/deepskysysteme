//
//  OptionManager.cxx
//
//  Created by GILLARD William on 30/04/15.
//  Centre de Physic des Particules de Marseille
//	Copyright (c) 2015, All rights reserved
//

#include <DSTCore/OptionManager.h>

#include<unistd.h>
#include<fstream>
#include<iostream>

// namespace DSS stand for DeepSkySys
namespace DSS
{
    std::string DOption::prg_name = std::string("");
    
    /**
     *  @brief Default destructor
     */
    DOption::~DOption()
    {
#if defined __linux__ && __cplusplus >= 201103L
        if(logfile.size() > 4 && OUTpos.__pos >= 0 && ERRpos.__pos >= 0)
        {
            fflush(stdout);			//<< All remaining data in buffer get send to stdout
            dup2(OUTfd,fileno(stdout));		//<< we clone the original stdout over the current stdout
            close (OUTfd);			//<< we close the cloned file
            fsetpos(stdout, &OUTpos);		//<< we restore our position in stdout
            
            fflush(stderr);			//<< All remaining data in buffer get send to stdout
            dup2(ERRfd,fileno(stderr));		//<< we clone the original stdout over the current stdout
            close (ERRfd);
            fsetpos(stderr, &ERRpos);		//<< we restore our position in stdout//<< we restore our position in stdout
        }
#else
	if(logfile.size() > 4 && OUTpos >= 0 && ERRpos >= 0)
	  {
            fflush(stdout);                     //<< All remaining data in buffer get send to stdout 
            dup2(OUTfd,fileno(stdout));         //<< we clone the original stdout over the current stdout
            close (OUTfd);                      //<< we close the cloned file
            fsetpos(stdout, &OUTpos);           //<< we restore our position in stdout

            fflush(stderr);                     //<< All remaining data in buffer get send to stdout
            dup2(ERRfd,fileno(stderr));         //<< we clone the original stdout over the current stdout
            close (ERRfd);
            fsetpos(stderr, &ERRpos);           //<< we restore our position in stdout//<< we restore our position in stdout
	  }
#endif
        
        if(prg_option.size() > 1)
            for(std::map<unsigned int, std::vector<std::string> >::iterator it = prg_option.begin(); it != prg_option.end(); it++)
                it->second.clear();
        prg_option.clear();
        
        if(prg_param.size() > 1)
            for(std::map<unsigned int, std::vector<std::string> >::iterator it = prg_param.begin(); it != prg_param.end(); it++)
                it->second.clear();
        prg_param.clear();
        
        prg_description.clear();
    }
    
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt_pos: Printing order of the optional parameters
     * @param opt: the option string
     * @param type: The optional parameter type, if needed
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(unsigned int opt_pos, std::string opt, std::string type, std::string info)
    {
        std::vector<std::string> p_opt;
        p_opt.push_back(opt);
        p_opt.push_back(type);
        p_opt.push_back(info);
        
        prg_option.insert(std::pair<unsigned int, std::vector<std::string> >(opt_pos,p_opt));
    }
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt_pos: Printing order of the optional parameters
     * @param opt: the option string
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(unsigned int opt_pos, std::string opt, std::string info)
    {
        AddOption(opt_pos, opt, "", info);
    }
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt: the option string
     * @param type: The optional parameter type, if needed
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(std::string opt, std::string type, std::string info)
    {
        AddOption(prg_option.size()+1,opt,type,info);
    }
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt: the option string
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(std::string opt, std::string info)
    {
        AddOption(opt, "", info);
    }
    
    /**
     * @details Add programe description.
     * @param info: The programme description
     */
    void DOption::AddDescription(std::string info)
    {
        prg_description.clear();
        prg_description += info;
        
        format_string(prg_description);
    }
    
    /**
     * @details Add a required parameters to the list of parameters that are neede for the program to run properly.
     * @param opt: the option string
     * @param info: The description of the optional parameters
     */
    void DOption::AddParam(std::string opt, std::string info)
    {
        std::vector<std::string> p_need;
        p_need.push_back(opt);
        p_need.push_back("  "+info);
        
        prg_param.insert(std::pair<unsigned int, std::vector<std::string> >(prg_param.size()+1,p_need));
    }
    /**
     * @details Printout program usage. This function is purelly virtual and should be replaced by users to adapt the printout to their code.
     */
    void DOption::Usage()
    {
        PrintSynopsys();
        
        printf("\n\033[22;31mOPTIONAL PARAMETERS\n");
        
        PrintOptions();
        std::cout<<std::endl<<"\033[22;32m*****************************************************************************************\033[0m"<<std::endl<<std::endl<<std::endl;
    }
    
    /**
     *  @details Print on standard output the description of the program and on its uses.
     */
    void DOption::PrintSynopsys()
    {
        if(prg_name.size() < 1)
            prg_name += "DST::DOption";
        
        std::cout<<std::endl<<std::endl<<"\033[22;31m"<<prg_name<<" : \033[22;34mHelp"<<std::endl;
        std::cout<<"\033[22;32m*****************************************************************************************\033[0m"<<std::endl<<std::endl;
        std::cout<<prg_description<<std::endl;
        std::cout<<std::endl<<"\033[22;32m*****************************************************************************************\033[0m"<<std::endl;
        
        std::string this_synopsis = std::string("\033[0m ");
        for(std::map<unsigned int, std::vector<std::string> >::const_iterator it=prg_option.begin(); it != prg_option.end(); it++)
        {
            this_synopsis += "[\033[22;32m" + it->second.at(0) + "\033[0m";
            std::string opt_val;
            if(it->second.at(1).size() > 0)
            {
                opt_val = " \033[2;31m" + it->second.at(1) + "\033[0m";
                
                while(opt_val.find_first_of("|") != std::string::npos)
                    opt_val.replace(opt_val.find_first_of("|"),1,",");
                
                while(opt_val.find_first_of(",") != std::string::npos)
                    opt_val.replace(opt_val.find_first_of(","),1,"\033[0m|\033[2;31m");
                
                while(opt_val.find_first_of("/") != std::string::npos)
                    opt_val.replace(opt_val.find_first_of("/"),1,"\033[0m|\033[2;31m");
            }
            
            this_synopsis += opt_val + "] ";
        }
        
        for(std::map<unsigned int, std::vector<std::string> >::const_iterator it=prg_param.begin(); it != prg_param.end(); it++)
        {
            this_synopsis += it->second.at(0) + " ";
        }
        
        format_string(this_synopsis,true);
        
        std::cout<<"  \033[31m"<<prg_name<<this_synopsis<<std::endl<<std::endl;
        
        std::cout<<std::endl<<"\033[22;31mREQUIRED PARAMETERS\033[0m"<<std::endl;
        
        for(std::map<unsigned int, std::vector<std::string> >::const_iterator it=prg_param.begin(); it != prg_param.end(); it++)
        {
            std::cout<<"  "<<it->second.at(0)<<std::endl;
            std::cout<<it->second.at(1)<<std::endl;
        }
        
    }
    
    /**
     *  @details Print on the standard output the different options availaible to the user and their description.
     */
    void DOption::PrintOptions()
    {
        for(std::map<unsigned int, std::vector<std::string> >::const_iterator it=prg_option.begin(); it != prg_option.end(); it++)
        {
            std::string opt_name = "  \033[22;32m" + it->second.at(0) + "\033[0m";
            std::string opt_val  = std::string();
            if(it->second.size() > 2)
                for(unsigned int n = 1; n < it->second.size()-1; n++)
                    opt_val += "\033[2;31m" + it->second.at(n) + "\033[0m ";
            
            while(opt_val.find_first_of("|") != std::string::npos)
                opt_val.replace(opt_val.find_first_of("|"),1,",");
            
            while(opt_val.find_first_of(",") != std::string::npos)
                opt_val.replace(opt_val.find_first_of(","),1,"\033[0m|\033[2;31m");
            
            while(opt_val.find_first_of("/") != std::string::npos)
                opt_val.replace(opt_val.find_first_of("/"),1,"\033[0m|\033[2;31m");
            
            while(opt_name.find_first_of("/") != std::string::npos)
                opt_name.replace(opt_name.find_first_of("/"),1,"\033[0m "+opt_val+" or\n  \033[22;32m");
            
            std::cout<<opt_name<<" "<<opt_val<<std::endl;
            
            std::string opt_info = "  "+it->second.at(it->second.size()-1);
            format_string(opt_info);
            
            std::cout<<opt_info<<std::endl<<std::endl;
        }
    }
    
    void DOption::format_string(std::string &str, bool isSynopsis)
    {
        int length = 90;
        
        std::string wspace = std::string("  ");
        if(isSynopsis)
        {
            while (wspace.size() < prg_name.size()+3)
                wspace+=std::string(" ");
            
            length += prg_name.size();
        }
        
        size_t k = 0;
        for(std::size_t pos = 1; pos < str.size(); pos++)
        {
            if(!str.compare(pos,1,"\n"))
                k=0;
            
            if(!str.compare(pos,1,"\033[22;32m"))
                k-=10;
            
            k++;
            if(!(k%length))
            {
                if(!str.compare(pos,1," "))
                    str.replace(pos,1,"\n"+wspace);
                else if(!str.compare(pos+1,1," "))
                {
                    ++pos;
                    str.replace(pos,1,"\n"+wspace);
                }
                else if(!str.compare(pos-1,1," "))
                    str.replace(pos-1,1,"\n"+wspace);
                else
                {
                    int k = 1;
                    while(str.compare(pos-k,1," ") && (pos-k) > prg_name.size()+wspace.size() )
                        k++;
                    
                    if(!str.compare(pos-k,1," "))
                        str.replace(pos-k,1,"\n"+wspace);
                    else
                        str.insert(pos-k,"-\n"+wspace);
                }
            }
        }
        
    }
    
    /**
     * @details Reads parameters string and manage options.
     * @param argc : Maximum number of parameter
     * @param argv : Parameter list
     * @return True if the parameters string didn't contains errors.
     */
    bool DOption::InitParameters(int argc, char **argv)
    {
        bool read_status = true;
        std::string arg;
        int n = 0;
        
        for(n = 0; n < argc ; n++)
        {
            arg = std::string(argv[n]);
            
            if(arg[0] == '-' || arg[0] == '+')
            {
                std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
            }
            
            if(!arg.compare("-h") || !arg.compare("--help"))
            {
                Usage();
                return false;
            }
        }
        
        n=0;
        while(n < argc-1)
        {
            n++;
            
            arg = std::string(argv[n]);
            
            //-- CONVERT TO LOWER CASE IF NEEDED
            if(arg[0] == '-' || arg[0] == '+')
            {
                std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
            }
            else
            {
                filelist.push_back(std::string(argv[n]));
                std::cout<<"   \033[34m• Add input file \033[0m"<<(--filelist.end())->c_str()<<std::endl;
                continue;
            }
            
            if(!arg.compare("-o") || !arg.compare("--output"))
            {
                if (++n >= argc) {n--; continue;}
                output_file += std::string(argv[n]);
                std::cout<<"   \033[34m• Set output to \033[0m"<<output_file.c_str()<<std::endl;
            }
            else if(!arg.compare("--log"))
            {
                if (++n >= argc) {n--; continue;}
                logfile += std::string(argv[n]);
                std::cout<<"   \033[34m• Redirect output to file \033[0m"<<logfile.c_str()<<std::endl;
            }
            else if(!arg.compare("--debug") || !arg.compare("-d"))
            {
                if (++n >= argc) {n--; continue;}
                if (std::string(argv[n]).find('-') != std::string::npos)
                {
                    n--;
                    level += 1;
                }
                else if(std::string(argv[n]).find_first_not_of("0123456789") != std::string::npos)
                {
                    n--;
                    continue;
                }
                else
                {
                    level += static_cast<int16_t>(atoi(argv[n]));
                }
                std::cout<<"   \033[34m• Avtivate debug mode @ level\033[0m"<<level<<std::endl;
            }
            else
            {
                if(UserParameters(n, argv, argc))
                    continue;
                
                usr_option += std::string(argv[n]);
            }
        }
        
        if(filelist.size() < 1)
            read_status = false;
        
        if(logfile.size() > 4)
        {
            if(!cleanLog(logfile))
                read_status = false;
            
            std::cout<<"\033[33mRedirect stdout and stderr to "<<logfile<<std::endl<<std::endl;
            
            fflush(stdout);							//<< All remaining data in buffer get send to stdout
            fgetpos(stdout, &OUTpos);				//<< Store current position in stdout
            OUTfd = dup(fileno(stdout));			//<< Clone file description of what is currently stdout
            freopen (logfile.c_str(),"a",stdout);	//<< Redirect std out to a file
            
            fflush(stderr);							//<< All remaining data in buffer get send to stdout
            fgetpos(stderr, &ERRpos);				//<< Store current position in stdout
            ERRfd = dup(fileno(stderr));			//<< Clone file description of what is currently stdout
            freopen (logfile.c_str(),"a",stderr);
        }
        
        return read_status;
    }
    
    /**
     *  @brief Logfile cleaner.
     *  @details Delete content of the logfile to clear log history.
     *  @param _fName: path and name of the logfile to be cleared.
     *  @return true if the 
     has been successfully cleaned. Basically, it will return false if it isn't able to delete the content of the file and this should be considered as an error.
     */
    bool DOption::cleanLog(const std::string _fName) const
    {
        std::fstream haystack (_fName.c_str(), std::fstream::in);
        
        if(!haystack.is_open())
             return true;
        
        haystack.close();
        if(remove(_fName.c_str()))
            return false;
        
        return true;
    }    
}
