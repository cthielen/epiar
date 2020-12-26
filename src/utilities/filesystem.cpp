/**\file			filesystem.cpp
 * \author			Maoserr
 * \date			Created: Wednesday, November 18, 2009
 * \date			Modified: Wednesday, November 18, 2009
 * \brief			Provides an abstraction to the file system
 * \details
 */

#include "includes.h"

#include "utilities/filesystem.h"
#include "utilities/log.h"

list<string> Filesystem::paths;

#ifdef USE_PHYSICSFS

/**\class Filesystem
 * \brief An interface to deal with file system operations.!!
 * 
 * \sa File for dealing with specific files.
 */

/**Initialize the PhysFS system
 * \return Nonzero on success. */
int Filesystem::Init( const char* argv0 ) {
	int retval;

	LogMsg(DEBUG, "Initializing filesystem using PhysicsFS ...");

	if ( (retval = PHYSFS_init(argv0)) == 0 )
		LogMsg(ERR, "Error initializing PhysicsFS. Reason: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

	if ( (retval = PHYSFS_setSaneConfig("games", "epiar", NULL, 0, 0 ) ) == 0 )
		LogMsg(ERR, "Could not set sane paths for PhysFS: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

	// Set up userDir
	if ( (retval = PHYSFS_mkdir("saves/") ) == 0 )
		LogMsg(ERR, "Could not set up the user dir: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

	// Don't add Root directory.  While this can solve some problems, it will create more.
	// Absolute paths are not portable across computers.
	//if ( (retval = PHYSFS_mount("/", NULL, 1)) == 0 )
	//	LogMsg(WARN,"Could not add Root to search path: %s.", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

#ifdef DATADIR
	// If using autotools, include this prefix to help binary find data files for cases where 'make install' was used
	if ( (retval = PHYSFS_mount(DATADIR, NULL, 1) ) == 0 )
		LogMsg(INFO, "Not using DATADIR directory due to an error. Most likely 'make install' has not been run yet, which is normal while developing. Reason: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
#endif /* DATADIR */
	
	return retval;
}

/**Appends an archive to be searched for files
 * \param archivename The path to the archive
 * \return Nonzero on success */
int Filesystem::AppendPath( const string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_mount(archivename.c_str(), NULL, 1)) == 0 )
		LogMsg(ERR, "Error on appends to search path %s.\n%s",archivename.c_str(),
				PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	return retval;
}

/**Prepends an archive to be searched for files
 * \param ardhivename The path to the archive (can also be a folder)
 * \return Nonzero on success */
int Filesystem::PrependPath( const string& archivename ) {
	int retval;
	if ( (retval = PHYSFS_mount(archivename.c_str(), NULL, 0)) == 0 )
		LogMsg(ERR, "Error on prepends to search path %s.\n%s", archivename.c_str(),
				PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	return retval;
}

/**Deletes a file from the filesystem
 * \param filename Filename relative to PHYSFS writedir to delete
 * \return True on success */
bool Filesystem::DeleteFile( const string &filename ) {
	int retval;

	if( (retval = PHYSFS_delete( filename.c_str() ) ) == 0 ) {
		LogMsg(ERR, "Could not delete file (%s): %s\n", filename.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return false;
	}

	return true;
}

/**Ensures no characters are in 'filename' that might cause issues
 * \param filename The filename/string to check
 * \return True if no dangerous characters are found, false if otherwise */
bool Filesystem::FilenameIsSafe( const string &filename ) {
	size_t found;

	// check to ensure the following are not found: <>:"/\|?*
	found = filename.find(string("<"));
	if( found != string::npos ) return false;
	found = filename.find(">");
	if( found != string::npos ) return false;
	found = filename.find(":");
	if( found != string::npos ) return false;
	found = filename.find("\"");
	if( found != string::npos ) return false;
	found = filename.find("/");
	if( found != string::npos ) return false;
	found = filename.find("\\");
	if( found != string::npos ) return false;
	found = filename.find("|");
	if( found != string::npos ) return false;
	found = filename.find("?");
	if( found != string::npos ) return false;
	found = filename.find("*");
	if( found != string::npos ) return false;
	
	return true;
}

/**Enumerates all files available in a path.
 * \param path String pointing to the path
 * \return Nonzero on success (not necessarily true?) */
list<string> Filesystem::Enumerate( const string& path, const string &suffix )
{
	list<string> files;
	char **rc = NULL;
	string fname;

	rc = PHYSFS_enumerateFiles(path.c_str());
	if (rc == NULL) {
		LogMsg(ERR, "Failure to enumerate %s. reason: %s.\n",
				path.c_str(), PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
	}
	else
	{
		int file_count;
		char **i;
		for (i = rc, file_count = 0; *i != NULL; i++, file_count++)
		{
            fname = string(*i);
            // Skip Makefiles
			if( fname == "Makefile.am") continue;
			if( fname == "Makefile.in") continue;
			// Skip hidden files
			if( fname[0] == '.' ) continue;
			// Check if the suffix matches
			if(fname.size() > suffix.size())
            {
				if( std::equal(fname.begin() + fname.size() - suffix.size(), fname.end(), suffix.begin()) ) {
					files.push_back( fname );
				}
			}
		}

		LogMsg(INFO,"\n total (%d) files.\n", file_count);
		PHYSFS_freeList(rc);
		//return 1;
	}
	return files;
}

/**Prints the current version of PhysFS.*/
void Filesystem::Version( void ){
	PHYSFS_Version compiled, linked;
	PHYSFS_VERSION(&compiled);
	PHYSFS_getLinkedVersion(&linked);
	LogMsg(INFO," Compiled against PhysicsFS version %d.%d.%d,\n"
			" and linked against %d.%d.%d.\n\n",
			(int) compiled.major, (int) compiled.minor, (int) compiled.patch,
			(int) linked.major, (int) linked.minor, (int) linked.patch);
}

/**Prints the current archives supported.*/
void Filesystem::OutputArchivers( void ){
	const PHYSFS_ArchiveInfo **rc = PHYSFS_supportedArchiveTypes();
	const PHYSFS_ArchiveInfo **i;

	LogMsg(INFO,"Supported archive types:\n");
	if (*rc == NULL)
		LogMsg(INFO," * Apparently, NONE!\n");
	else{
		for (i = rc; *i != NULL; i++){
			LogMsg(INFO," * %s: %s\n	Written by %s.\n	%s\n",
					(*i)->extension, (*i)->description,
					(*i)->author, (*i)->url);
		}
	}
}

/**Unloads the physfs library
  * \return Nonzero on success */
int Filesystem::Close() {
	int retval;

	if ( (retval = PHYSFS_deinit()) == 0 )
		LogMsg(ERR, "Error de-initializing PhysicsFS.\n%s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

	return retval;
}

#else

/**\class Filesystem
 * An interface to deal with file system operations.
 * see the File class for dealing with specific files.
 * \sa File */

/** Stub. Empty function.
 * \return Always 1 */
int Filesystem::Init( const char* argv0 ) {
	LogMsg(DEBUG, "Initializing filesystem without using PhysicsFS ...");
	return 1;
}

/**Initialize the PhysFS system with some default configuration.
 * The write directory is set to $HOME/.Games/Epiar
 * The search path is set to
 *  - The write directory
 *  - The directory of the current executeable
 *  - Any archives found in the path (must pass in the extension)
 * \return Nonzero on success. */
int Filesystem::Init( const char* argv0, const string &extension ) {
	return 1;
}

/**Appends an archive to be searched for files
 * \param archivename The path to the archive
 * \return Nonzero on success */
int Filesystem::AppendPath( const string& archivename ) {
	paths.push_back( archivename );
	return 1;
}

/**Prepends an archive to be searched for files
 * \param ardhivename The path to the archive (can also be a folder)
 * \return Nonzero on success */
int Filesystem::PrependPath( const string& archivename ) {
	paths.push_front( archivename );
	return 1;
}

/**Enumerates all files available in a path.
 * \todo Need windows version
 * \param path String pointing to the path
 * \return Nonzero on success */
list<string> Filesystem::Enumerate( const string& path, const string &suffix )
{
	list<string> files;
#ifndef _WIN32
	DIR *dp;
	struct dirent *ep;
	string fname;

	dp = opendir (path.c_str());
	if (dp != NULL)
	{
		while (NULL != (ep = readdir (dp))) {
			fname = string(ep->d_name);
			// Skip hidden files
			if( fname[0] == '.' ) continue;
			// Check if the suffix matches
			if(fname.size() > suffix.size()) {
				if( std::equal(fname.begin() + fname.size() - suffix.size(), fname.end(), suffix.begin()) ) {
					files.push_back( fname );
				}
			}
		}
		(void) closedir (dp);
	}
	else
		perror ("Couldn't open the directory");
#else
#error WIN32 Filesystem not written yet. Use the PhysFS build.
#endif
	return files;
}

/**Prints the current version of PhysFS.*/
void Filesystem::Version( void ){
}

/**Prints the current archives supported.*/
void Filesystem::OutputArchivers( void ){
	
}

/**Unloads the physfs library
  * \return Nonzero on success */
int Filesystem::DeInit() {
	return 1;
}

#endif
