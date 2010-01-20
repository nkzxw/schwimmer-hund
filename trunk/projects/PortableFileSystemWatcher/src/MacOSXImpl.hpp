#ifndef MacOSXImpl_h__
#define MacOSXImpl_h__

#include "BaseImpl.hpp"

namespace detail
{

struct MacOSXImpl : public BaseImpl
{
	void start()
	{
		std::cout << "MacOSXImpl::start()" << std::endl;
	}
};

typedef MacOSXImpl PlatformImpl;

}

#endif // MacOSXImpl_h__