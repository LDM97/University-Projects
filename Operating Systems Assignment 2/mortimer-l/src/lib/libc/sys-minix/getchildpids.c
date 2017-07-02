#include <lib.h>
#include <unistd.h>
#include <errno.h>

int printchildpids(pid_t pid) {
	
	message m;
	m.m1_i1 = ( int ) pid;
	
	if( m.m1_i1 < 1 ){
		// global errno set to reflect bad input.
		errno = EINVAL;
		return -1;
	}
	_syscall( PM_PROC_NR, PRINTCHILDPIDS, &m );
	
    return 0;
}

int getnchildren(pid_t pid) {
    
	message m;
	m.m1_i1 = ( int ) pid;
	
	if( m.m1_i1 < 1 ){
		// global errno set to reflect bad input.
		errno = EINVAL;
		return -1;
	}
	int children = _syscall( PM_PROC_NR, GETNCHILDREN, &m );
	
    return children;
}
        
int getchildpids(pid_t pid, int nchildren, pid_t *childpids) {
	
    message m;
	
	m.m1_i1 = nchildren;
	m.m1_i2 = ( int ) pid;
	char *arrayAddress = (char *)childpids;
	m.m1_p1 = arrayAddress;
	
	if( m.m1_i1 < 1 || m.m1_i2 < 1 ){
		// global errno set to reflect bad input.
		errno = EINVAL;
		return -1;
	}

	int foundChildren = _syscall(PM_PROC_NR, GETCHILDPIDS, &m);
	return foundChildren;
}
