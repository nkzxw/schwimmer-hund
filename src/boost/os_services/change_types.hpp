#ifndef BOOST_OS_SERVICES_CHANGE_TYPES_HPP
#define BOOST_OS_SERVICES_CHANGE_TYPES_HPP

//#define IN_ACCESS	 0x00000001	/* File was accessed.  */
//#define IN_ATTRIB	 0x00000004	/* Metadata changed.  */
//#define IN_CLOSE_WRITE	 0x00000008	/* Writtable file was closed.  */
//#define IN_CLOSE_NOWRITE 0x00000010	/* Unwrittable file closed.  */
//#define IN_CLOSE	 (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) /* Close.  */
//#define IN_OPEN		 0x00000020	/* File was opened.  */

//#define IN_MODIFY	 0x00000002	/* File was modified.  */
//#define IN_MOVED_FROM	 0x00000040	/* File was moved from X.  */
//#define IN_MOVED_TO      0x00000080	/* File was moved to Y.  */
//#define IN_MOVE		 (IN_MOVED_FROM | IN_MOVED_TO) /* Moves.  */
//#define IN_CREATE	 0x00000100	/* Subfile was created.  */
//#define IN_DELETE	 0x00000200	/* Subfile was deleted.  */
//#define IN_DELETE_SELF	 0x00000400	/* Self was deleted.  */
//#define IN_MOVE_SELF	 0x00000800	/* Self was moved.  */

namespace boost {
namespace os_services {
namespace change_types
{
	static const int created = 1;
	static const int deleted = 2;
	static const int changed = 4;
	static const int renamed = 8;
	static const int all = 15;

} // namespace change_types
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_CHANGE_TYPES_HPP
