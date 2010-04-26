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
#include <boost/filesystem/path.hpp>
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
		, implementation_( new detail::fsm_implementation_type ) //TODO: cambiar nombre "fsm_implementation_type"
	{

		//TODO: donde setear estos datos?????
		//, notify_filters_(notify_filters::last_write | notify_filters::directory_name | notify_filters::file_name)
		//, filter_("*.*")

	}

	//TODO: ver que todos los destructores sean virtuales...
	//virtual ~file_system_monitor()
	//{
	//}

	//TODO: agregar otro add_directory que acepte un boost::filesystem::path(?)
	void add_directory( const std::string& dir_name ) //throw (std::invalid_argument)
	{ 
		implementation_->add_directory( dir_name );
	}

	void add_directory( const boost::filesystem::path& directory ) //throw (std::invalid_argument, std::runtime_error)
	{ 
		implementation_->add_directory( directory );
	}

	int get_notify_filters() const
	{ 
		return implementation_->notify_filters_;
	}

	void set_notify_filters(int val) 
	{ 
		implementation_->notify_filters_ = val;
	}

	std::string get_filter() const //TODO: evitar copias
	{ 
		return implementation_->filter_;
	}

	void set_filter( const std::string& val ) 
	{ 
		implementation_->filter_ = val;
	}

	//TODO: poder setear el tamaño del buffer desde afuera //setInternalBufferSize

	void start() //throw (std::runtime_error)
	{
		//TODO: is_started_ debe ser protegida con MUTEX.
		if (!is_started_)
		{
			implementation_->start();
			is_started_ = true;
		}
	}

	//template <typename EH>
	//void setEventHandler(filesystem_event_handler handler)
	//{
	//	this->implementation_->Changed = handler;
	//}

	// Event Handlers Setters
	void set_changed_event_handler( filesystem_event_handler handler ) //TODO: parametro const???
	{
		this->implementation_->changed_handler_ = handler;
	}

	void set_created_event_handler( filesystem_event_handler handler )
	{
		this->implementation_->created_handler_ = handler;
	}

	void set_deleted_event_handler( filesystem_event_handler handler )
	{
		this->implementation_->deleted_handler_ = handler;
	}

	void set_renamed_event_handler( renamed_event_handler handler )
	{
		this->implementation_->renamed_handler_ = handler;
	}


private:

	bool is_started_; //TODO: pregunta: esto deberia ser manejado acá o por la clase implementacion?

	//TODO: todos estos datos deberian ser manejados por cada Watch
	//int notify_filters_;				//TODO: debería ser un enum
	//std::string filter_;
	//bool include_subdirectories_;

	//TODO: typedef
	boost::shared_ptr<detail::fsm_implementation_type> implementation_;
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
