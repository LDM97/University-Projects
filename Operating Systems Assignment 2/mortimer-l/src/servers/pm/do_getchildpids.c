#include <stdio.h>
#include "pm.h"             // for glo.h:  mp, call_nr, who_p etc.
#include "mproc.h"          // for proc table
#include <errno.h>

// system call for library function printchildpids
int do_printchildpids() {

    int idx = m_in.m1_i1; // Get the input id from the message.

	if( idx < 1 ){
		// global errno set to reflect bad input.
		errno = EINVAL;
		return -1;
	}
	
	printf( "Searching for children of process: %d\n", idx );
	
	for( int i = 0; i < NR_PROCS; i++ ){
		int currentPid = mproc[ i ].mp_pid; // Get the pid of the current index.
		int parentPid = mproc[ mproc[ i ].mp_parent ].mp_pid; // Find current index's parent.
		
		
		if( idx == parentPid && currentPid != 0 ){ // Is current process' parent the input id? Print if yes.
			printf( "%d\n", currentPid );
		}
	}
	
	return 0;
}

// system call for library function getnchildren
int do_getnchildren() {
    int idx = m_in.m1_i1; // Get the input id from the message.

	if( idx < 1 ){
		// global errno set to reflect bad input.
		errno = EINVAL;
		return -1;
	}
	
	// Set counter for the number of children.
	int numOfChildren = 0;
	
	for( int i = 0; i < NR_PROCS; i++ ){
		int currentPid = mproc[ i ].mp_pid; // Get the pid of the current index.
		int parentPid = mproc[ mproc[ i ].mp_parent ].mp_pid; // Find current index's parent.
		
		
		if( idx == parentPid && currentPid != 0 ){ 
			// Is current process' parent the input id? Found a child, ++ the count.
			numOfChildren ++; // Increment the number of children found.
		}
	}
    return numOfChildren;
}

// system call for library function getchildpids
int do_getchildpids() {
	// Recieve the inputs from the message.
	int nchildren = m_in.m1_i1;
	int inputPid = m_in.m1_i2;
	char *arrayAddress = m_in.m1_p1;
	
	if( inputPid < 1 || nchildren < 1 ){
		// global errno set to reflect bad inputs.
		errno = EINVAL;
		return -1;
	}
	
	// Initialise array to store found pids and count for the number of children found.
	pid_t foundChildren[ nchildren ];
	int childrenFound = 0;
	
	for( int i = 0; childrenFound < nchildren; i++ ){ // Loop until found nchildren
		if( i >= NR_PROCS ){
			break; // Don't want to exceed pm process table, break if reach the end.
		}
		
		
		int currentPid = mproc[ i ].mp_pid; // Get the pid of the current index.
		int parentPid = mproc[ mproc[ i ].mp_parent ].mp_pid; // Find current index's parent.
			
			
		if( inputPid == parentPid && currentPid != 0 ){ 
			// Is current process' parent the input id? Store it in array if yes.
			foundChildren[ childrenFound ] = ( pid_t )currentPid;
			childrenFound ++;
		} 
	}
	
	// sys_vircopy( SELF, (vir_bytes) childpids, who_e, (vir_bytes) arrayaddress, sizeof(childpids) );
	sys_vircopy( SELF, (vir_bytes) foundChildren, who_e, (vir_bytes) arrayAddress, sizeof( foundChildren ) );

	
    return childrenFound;
}
