#ifndef FileSystemMonitorImplTestGeneric_h__
#define FileSystemMonitorImplTestGeneric_h__

namespace detail
{

template <typename PlatformImpl>
class FileSystemMonitorGenericImpl : public PlatformImpl
{
public:
//	FileSystemMonitorGenericImpl()
//	{
//		std::cout << "FileSystemMonitorGenericImpl()" << std::endl;
//	}
//
//	~FileSystemMonitorGenericImpl()
//	{
//		std::cout << "~FileSystemMonitorGenericImpl()" << std::endl;
//	}
};

}


////TODO: hacemos esto o especializamos???????????
//template <>
//class FileSystemMonitorGenericImpl<Win32Impl> : public PlatformImpl
//{
//public:
//
//};

#endif // FileSystemMonitorImplTestGeneric_h__
