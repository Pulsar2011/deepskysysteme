#include "DSTCore/DSS_version.h" //update as appropriate

// Namespace DSS stand For DeepSkySys.
namespace DSS
{
  gGIT *gGIT::this_ver = NULL;
  
  gGIT::gGIT():_VERSION("HEAD-HASH-NOTFOUND"),_VERSION_SHORT("HEAD-HASH-NOTFOUND.HEAD-HASH-NOTFOUND.HEAD-HASH-NOTFOUND"),_VERSION_SHA1("HEAD-HASH-NOTFOUND")
  {

  }

  gGIT::~gGIT()
  {
  }

  const std::string gGIT::version() const
  {
    if(this_ver == NULL)
      gGIT::this_ver = new gGIT();

    return this_ver->_VERSION;
  }

  const std::string gGIT::version_short() const
  {
    if(this_ver == NULL)
      gGIT::this_ver = new gGIT();

    return this_ver->_VERSION_SHORT;
  }

  const std::string gGIT::sha1() const
  {
    if(this_ver == NULL)
      gGIT::this_ver = new gGIT();

    return this_ver->_VERSION_SHA1;
  }
}
