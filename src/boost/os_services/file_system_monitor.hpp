//  boost/os_services/file_system_monitor.hpp  --------------------------------//

//  Copyright Fernando Pelliccioni 2010

//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/os_services

// You are welcome to contact the author at:
//  fpelliccioni@gmail.com

//----------------------------------------------------------------------------// 

#ifndef BOOST_OS_SERVICES_FILE_SYSTEM_MONITOR_HPP
#define BOOST_OS_SERVICES_FILE_SYSTEM_MONITOR_HPP

#include <boost/bimap.hpp>
#include <boost/noncopyable.hpp>

#include <boost/os_services/details/impl_selector.hpp>
#include <boost/os_services/utils.hpp>

namespace boost {
namespace os_services {

class file_system_monitor : private boost::noncopyable
{
public:
	
	file_system_monitor()
		: is_started_(false)
		, notify_filters_(notify_filters::last_write | notify_filters::directory_name | notify_filters::file_name)
		//, directory_("")
		, filter_("*.*")
		, implementation_( new detail::FSMImplementationType )
	{
	}

	//virtual ~FileSystemMonitor()
	//{
	//}

	//std::string add_directory() const //TODO: evitar copias
	//{ 
	//	return this->directory_; 
	//}

	void add_directory (const std::string& dir_name) throw (std::invalid_argument)
	{ 
		implementation_->add_directory( dir_name );
	}

	int get_notify_filters() const
	{ 
		//return notify_filters_;
		return implementation_->notify_filters_;
	}

	void set_notify_filters(int val) 
	{ 
		//notify_filters_ = val; 
		implementation_->notify_filters_ = val;
	}

	std::string get_filter() const //TODO: evitar copias
	{ 
		//return filter_;
		return implementation_->filter_;
	}

	void set_filter(const std::string& val) 
	{ 
		//filter_ = val;
		implementation_->filter_ = val;
	}

	//bool get_enable_eaising_events() const 
	//{ 
	//	return enable_raising_events_;
	//}
	//void set_enable_eaising_events(bool val) 
	//{ 
	//	enable_raising_events_ = val; 
	//	//TODO: aca va el codigo
	//}

	//TODO: poder setear el tamaño del buffer desde afuera //setInternalBufferSize
	

	void start() throw (std::runtime_error)
	{
		//TODO: validar que no haya sido inicializado antes.

		//implementation_->filter_ = filter_;
		//implementation_->notify_filters_ = notify_filters_;
		//implementation_->include_subdirectories_ = include_subdirectories_;
		//implementation_->start(directory_); //, notify_filters_, include_subdirectories_, filter_);
		implementation_->start();
	}


	//template <typename EH>
	//void setEventHandler(filesystem_event_handler handler)
	//{
	//	this->implementation_->Changed = handler;
	//}

	// Event Handlers Setters
	void set_changed_event_handler(filesystem_event_handler handler)
	{
		this->implementation_->changed_callback_ = handler;
	}

	void set_created_event_handler(filesystem_event_handler handler)
	{
		this->implementation_->created_callback_ = handler;
	}

	void set_deleted_event_handler(filesystem_event_handler handler)
	{
		this->implementation_->deleted_callback_ = handler;
	}

	void set_renamed_event_handler(renamed_event_handler handler)
	{
		this->implementation_->renamed_callback_ = handler;
	}


private:

	bool is_started_;

	int notify_filters_;				//TODO: debería ser un enum
	std::string filter_;
	bool include_subdirectories_;
	//bool enable_raising_events_;

	boost::shared_ptr<detail::FSMImplementationType> implementation_;

};

} // namespace os_services
} // namespace boost


#endif // BOOST_OS_SERVICES_FILE_SYSTEM_MONITOR_HPP



//    public FSW(string path)
//        : this(path, "*.*")
//    {
//    }
//
//    public FSW(string path, string filter)
//    {
//        this.notifyFilters = NotifyFilters.LastWrite | NotifyFilters.DirectoryName | NotifyFilters.FileName;
//        this.internalBufferSize = 0x2000;
//        if (path == null)
//        {
//            throw new ArgumentNullException("path");
//        }
//        if (filter == null)
//        {
//            throw new ArgumentNullException("filter");
//        }
//        if ((path.Length == 0) || !Directory.Exists(path))
//        {
//            throw new ArgumentException(SR.GetString("InvalidDirName", new object[] { path }));
//        }
//        this.directory = path;
//        this.filter = filter;
//    }

//    [DefaultValue("*.*"), IODescription("FSW_Filter"), TypeConverter("System.Diagnostics.Design.StringValueConverter, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a"), RecommendedAsConfigurable(true)]
//    public string Filter
//    {
//        get
//        {
//            return this.filter;
//        }
//        set
//        {
//            if ((value == null) || (value == string.Empty))
//            {
//                value = "*.*";
//            }
//            if (string.Compare(this.filter, value, StringComparison.OrdinalIgnoreCase) != 0)
//            {
//                this.filter = value;
//            }
//        }
//    }

//    [IODescription("FSW_ChangedFilter"), DefaultValue(0x13)]
//    public NotifyFilters NotifyFilter
//    {
//        get
//        {
//            return this.notifyFilters;
//        }
//        set
//        {
//            if ((value & ~notifyFiltersValidMask) != 0)
//            {
//                throw new InvalidEnumArgumentException("value", (int)value, typeof(NotifyFilters));
//            }
//            if (this.notifyFilters != value)
//            {
//                this.notifyFilters = value;
//                this.Restart();
//            }
//        }
//    }

//    [TypeConverter("System.Diagnostics.Design.StringValueConverter, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a"), IODescription("FSW_Path"), Editor("System.Diagnostics.Design.FSWPathEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", "System.Drawing.Design.UITypeEditor, System.Drawing, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a"), DefaultValue(""), RecommendedAsConfigurable(true)]
//    public string Path
//    {
//        get
//        {
//            return this.directory;
//        }
//        set
//        {
//            value = (value == null) ? string.Empty : value;
//            if (string.Compare(this.directory, value, StringComparison.OrdinalIgnoreCase) != 0)
//            {
//                if (base.DesignMode)
//                {
//                    if ((value.IndexOfAny(wildcards) != -1) || (value.IndexOfAny(Path.GetInvalidPathChars()) != -1))
//                    {
//                        throw new ArgumentException(SR.GetString("InvalidDirName", new object[] { value }));
//                    }
//                }
//                else if (!Directory.Exists(value))
//                {
//                    throw new ArgumentException(SR.GetString("InvalidDirName", new object[] { value }));
//                }
//                this.directory = value;
//                this.readGranted = false;
//                this.Restart();
//            }
//        }
//    }


//    private bool MatchPattern(string relativePath)
//    {
//        string fileName = System.IO.Path.GetFileName(relativePath);
//        return ((fileName != null) && System.IO.PatternMatcher.StrictMatchPattern(this.filter.ToUpper(CultureInfo.InvariantCulture), fileName.ToUpper(CultureInfo.InvariantCulture)));
//    }

//    private void Notifyfilesystem_event_args(int action, string name)
//    {
//        if (this.MatchPattern(name))
//        {
//            switch (action)
//            {
//                case 1:
//                    this.OnCreated(new filesystem_event_args(WatcherChangeTypes.Created, this.directory, name));
//                    return;
//
//                case 2:
//                    this.OnDeleted(new filesystem_event_args(WatcherChangeTypes.Deleted, this.directory, name));
//                    return;
//
//                case 3:
//                    this.OnChanged(new filesystem_event_args(WatcherChangeTypes.Changed, this.directory, name));
//                    return;
//            }
//        }
//    }

//    private void NotifyRenameEventArgs(WatcherChangeTypes action, string name, string oldName)
//    {
//        if (this.MatchPattern(name) || this.MatchPattern(oldName))
//        {
//            renamed_event_args e = new renamed_event_args(action, this.directory, name, oldName);
//            this.OnRenamed(e);
//        }
//    }
