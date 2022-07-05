#include "gTest_version.h"
#include <gtest/gtest.h>
#include <OptionManager.h>

TEST(DSS_sytem_tools, create_dir)
{
    ASSERT_EQ(DSS::mkdir(PROJECT_DIR+std::string("/build/tmp")), true);
    ASSERT_EQ(fs::exists(PROJECT_DIR+std::string("/build/tmp")), true);
}

namespace DSS
{
    class gTEST_manager: public DOption
    {
        protected:
            bool UserParameters(std::vector<std::string>::const_iterator& it, const std::vector<std::string>& arg){return true;}
    };

    TEST(gTEST_manager, constructor)
    {
        gTEST_manager manager = gTEST_manager();
        ASSERT_EQ(manager.filelist.size(),0);
        ASSERT_EQ(manager.logfile.size(),0);
        ASSERT_EQ(manager.prg_description.size()>0,true);
        ASSERT_EQ(manager.prg_option.size(),4);
        ASSERT_EQ(manager.prg_param.size(),0);
        ASSERT_EQ(manager.output_file.size(),0);
        ASSERT_EQ(manager.level,0);

#if defined __linux__ && __cplusplus >= 201103L
	    ASSERT_EQ(manager.ERRpos.__pos, -1);
	    ASSERT_EQ(manager.OUTpos.__pos, -1);
#else
	    ASSERT_EQ(manager.ERRpos, -1);
        ASSERT_EQ(manager.OUTpos, -1);
#endif
    }

    TEST(gTEST_manager, cleaner)
    {
        gTEST_manager manager = gTEST_manager();
        manager.SetLogfile(PROJECT_DIR+std::string("/build/tmp/gTest.log"));

        ASSERT_EQ(!manager.logfile.compare(PROJECT_DIR+std::string("/build/tmp/gTest.log")),true);
    }

    TEST(gTEST_manager, add_option)
    {
        gTEST_manager manager = gTEST_manager();
        size_t n_opt = manager.prg_option.size();
        manager.AddOption("-t","test","this is a test option");
        manager.AddOption("-T","this is a test option");

        ASSERT_EQ(  manager.prg_option.size() ,n_opt+2)     <<"Incompatible number of option";

        bool found_t=false;
        bool found_T=false;

        for(std::map<unsigned int, std::vector<std::string> >::const_iterator it=manager.prg_option.cbegin(); it != manager.prg_option.cend(); it++)
        {
            if(!(it->second[0].compare("-t")))
            {
                found_t |= true;
                ASSERT_EQ(  it->second.size()        ,3)     <<"Unconsistent -t option size";
                ASSERT_EQ(!(it->second[0].compare("-t")),true)  <<"Unconsistent -t option parameters";
                ASSERT_EQ(!(it->second[1].compare("test")),true)<<"Unconsistent -t option value";
                ASSERT_EQ(!(it->second[2].compare("this is a test option")),true)<<"Unconsistent -t option description";
            }
            else if(!(it->second[0].compare("-T")))
            {
                found_T |= true;
                ASSERT_EQ(  it->second.size()        ,3)        <<"Unconsistent -T option size";
                ASSERT_EQ(!(it->second[0].compare("-T")),true)  <<"Unconsistent -T option parameters";
                ASSERT_EQ(  it->second[1].size(),0)  <<"Unconsistent -t option value";
                ASSERT_EQ(!(it->second[2].compare("this is a test option")),true)<<"Unconsistent -T option description";
            }
        }
        ASSERT_EQ(found_t,true)<<"Option -t not found";
        ASSERT_EQ(found_T,true)<<"Option -T not found";
    }

    TEST(gTEST_manager, add_param)
    {
        gTEST_manager manager = gTEST_manager();
        size_t n_param = manager.prg_param.size();

        manager.AddParam("new_param","This is a new parameter");

        ASSERT_EQ(  manager.prg_param.size() ,n_param+1)     <<"Incompatible number of parameters";

        bool found_param=false;

        for(std::map<unsigned int, std::vector<std::string> >::const_iterator it=manager.prg_param.cbegin(); it != manager.prg_param.cend(); it++)
        {
            if(!(it->second[0].compare("new_param")))
            {
                found_param |= true;
                ASSERT_EQ(  it->second.size()        ,2)     <<"Unconsistent  parameter size";
                ASSERT_EQ(!(it->second[0].compare("new_param")),true)<<"Unconsistent parameter value";
                ASSERT_EQ(!(it->second[1].compare("  This is a new parameter")),true)<<"Unconsistent parameter description.";
            }
        }
        ASSERT_EQ(found_param,true)<<"Parameter not found";
    }

    TEST(gTEST_manager, add_dscrp)
    {
        gTEST_manager manager = gTEST_manager();
        manager.AddDescription("This is a test description");
        ASSERT_EQ( !(manager.prg_description.compare("This is a test description")),true)     <<"Unconsistent prg synopsis.";
    }
}

class gTEST_option: public DSS::DOption
{
    private:
        bool fTest;
        uint16_t fUint;
        int16_t fInt;
        float fFloat;
        std::string fString;
        std::pair<int16_t,int16_t> fArray;
    
    protected:
        virtual bool UserParameters(std::vector<std::string>::const_iterator& it, const std::vector<std::string>& arg)
        {
            if(!it->compare("-t"))
            {
                fTest |= true;
            }
            else if(!it->compare("--uint"))
            {
                if(std::next(it) != arg.cend()){it++;}else{return false;}
                
                if(it->find_first_not_of("+0123456789") != std::string::npos)
                    return false;

                fUint += static_cast<uint16_t>(stoi(*it)); 
            }
            else if(!it->compare("--int"))
            {
                if(std::next(it) != arg.cend()){it++;}else{return false;}
                
                if(it->find_first_not_of("+-0123456789") != std::string::npos)
                    return false;

                fInt += static_cast<int16_t>(stoi(*it)); 
            }
            else if(!it->compare("--float"))
            {
                if(std::next(it) != arg.cend()){it++;}else{return false;}
                
                if(it->find_first_not_of("+-0123456789.eE") != std::string::npos)
                    return false;

                fFloat += static_cast<float>(stof(*it)); 
            }
            else if(!it->compare("--string"))
            {
                if(std::next(it) != arg.cend()){it++;}else{return false;}

                fString += std::string(*it); 
            }
            else if(!it->compare("--array"))
            {
                if(std::next(it) != arg.cend()){it++;}else{return false;}
                if(it->find_first_not_of("+-0123456789") != std::string::npos)
                    return false;

                fArray.first += static_cast<int16_t>(stoi(*it)); 

                if(std::next(it) != arg.cend()){it++;}else{return false;}
                if(it->find_first_not_of("+-0123456789") != std::string::npos)
                    return false;

                fArray.second += static_cast<int16_t>(stoi(*it)); 
            }
            
            return true;
        }

    public:
        gTEST_option():DSS::DOption(),fTest(false),fUint(0),fInt(0),fFloat(0),fString(),fArray(0,0)
        {
            AddOption("-T","This is a basic test");
            AddOption("--Uint","uint","Test for uint param");
            AddOption("--Int","int","Test for int param");
            AddOption("--Float","foat","Test for float param");
            AddOption("--String","str","Test for float param");
            AddOption("--Array","int int","Test for int array");
        }

        inline const bool& Test                 () const {return fTest;}
        inline const uint16_t& Uint                 () const {return fUint;}
        inline const int16_t& Int                   () const {return fInt;}
        inline const float&  Float                  () const {return fFloat;}
        inline const std::string& Str               () const {return fString;}
        inline const std::pair<int16_t,int16_t>& Arr() const {return fArray;}
    
};

TEST(DSS_DOption, default_constructor)
{
    int args=7;
    char** argv= new char*[args];

    std::pair<std::string,std::string> d("-d","3");
    std::pair<std::string,std::string> o("-o","output");
    std::string f1 = std::string("/url/file_1");
    std::string f2 = std::string("/url/file_2");
    std::string f3 = std::string("file_3");

    uint16_t k=0;

    argv[k] = new char[d.first.size()];
    strcpy(argv[k],d.first.c_str());
    k++;

    argv[k] = new char[d.second.size()];
    strcpy(argv[k],d.second.c_str());
    k++;

    argv[k] = new char[o.first.size()];
    strcpy(argv[k],o.first.c_str());
    k++;

    argv[k] = new char[o.second.size()];
    strcpy(argv[k],o.second.c_str());
    k++;

    argv[k] = new char[f1.size()];
    strcpy(argv[k],f1.c_str());
    k++;

    argv[k] = new char[f2.size()];
    strcpy(argv[k],f2.c_str());
    k++;

    argv[k] = new char[f3.size()];
    strcpy(argv[k],f3.c_str());
    k++;

    gTEST_option manager=gTEST_option();
    ASSERT_EQ(manager.InitParameters(args,argv),true)<<"Initialisation errors !!!";
    ASSERT_EQ(manager.Debug(),true)<<"Debug not set !!!";
    ASSERT_EQ(manager.DebugLevel(),3)<<"Debug level inconsistency !!!";
    ASSERT_EQ(!manager.GetOutput().compare("output"),true)<<"Output inconsistency !!!";
    ASSERT_EQ(manager.GetFiles().size(),3) <<"Input inconsistency "<<manager.GetFiles().size()<<"!!!";
    ASSERT_EQ(!manager.GetFiles()[0].compare("/url/file_1"),true)<<"Input[0] inconsistency !!!";
    ASSERT_EQ(!manager.GetFiles()[1].compare("/url/file_2"),true)<<"Input[1] inconsistency !!!";
    ASSERT_EQ(!manager.GetFiles()[2].compare("file_3"),true)<<"Input[2] inconsistency !!!"; 

    ASSERT_EQ(manager.Test (),false)<<"Initialisation errors !!!"; 
    ASSERT_EQ(manager.Uint (),0)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager. Int (),0)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager.Float(),0)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager.Str().size(),0)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager.Arr().first,0)<<"Initialisation errors !!!";   
    ASSERT_EQ(manager.Arr().second,0)<<"Initialisation errors !!!";   
}

TEST(DSS_DOption, help_constructor)
{
    int args=1;
    char** argv= new char*[args];

    std::string opt = std::string("-h");

    uint16_t k=0;

    argv[k] = new char[opt.size()];
    strcpy(argv[k],opt.c_str());
    k++;

    gTEST_option manager=gTEST_option();
    ASSERT_EQ(manager.InitParameters(args,argv),false)<<"Initialisation errors !!!";
   
}

TEST(DSS_DOption, optional_constructor)
{
    std::vector<std::string> opt=std::vector<std::string>();
    opt.push_back("-T");
    opt.push_back("--Uint");
    opt.push_back("7");
    opt.push_back("--Int");
    opt.push_back("-7");
    opt.push_back("--Float");
    opt.push_back("-7.25");
    opt.push_back("--String");
    opt.push_back("Test-String");
    opt.push_back("--Array");
    opt.push_back("-324");
    opt.push_back("+324");
    opt.push_back("/url/file.test");
    
    
    int args=static_cast<int>(opt.size());
    char** argv= new char*[args];

    for(size_t i=0; i < opt.size(); i++)
    {
        int k= static_cast<int>(i);
        argv[k] = new char[opt[i].size()];
        strcpy(argv[k],opt[i].c_str());
    }

    gTEST_option manager=gTEST_option();
    ASSERT_EQ(manager.InitParameters(args,argv),true)<<"Initialisation errors !!!";
    ASSERT_EQ(manager.Test (),true)<<"Initialisation errors !!!"; 
    ASSERT_EQ(manager.Uint (),7)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager. Int (),-7)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager.Float(),-7.25)<<"Initialisation errors !!!";  
    ASSERT_EQ(!manager.Str().compare("Test-String"),true)<<"Initialisation errors !!!";  
    ASSERT_EQ(manager.Arr().first,-324)<<"Initialisation errors !!!";   
    ASSERT_EQ(manager.Arr().second,324)<<"Initialisation errors !!!";    
}

TEST(DSS_DOption, test_exception)
{
    std::vector<std::string> opt=std::vector<std::string>();
    opt.push_back("-G");   
    
    int args=static_cast<int>(opt.size());
    char** argv= new char*[args];

    for(size_t i=0; i < opt.size(); i++)
    {
        int k= static_cast<int>(i);
        argv[k] = new char[opt[i].size()];
        strcpy(argv[k],opt[i].c_str());
    }

    gTEST_option manager=gTEST_option();
    EXPECT_THROW(manager.InitParameters(args,argv),std::runtime_error);
}

TEST(DSS_DOption, debug_as_bool)
{
    std::vector<std::string> opt=std::vector<std::string>();
    opt.push_back("-d"); 
    opt.push_back("/url/test");   
    
    int args=static_cast<int>(opt.size());
    char** argv= new char*[args];

    for(size_t i=0; i < opt.size(); i++)
    {
        int k= static_cast<int>(i);
        argv[k] = new char[opt[i].size()];
        strcpy(argv[k],opt[i].c_str());
    }

    gTEST_option manager=gTEST_option();
    ASSERT_EQ(manager.InitParameters(args,argv),true)<<"Initialisation errors !!!";
    ASSERT_EQ(manager.Debug(),true)<<"Debug not set !!!";
    ASSERT_EQ(manager.DebugLevel(),1)<<"Debug not set !!!";
}

TEST(DSS_DOption, debug_level)
{
    std::vector<std::string> opt=std::vector<std::string>();
    opt.push_back("-d");
    opt.push_back("3"); 
    opt.push_back("/url/test");   
    
    int args=static_cast<int>(opt.size());
    char** argv= new char*[args];

    for(size_t i=0; i < opt.size(); i++)
    {
        int k= static_cast<int>(i);
        argv[k] = new char[opt[i].size()];
        strcpy(argv[k],opt[i].c_str());
    }

    gTEST_option manager=gTEST_option();
    ASSERT_EQ(manager.InitParameters(args,argv),true)<<"Initialisation errors !!!";
    ASSERT_EQ(manager.Debug(),true)<<"Debug not set !!!";
    ASSERT_EQ(manager.DebugLevel(),3)<<"Debug not set !!!";
}

TEST(DSS_DOption, test_missing_input)
{
    std::vector<std::string> opt=std::vector<std::string>();
    
    int args=static_cast<int>(opt.size());
    char** argv= new char*[args];

    for(size_t i=0; i < opt.size(); i++)
    {
        int k= static_cast<int>(i);
        argv[k] = new char[opt[i].size()];
        strcpy(argv[k],opt[i].c_str());
    }

    gTEST_option manager=gTEST_option();
    EXPECT_THROW(manager.InitParameters(args,argv),std::runtime_error);
}
