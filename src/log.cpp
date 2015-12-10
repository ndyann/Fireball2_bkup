//
//  MINT
//	Logging functions
//
//  Created by Lucas Sousa de Oliveira on 07/18/12.
//  Property of NASA - Goddard Space Flight Center
//
#include "log.hpp"

FILE* logfile[3] = {NULL};
unsigned int opened_files[3] = {OK,OK,OK};

int open_log(devices file, string path) {
#if LOGGING
	char fname[20] = "mint.log";
	char time1[100],ttime[500],aux[500];
	time_t rawtime;
	struct tm* timeinfo;
    
	// Try to open each logfile X(=1000) times
	for(unsigned int i = 1; (logfile[file] = fopen((path+string(fname)).c_str(),"a")) == NULL && i <= 1000; i++)
		sprintf(fname,"mintlog_%u.log",i);
    
	// If after 1000 tries could not open logfile, disable this address (possible error on device)
	if(logfile[file] == NULL)	{
#if LOG2SCREEN
		fprintf(stderr,"LOG ERROR: The log file (%d) could not be opened at %s\n",file,path.c_str());
#endif
		return DISABLED;
	}
#endif
    
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(time1, 100, "%d/%m/%Y %H:%M:%S:%%06d",timeinfo);
	sprintf(ttime,time1,clock()%CLOCKS_PER_SEC);
	
	sprintf(aux,"Successfully opened logfile (%d) at %s",file,path.c_str());
#if LOG2SCREEN
	fprintf(stderr,"      \t%20s\t%s\t%s\n",__FUNCTION__,ttime,aux); fflush(stdout);
#endif
#if LOGGING
	if(logfile[HD1] != NULL)	{fprintf(logfile[HD1],"      \t%20s\t%s\t%s\n",__FUNCTION__,ttime,aux); fflush(logfile[HD1]);}
	if(logfile[HD2] != NULL)	{fprintf(logfile[HD2],"      \t%20s\t%s\t%s\n",__FUNCTION__,ttime,aux); fflush(logfile[HD2]);}
	if(logfile[USB] != NULL)	{fprintf(logfile[USB],"      \t%20s\t%s\t%s\n",__FUNCTION__,ttime,aux); fflush(logfile[USB]);}
#endif
	return 	OK;
}

int log(string func, unsigned int code, string msg){
#if LOGGING
	string aux;
	char time1[100],ttime[500];
	time_t rawtime;
	struct tm* timeinfo;
    
	if(logfile[HD1] == NULL && opened_files[HD1] != DISABLED)	opened_files[HD1] = open_log(HD1,HD1_PATH);
	if(logfile[HD2] == NULL && opened_files[HD2] != DISABLED)	opened_files[HD2] = open_log(HD2,HD2_PATH);
	if(logfile[USB] == NULL && opened_files[USB] != DISABLED)	opened_files[USB] = open_log(USB,USB_PATH);
#endif
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(time1, 100, "%d/%m/%Y %H:%M:%S:%%06d",timeinfo);
	sprintf(ttime,time1,clock()%CLOCKS_PER_SEC);
     
	switch(code){
		case WARNING: 	aux = "Warning"; 	break;
		case ERROR: 	aux = "ERROR!"; 	break;
		case FATAL:		aux = "FATAL!";		break;
		case OK:		aux = "      ";		break;
        case INFO:      aux = "Info  ";     break;
		default:		aux = "Unknown";	break;
	}

#if LOG2SCREEN
	// Writing to screen
	fprintf(stdout,"%s\t%20s\t%s\t%s\n",aux.c_str(),func.c_str(),ttime,msg.c_str()); fflush(stdout);
#endif
#if LOGGING
	// Writing to individual logs
	if(opened_files[HD1] == OK)	{fprintf(logfile[HD1],"%s\t%20s\t%s\t%s\n",aux.c_str(),func.c_str(),ttime,msg.c_str()); fflush(logfile[HD1]);}
	if(opened_files[HD2] == OK) {fprintf(logfile[HD2],"%s\t%20s\t%s\t%s\n",aux.c_str(),func.c_str(),ttime,msg.c_str()); fflush(logfile[HD2]);}
	if(opened_files[USB] == OK)	{fprintf(logfile[USB],"%s\t%20s\t%s\t%s\n",aux.c_str(),func.c_str(),ttime,msg.c_str()); fflush(logfile[USB]);}
#endif
#if DEBUG
    if (code == ERROR || code == FATAL) {
        printf("Type something to continue...\n");
        getchar();
    }
#endif
    
	// If the error is critical/fatal, shut the program down and wait something to restart it.
	if(code == FATAL)	exit(1);
	else				return code;
}

int		close_logs(void){
#if LOGGING
	for(int i=0; i < 3; i++){
        if (opened_files[i] != DISABLED) {
            fflush(logfile[i]);
            fclose(logfile[i]);
        }
	}
#endif
	return OK;
}

