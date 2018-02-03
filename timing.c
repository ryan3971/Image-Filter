#include<sys/resource.h>
#include<unistd.h>

double getTime() {           
  struct rusage usage;
  struct timeval time;

  getrusage(RUSAGE_SELF, &usage);

  time = usage.ru_utime;
  return time.tv_sec+time.tv_usec/1e6;
}

	
        
    
    
    
    

