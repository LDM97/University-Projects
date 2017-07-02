/*
 * Replace the following string of 0s with your student number
 * 150228550
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

// you will probably need other system library header files included here
# include <pwd.h>
# include <string.h>
# include <errno.h>

#include "filecmdrlib.h"

#define _BSD_SOURCE
#include <dirent.h>

#define MODE_S_LEN  11
#define TIME_S_LEN  17

/* 
 * is_user_exec helper function to test user execute permission for given 
 * file mode and owner uid and gid.
 * Uses getuid() to get the uid of the calling process and getgid() to get the 
 * gid of the calling process.
 * This function is not part of the filecmdrlib interface.
 */ 
bool is_user_exec(mode_t mode, uid_t ouid, gid_t ogid) {
    if (ouid == getuid())
        return mode & S_IXUSR;
    
    if (ogid == getgid())
        return mode & S_IXGRP;
        
    return mode & S_IXOTH;
}

/* 
 * The following functions are defined in filecmdrlib.h and need to be
 * modified to comply with their definitions and the coursework specification.
 */
 
int execfile( char *path ) {
	
	// Store the arguments for use with execv.
	char *args[ 2 ];
	args[ 0 ] = path;
	args[ 1 ] = NULL;
	
	// Fork to run the execute.
	pid_t processID = fork();
	
	if( processID == -1 ){ // Failed to fork.
		return -1;
	}
	else if( processID == 0 ){ // only runs for the child.
	
		if( execv( path, args ) == -1 ){ 
			return -1; // Failed to execute.
		}
	}

	exit(0); // Succesful execution, exit the program.
	
}

int listdir(char *path) {
	
	// Scan the directory entries and store them.
	struct dirent **entries;
	int num;
	
	if( ( num = scandir( path, &entries, NULL, alphasort ) ) == -1 ){ 
		// unable to scandirectory for some reason.
		return -1;
	}
	
	// change the working directory to the path.
	if( chdir( path ) == -1 ){
		// Unable to change working directory.
		return -1;
	}
	
	for( int i = 0; i < num; i++ ){ // Loop through each entry in the directory.
		
		// Get printfinf to display the information of the given entry, freeing that space in memory once used.
		printfinf( entries[ i ] -> d_name );
		free( entries[ i ] );
	}
	// All entries used, so free the dirent.
	free( entries );
	
	// Return success.
    return 0;
}

int listfile(char *path) {
	
	// Initialise stuff needed for getline method later.
	FILE *filePointer;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	
	// Attempt to open the file w/ the path given. Return fail if unable to.
	filePointer = fopen( path, "r" );
	if( filePointer == NULL ){
		return -1;
	}
	
	// Read in the file line by line to print it out. 
	// Stops when getline can't find another line / returns -1.
	while( ( read = getline( &line, &len, filePointer ) ) != -1 ){
		printf( "%s", line );
	}
	
	// Close the file and succesful return.
	fclose( filePointer );
	return 0;
}

char *mode2str(mode_t mode, uid_t ouid, gid_t ogid) { 
	
	if( mode < MODE_MIN || mode > MODE_MAX ){
		// If mode out of valid range, set errno to EDOM and return fail / NULL.
		errno = EDOM;
		return NULL;
	}
	
	// Allocate memory to store the mode string.
    char *mode_s;

	if(	( mode_s = (char *) calloc(MODE_S_LEN, sizeof(char) ) ) == NULL ){
		// if calloc is null, unable to allocate memory.
		errno = ENOMEM;
		return NULL;
	}
	
	// is a directory.
	if( S_ISDIR( mode ) ){
		mode_s[ 0 ] = 'd';
	}
	// is regular file.
	else if( S_ISREG( mode ) ){
		// Is the file user executable? Report relevant file type if y or n.
		mode_s[ 0 ] = ( is_user_exec( mode, ouid, ogid ) ) ? 'e' : 'f';
	}
	// if is symbolic link.
	else if( S_ISLNK( mode ) ){
		mode_s[ 0 ] = 'l';
	}
	// is other.
	else{
		mode_s[ 0 ] = 'o';
	}
	
	// Setup the bit mask for checking file permissions.
	mode_t mask = ( mode & ~S_IFMT ); 
	
	// User permissions.
	mode_s[ 1 ] = ( mask & S_IRUSR ) ? 'r' : '-';
	mode_s[ 2 ] = ( mask & S_IWUSR ) ? 'w' : '-';
	mode_s[ 3 ] = ( mask & S_IXUSR ) ? 'x' : '-';
	
	// Group permissions.
	mode_s[ 4 ] = ( mask & S_IRGRP ) ? 'r' : '-';
	mode_s[ 5 ] = ( mask & S_IWGRP ) ? 'w' : '-';
	mode_s[ 6 ] = ( mask & S_IXGRP ) ? 'x' : '-';
	
	// Other permissions.
	mode_s[ 7 ] = ( mask & S_IROTH ) ? 'r' : '-';
	mode_s[ 8 ] = ( mask & S_IWOTH ) ? 'w' : '-';
	mode_s[ 9 ] = ( mask & S_IXOTH ) ? 'x' : '-';
	
	// Return the string represneting file permissions.
    return mode_s;
}

int printfinf(char *path) {
	
	// Setup string format.
	static char *str_fmt = "%s %s %*lld %s %s \n";
	
	// Attempt to lstat the file given path. Return error if not possible.
    struct stat file;
	if ( lstat( path, &file ) == -1 ){
		return FTYPE_ERR;
	}
	
	// Obtain file's uid, used in mode2str.
	struct passwd *pwd;
	if( ( pwd = getpwuid( file.st_uid ) ) == NULL ){
		// getpwuid returns null on error, return -1 to reflect the error.
		return FTYPE_ERR;
	}
	
	// Print out the file's info.
	// Uses the string format, mode2str(), time2str(), & spacing alignment.
	printf( str_fmt, mode2str( file.st_mode, file.st_uid, file.st_gid ), pwd -> pw_name, 15, file.st_size, time2str( file.st_mtime ), path );
	
	// is a directory.
	if( S_ISDIR( file.st_mode ) ){
		return FTYPE_DIR;
	}
	
	// is regular file.
	else if( S_ISREG( file.st_mode ) ){
		
		// Return the appropriate file type based on weather the file type is user executable or not.
		return ( is_user_exec( file.st_mode, file.st_uid, file.st_gid ) ) ? FTYPE_EXE : FTYPE_REG;
		
	}
	// if is symbolic link.
	else if( S_ISLNK( file.st_mode ) ){
		return FTYPE_LNK;
	}
	// other
	else{
		return FTYPE_OTH;
	}

}

char *time2str(time_t time) {
    static char *str_fmt = "%02d/%02d/%4d %02d:%02d\n"; // dd/mm/yyyy hh:mm
	
	// calloc the number of chars required, date always 17 chars long.
    char *time_s;
	if( ( time_s = ( char * ) calloc( 17, sizeof( char ) ) ) == NULL ){
		// Calloc returns NULL if unable to allocate memory.
		// Set and return the error values.
		errno = ENOMEM;
		return NULL;
	}
	
	// Store the time in a local struct to allow easy extraction of data needed.
	struct tm local_tm = *localtime( &time );
	
	// Construct the return string to be stored in time_s.
	// max buffer set to size of 17 chars (the size of time_s).
	// month + 1 for readability as starts from 0, year + 1900 to convert from time since epoch.
	
	if ( snprintf( time_s, sizeof( char ) * 17, str_fmt, local_tm.tm_mday, local_tm.tm_mon + 1, 
		local_tm.tm_year + 1900, local_tm.tm_hour, local_tm.tm_min ) < 0 ){
			
			// if snprintf returns value < 0, an error has occured so reflect this.
			return NULL;
		}
	
	// Succes so return the constructed string.
    return time_s;
}

int useraction(int ftype, char *path) {
	
	// Store action prompts.
    static const char *action_prompt[] = {
        "Do you want to list the directory %s (y/n): ",
        "Do you want to execute %s (y/n): ",
        "Do you want to list the file %s (y/n): " 
    };
	
	// Stat the file to access its data.
	// Error checking for stat already done in printfinf().
	struct stat file;
	lstat( path, &file );

	// Char to store the response of the user from scanf.
	char response;
	
	// Handle a directory.
	if( ftype == FTYPE_DIR ){
		
		// Prompt for directory and wait for response.
		printf( action_prompt[ FTYPE_DIR ], path );
		scanf( "%c", &response );
		
		// List if yes, do nothing if anything else.
		if( response == 'y' || response == 'Y' ){
			if( listdir( path ) == -1 ){
				// Was there an error during listdir's execution?
				return -1;
			} else { return 0; }
		}
		else{
			return 0;
		}
	}
	
	// Handle a user executable regular file.
	if( ftype == FTYPE_EXE ){
		
		// Prompt for user executable file.
		printf( action_prompt[ FTYPE_EXE ] , path );
		scanf( "%c", &response );
		
		// Execute if yes, do nothing if anything else.
		if( response == 'y' || response == 'Y' ){
			execfile( path );
		}
		else{
			return 0;
		}
	}
	
	// Handle a non executable regular file.
	if( ftype == FTYPE_REG ){
		
		// Prompt for regular file.
		printf( action_prompt[ FTYPE_REG ], path );
		scanf( "%c", &response );
		
		// List file if yes, do nothing if anything else.
		if( response == 'y' || response == 'Y' ){
			if( listfile( path ) == -1 ){
				// Was there an error executing listfile?
				return -1;
			} else { return 0; }
		}
		else{
			return 0;
		}
	}
	
	// Handle other file types.
	return 0;
	
}