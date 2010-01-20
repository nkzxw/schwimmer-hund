#ifndef BaseImpl_h__
#define BaseImpl_h__

#include <string>

namespace detail
{

class BaseImpl
{
public: // private: //TODO:
	int notifyFilters_;									//TODO: debería ser un enum
	std::string filter_;
	bool includeSubdirectories_;
};

}

#endif // BaseImpl_h__