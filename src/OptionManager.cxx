//
//  OptionManager.cxx
//
//  Created by GILLARD William on 30/04/15.
//  Fork from DeepSkyLib by GILLARD William on 12/06/22.
//  Centre de Physic des Particules de Marseille
//  Licensed under CC BY-NC 4.0
//  You may share and adapt this code with attribution, 
//  but not for commercial purposes.
//  Licence text: https://creativecommons.org/licenses/by-nc/4.0/

#include <DSTCore/OptionManager.h>

#include<unistd.h>
#include<fstream>
#include<iostream>

// namespace DSS stand for DeepSkySys
namespace DSL
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
            for(std::map<size_t, DParam >::iterator it = prg_option.begin(); it != prg_option.end(); it++)
                it->second.clear();
        
        prg_option.clear();
        
        if(prg_param.size() > 1)
            for(std::map<size_t, DParam >::iterator it = prg_param.begin(); it != prg_param.end(); it++)
                it->second.clear();
        
        prg_param.clear();
        prg_description.clear();
    }
    
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt_pos: Printing order of the optional parameters
     * @param opt: the option string
     * @param type: The list of optional parameter types, if needed
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(const size_t& opt_pos, const std::string& opt, const std::initializer_list<DParam::fparameter>& type, const std::string& info)
    {        
        prg_option.insert(std::pair<size_t, DParam >(opt_pos,DParam(opt,type,info,true)));
    }

    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt_pos: Printing order of the optional parameters
     * @param opt: the option string
     * @param type: The optional parameter type, if needed
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(const size_t& opt_pos, const std::string& opt, const DParam::fparameter& type, const std::string& info)
    {        
        AddOption(opt_pos,opt,{type},info);
    }
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt_pos: Printing order of the optional parameters
     * @param opt: the option string
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(const size_t& opt_pos, const std::string& opt, const std::string& info)
    {
        AddOption(opt_pos, opt, {DParam::fparameter::none}, info);
    }
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt: the option string
     * @param type: The list of optional parameter types, if needed
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(const std::string& opt, const std::initializer_list<DParam::fparameter>& type, const std::string& info)
    {
        AddOption(prg_option.size()+1,opt,type,info);
    }

    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt: the option string
     * @param type: The optional parameter type, if needed
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(const std::string& opt, const DParam::fparameter& type, const std::string& info)
    {
        AddOption(opt,{type},info);
    }
    
    /**
     * @details Add an optional parameter to the list of optional parameters that this program can accept.
     * @param opt: the option string
     * @param info: The description of the optional parameters
     */
    void DOption::AddOption(const std::string& opt, const std::string& info)
    {
        AddOption(opt,{DParam::fparameter::none}, info);
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
    void DOption::AddParam(const std::string& opt, const std::string& info)
    {
        prg_param.insert(std::pair<size_t, DParam >(prg_param.size()+1, DParam(opt,DParam::fparameter::none,info,true)));
    }
    /**
     * @details Printout program usage. This function is purelly virtual and should be replaced by users to adapt the printout to their code.
     */
    void DOption::Usage()
    {
        PrintSynopsys();
        
        printf("\n\033[22;34mOPTIONAL PARAMETERS\n");
        
        PrintOptions();
        std::cout<<std::endl<<"\033[22;32m***************************************************************************************************\033[0m"<<std::endl<<std::endl<<std::endl;
    }
    
    /**
     *  @details Print on standard output the description of the program and on its uses.
     */
    void DOption::PrintSynopsys()
    {
        if(prg_name.size() < 1)
            prg_name += "DST::DOption";
        
        std::cout<<std::endl<<std::endl<<"\033[22;31m"<<prg_name<<" : \033[22;34mHelp"<<std::endl;
        std::cout<<"\033[22;32m***************************************************************************************************\033[0m"<<std::endl<<std::endl;
        format_string(prg_description,true);
        std::cout<<prg_description<<std::endl;
        std::cout<<std::endl<<"\033[22;32m***************************************************************************************************\033[0m"<<std::endl;
        
        size_t kSize = 0;
        std::string this_synopsis = std::string("\033[0m ");
        for(std::map<size_t, DParam >::const_iterator it=prg_option.begin(); it != prg_option.end(); it++)
        {
            this_synopsis += "[\033[22;32m"  + it->second.name() + "\033[0m";
            std::string opt_val;
            for(std::vector<DParam::fparameter>::const_iterator it2 = it->second.optType().cbegin(); it2 != it->second.optType().cend(); ++it2)
            {
                if(it2==it->second.optType().cbegin())
                    opt_val = " ";

                opt_val += "\033[2;31m" + DParam::what(*it2) + "\033[0m";
                if(it2 != it->second.optType().cend()-1)
                    opt_val +="\033[0m ";

            }

            this_synopsis += opt_val + "] ";
            kSize += opt_val.size();

            if(kSize > 60)
            {
                this_synopsis += "\n"+std::string(prg_name.size()+3,' ');

                kSize = 0;
            }
        }
        
        for(std::map<size_t, DParam >::const_iterator it=prg_param.begin(); it != prg_param.end(); it++)
        {
            this_synopsis += it->second.name() + " ";
        }
        
        //format_string(this_synopsis);
        
        std::cout<<"  \033[31m"<<prg_name<<this_synopsis<<std::endl<<std::endl;
        
        std::cout<<std::endl<<"\033[22;33mREQUIRED PARAMETERS\033[0m"<<std::endl;
        
        for(std::map<size_t,DParam>::const_iterator it=prg_param.begin(); it != prg_param.end(); it++)
        {
            std::cout<<"  "<<it->second.name()<<"   "<<it->second.paramString()<<std::endl;
            std::cout<<it->second.description()<<std::endl;
        }
        
    }
    
    /**
     *  @details Print on the standard output the different options availaible to the user and their description.
     */
    void DOption::PrintOptions()
    {
        for(std::map<size_t,DParam >::const_iterator it=prg_option.cbegin(); it != prg_option.cend(); it++)
        {
            std::string opt_name = "  \033[22;32m" + it->second.name() + "\033[0m";
            std::string opt_val  = std::string();
            for(std::vector<DParam::fparameter>::const_iterator it2 = it->second.optType().cbegin(); it2 != it->second.optType().cend(); ++it2)
            {
                    opt_val += "\033[2;31m" + DParam::what(*it2) + "\033[0m";
                    if(it2 != it->second.optType().cend()-1)
                        opt_val +="\033[0m ";
            }

            std::cout<<opt_name<<" "<<opt_val<<std::endl;
            
            std::string opt_info = "  "+it->second.description();
            format_string(opt_info);
            
            std::cout<<opt_info<<std::endl<<std::endl;
        }
    }
    
    void DOption::format_string(std::string &str, bool isSynopsis)
    {
        int length = 80;
        
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
        std::vector<std::string> arg;
        int n = 0;
        
        for(n = 0; n < argc ; n++)
        {
            arg.push_back(std::string(argv[n]));
            
            if(arg[arg.size()-1][0] == '-' || arg[arg.size()-1][0] == '+')
            {
                std::transform(arg[arg.size()-1].begin(), arg[arg.size()-1].end(), arg[arg.size()-1].begin(), ::tolower);
            }
            
            if(!arg[arg.size()-1].compare("-h") || !arg[arg.size()-1].compare("--help"))
            {
                Usage();
                return false;
            }
        }

        std::cout<<"\033[22;34m>>>>>>>>>> Program: \033[0m"<<prg_name<<std::endl;
        
        std::vector<std::string>::const_iterator it=arg.cbegin();
        while(it != arg.cend())
        {
            //-- CONVERT TO LOWER CASE IF NEEDED
            if((*it)[0] != '-' && (*it)[0] != '+' )
            {
                if(it != arg.cbegin())
                {
                    filelist.push_back(std::string(*it));
                    std::cout<<"   \033[34m• Add input file \033[0m"<<(--filelist.end())->c_str()<<std::endl;
                }
                it++;
                continue;
            }
            
            if(!it->compare("-o") || !it->compare("--output"))
            {
                if(std::next(it) != arg.cend()){it++;}else{it++; continue;}               
                output_file += std::string(*it);
                std::cout<<"   \033[34m• Set output to \033[0m"<<output_file.c_str()<<std::endl;
            }
            else if(!it->compare("--log"))
            {
                if(std::next(it) != arg.cend()){it++;}else{it++; continue;}
                logfile += std::string(*it);
                std::cout<<"   \033[34m• Redirect output to file \033[0m"<<logfile.c_str()<<std::endl;
            }
            else if(!it->compare("--debug") ||
                    !it->compare("-d"))
            {
                if(std::next(it) != arg.cend()){it++;}else{it++; continue;}
                if (std::string(*it).find('-') != std::string::npos)
                {
                    it--;
                    level += 1;
                }
                else if(std::string(*it).find_first_not_of("0123456789") != std::string::npos)
                {
                    it--;
                    level += 1;
                }
                else
                {
                    level += static_cast<int16_t>(stoi(*it));
                }
                std::cout<<"   \033[34m• Avtivate debug mode @ level \033[0m"<<level<<std::endl;
            }
            else
            {
                if(!UserParameters(it,arg))
                {
                    std::string err_str = std::string("\033[43;36m[");
                    err_str+=prg_name;
                    err_str += std::string("]\033[1;0m Unknown parameter ");
                    err_str += *it;
                    err_str +=std::string("\033[0m");  
                    throw std::runtime_error(err_str);
                }
            }
            
            it++;
        }
        
        // Only enforce the positional-file requirement when the subclass explicitly
        // declared required parameters via AddParam(). Programs that work entirely
        // through keyword options (no positional file arguments) should simply not
        // call AddParam(), and this check will be skipped automatically.
        if(prg_param.size() > 0 && filelist.size() < prg_param.size())
        {
            std::string err_str = std::string("\033[43;36m[");
            err_str+=prg_name;
            err_str += std::string("]\033[1;0m required parameters are missing.");
            err_str +=std::string("\033[0m");  
            throw std::runtime_error(err_str);
        }
        
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

    /**
     *  @brief Create directory.
     *  @details Check directory dir exists. If it does not exists, it create the directory.
     *  @param dir: directory to create.
     *  @return true if their isn't any error while creating directory.
     */
    bool mkdir(const std::string& dir)
    {
#if __cplusplus >= 201703L
        if(fs::exists(dir)) return true;
        return fs::create_directories(dir);
#else
        fs::path path(dir);
        if(fs::exists(path)) return true;
        
        return fs::create_directory(path);
#endif
    }
}
