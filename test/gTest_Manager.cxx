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
            bool UserParameters(int &index, char** argv, int argc){return true;}
    };

    TEST(gTEST_manager, constructor)
    {
        gTEST_manager manager = gTEST_manager();
        ASSERT_EQ(manager.filelist.size(),0);
        ASSERT_EQ(manager.usr_option.size(),0);
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
        manager.AddOption("-t","test","this is a test option");
        manager.AddOption("-T","this is a test option");

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

}