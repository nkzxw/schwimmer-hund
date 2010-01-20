#ifndef FreeBSDImpl_h__
#define FreeBSDImpl_h__

#include "BaseImpl.hpp"

namespace detail
{

struct FreeBSDImpl : public BaseImpl
{
	void start()
	{
		std::cout << "FreeBSDImpl::start()" << std::endl;
	}
};

typedef FreeBSDImpl PlatformImpl;

}

#endif // FreeBSDImpl_h__