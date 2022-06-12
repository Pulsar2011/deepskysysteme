#ifndef DSS_VERSION_H
#define DSS_VERSION_H

#include <string>

namespace DSS
{
  class gGIT
  {
  private:
   const std::string _VERSION;
   const std::string _VERSION_SHORT;
   const std::string _VERSION_SHA1;
  
   ~gGIT();
   gGIT();
   
   static gGIT *this_ver;

  public:
   static const std::string version();
   static const std::string version_short();
   static const std::string sha1();
   
  };

}

#endif //DSS_VERSION_H
