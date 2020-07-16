/**\file			file.cpp
 * \author			Christopher Thielen (chris@epiar.net)
 * \date			Created: Monday, April 21, 2008
 * \date			Modified: Wednesday, July 15, 2020
 * \brief			Low level interface for file access.
 * \details
 * Use filesystem for higher level access.*/

#include "includes.h"
#include "utilities/file.h"
#include "utilities/log.h"

#ifndef USE_PHYSICSFS
#define PHYSFS_getLastError() "FAILED!"
#endif


/** \class File
 *  \brief Low level file access abstraction through PhysicsFS.
 */

/**Creates empty file instance.*/
File::File( void ):
	fp(NULL), contentSize(0),validName("")
{
	
}

/**Creates file instance linked to filename. \sa Open.*/
File::File( const string& filename, bool writable ):
	fp(NULL), contentSize(0), validName("")
{
	if( writable )
		OpenWrite( filename );
	else
		OpenRead( filename );
}

/**Opens a file for reading
 * \param filename The filename path.
 * \return true if successful, false otherwise.*/
bool File::OpenRead( const string& filename ) {
	if ( fp != NULL )
		this->Close();

	const char *cName;

	cName = filename.c_str();

	// Check for file existence
	if( !File::Exists(filename) ){
		LogMsg(WARN,"Could not open file for reading. File does not exist.\n");
		return false;
	}

#ifdef USE_PHYSICSFS
	const char *realDir = PHYSFS_getRealDir(cName);
	LogMsg(DEBUG, "Opening '%s%s' (PhysFs)", realDir, cName);
	fp = PHYSFS_openRead( cName );
#else
	LogMsg(DEBUG, "Opening '%s' (no PhysFs, working directory assumed)", cName);
	fp = fopen(cName, "rb");
#endif
	if( fp == NULL ) {
		LogMsg(ERR,"Could not open file: %s.\n%s", cName,
			PHYSFS_getLastError());
		return false ;
	}
	
#ifdef USE_PHYSICSFS
	contentSize = static_cast<long>( PHYSFS_fileLength( fp ) );
#else
	struct stat fileStatus;
	int stat_ret = stat(cName, &fileStatus );
	if(stat_ret) {
		return false;
	}
	contentSize = fileStatus.st_size;
#endif
	validName.assign( filename );

	return true ;
}

/**Opens a file for writing
 * \param filename The filename path
 * \return true if successful, false otherwise.*/
bool File::OpenWrite( const string& filename ) {
	if ( fp != NULL )
		this->Close();

	const char *cName;
	cName = filename.c_str();
#ifdef USE_PHYSICSFS
	this->fp = PHYSFS_openWrite( cName );
#else
	this->fp = fopen( cName, "wb");
#endif
	if( fp == NULL ){
		LogMsg(ERR,"Could not open file (%s) for writing: %s\n",cName,
				PHYSFS_getLastError());
		return false;
	}
	validName.assign( filename );

	return true;
}

/**Returns the valid path to the file
 * \return path successful, NULL otherwise.*/
string File::GetRelativePath(){
	return validName;
}

/**Returns the full path to the file
 * \return path successful.*/
string File::GetAbsolutePath(){
	string abs = PHYSFS_getRealDir( validName.c_str() );

	abs += PHYSFS_getDirSeparator() + validName;

	return abs;
}

/**Reads a specified number of bytes.
 * \param numBytes Number of bytes to read.
 * \param buffer Buffer to read bytes into.
 * \return true if successful, false otherwise.*/
bool File::Read( long numBytes, char *buffer ){
	if ( fp == NULL )
		return false;

	long bytesRead = static_cast<long>(
#ifdef USE_PHYSICSFS
		PHYSFS_read( fp, buffer, 1, numBytes )
#else
		fread(buffer, 1, numBytes, fp )
#endif
		);
	if ( bytesRead == numBytes ){
		return true;
	} else {
		LogMsg(ERR,"%s: Unable to read specified number of bytes. %s",
			validName.c_str(), PHYSFS_getLastError());
		return false;
	}
}

/**Reads the whole file into a buffer. Buffer will be automatically allocated
 * for you, but you must explicitly free it by using "delete [] buffer"
 * \return Pointer to buffer, NULL otherwise.*/
char *File::Read( void ){
	if ( fp == NULL )
		return NULL;

	// Seek to beginning
	Seek( 0 );
	char *fBuffer = new char[static_cast<Uint32>(contentSize)];
	long bytesRead = static_cast<long>(
#ifdef USE_PHYSICSFS
		PHYSFS_read( this->fp, fBuffer, 1, contentSize )
#else
		fread(fBuffer,1,contentSize,fp)
#endif
		);
	if( bytesRead == contentSize){
		return fBuffer;
	} else {
		delete [] fBuffer;
		LogMsg(ERR,"%s: Unable to read file into memory. %s",
			validName.c_str(), LastErrorMessage().c_str() );
		return NULL;
	}
}

/**Writes buffer to file.
 * \param buffer The buffer to write
 * \param bufsize The size of the buffer in bytes
 * \return true if successful, false otherwise. */
bool File::Write( char *buffer, const long bufsize ){
	if ( fp == NULL )
		return false;
#ifdef USE_PHYSICSFS
	PHYSFS_sint64 bytesWritten = PHYSFS_write(this->fp, buffer, sizeof(char), bufsize);
#else
	long bytesWritten = fwrite(buffer,1,bufsize,fp);
#endif
	if ( bytesWritten != bufsize ){
		LogMsg(ERR,"Unable to write to file '%s'. (%d/%d bytes written) %s",
			this->validName.c_str(),
			bytesWritten, bufsize,
			LastErrorMessage().c_str() );
		return false;
	}

	return true;
}

/**Gets the current offset from the beginning of the file.
 * \return Offset in bytes from the beginning of the file.*/
long File::Tell( void ){
	long offset;
	offset = static_cast<long>(
#ifdef USE_PHYSICSFS
		PHYSFS_tell( fp )
#else
		ftell(fp)
#endif
		);
	if ( offset == -1 ){
		LogMsg(ERR,"%s: Error using file tell. %s",
			validName.c_str(), PHYSFS_getLastError());
	}
	return offset;
}

/**Seek to new position in the file.
 * \param pos Position in bytes form the beginning of the file.
 * \return true if successful, false otherwise.*/
bool File::Seek( long pos ){
	int retval;
	if ( fp == NULL )
		return false;
#ifdef USE_PHYSICSFS
	retval = PHYSFS_seek( fp, static_cast<PHYSFS_uint64>( pos ));
#else
	const char *cName;
	cName = validName.c_str();
      retval = fseek(fp, pos, SEEK_SET);
#endif
	if ( retval == 0 ){
		LogMsg(ERR,"%s: Error using file seek [%ld]. %s",
		                validName.c_str(), pos, LastErrorMessage().c_str() );
		return false;
	}
	return true;
}

/**Gets the total length in bytes of the file.
 * \return Total length of the file in bytes.*/
long File::GetLength( void ){
	return contentSize;
}

/**Sets the internal buffer for read/write operations.
 * \return Nonzero on success */
int File::SetBuffer( int bufSize ){
#ifdef USE_PHYSICSFS
	if ( PHYSFS_setBuffer( fp, bufSize ) == 0 ){
		LogMsg(ERR,"Could not create internal buffer for file: %s.\n%s",
				validName.c_str(), PHYSFS_getLastError());
		PHYSFS_close( fp );
		return 0;
	}
	return 1;
#else
	return 0; // No idea how this works??? ~ Matt Zweig
#endif
}

/**Destroys file instance. \sa Close.*/
File::~File() {
	Close();
}

/**Closes the file handle and frees associated buffer.
 * \return true if successful, false otherwise*/
bool File::Close() {
	if ( validName.compare( "" ) == 0 )
		return false;

	if ( fp == NULL )
		return false;

#ifdef USE_PHYSICSFS
	int retval = PHYSFS_close( fp );
	if ( retval == 0 )
#else
	int retval = fclose( fp );
	if ( retval != 0 )
#endif
	{
		LogMsg(ERR, "%s: Unable to close file handle.%s",
			validName.c_str(), PHYSFS_getLastError());
		return false;
	}
	contentSize = 0;

	LogMsg(DEBUG, "File '%s' closed/saved successfully.", validName.c_str());

	return true;
}

bool File::Exists( const string& filename ) {
	const char *cName;
	cName = filename.c_str();

#ifdef USE_PHYSICSFS
	if ( !PHYSFS_exists( cName ) ){
		LogMsg(ERR, "%s: %s.", LastErrorMessage().c_str(), cName);
		return false;
	}
#else
	struct stat fileStatus;
	int stat_ret = stat(cName, &fileStatus );

	if ( stat_ret != 0 ) {
        LogMsg(ERR,"%s: %s.", LastErrorMessage().c_str(), cName); break;
		return false;
	}
#endif

	return true;
}

bool File::IsDir( const string& filename ) {
	// TODO: determine if the filename is a directory
	// This can be used for walking a directory tree
	// This can be used for skipping directories
	return false;
}

bool IsBigEndian() {
	int test_var = 1;
	unsigned char *test_array = (unsigned char*)&test_var;
	return (test_array[0] == '\0');
}


string File::LastErrorMessage( void )
{
#ifdef USE_PHYSICSFS
    return PHYSFS_getLastError();
#else
    switch( errno )
    {
        case EPERM: return "Operation not permitted" ;
        case ENOENT: return "No such file or directory" ;
        case ESRCH: return "No such process" ;
        case EINTR: return "Interrupted system call" ;
        case EIO: return "I/O error" ;
        case ENXIO: return "No such device or address" ;
        case E2BIG: return "Argument list too long" ;
        case ENOEXEC: return "Exec format error" ;
        case EBADF: return "Bad file number" ;
        case ECHILD: return "No child processes" ;
        case EAGAIN: return "Try again" ;
        case ENOMEM: return "Out of memory" ;
        case EACCES: return "Permission denied" ;
        case EFAULT: return "Bad address" ;
        case ENOTBLK: return "Block device required" ;
        case EBUSY: return "Device or resource busy" ;
        case EEXIST: return "File exists" ;
        case EXDEV: return "Cross-device link" ;
        case ENODEV: return "No such device" ;
        case ENOTDIR: return "Not a directory" ;
        case EISDIR: return "Is a directory" ;
        case EINVAL: return "Invalid argument" ;
        case ENFILE: return "File table overflow" ;
        case EMFILE: return "Too many open files" ;
        case ENOTTY: return "Not a typewriter" ;
        case ETXTBSY: return "Text file busy" ;
        case EFBIG: return "File too large" ;
        case ENOSPC: return "No space left on device" ;
        case ESPIPE: return "Illegal seek" ;
        case EROFS: return "Read-only file system" ;
        case EMLINK: return "Too many links" ;
        case EPIPE: return "Broken pipe" ;
        case EDOM: return "Math argument out of domain of func" ;
        case ERANGE: return "Math result not representable";
        default:
        {
            char errormsg[30];
            snprintf( errormsg, sizeof(errormsg), "Unknown Error %d.", errno );
            return errormsg;
        }
    }
#endif
}
