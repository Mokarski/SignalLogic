#ifndef JOURNAL_H_
#define JOURNAL_H_


#define LOG_FILE_SIZE  10000 //limit to file size in bytes
//#define ALL_IN_ONE  //write ALL in one file
//#define ROTATION  //rotate log file

//code states
#define ST_NORMAL   0 //normal level
#define ST_ALARM    1 //alarm level
#define ST_WARNING  2 //warning level
#define ST_CRITICAL 3 //critical level

//long LOG_MAX_FILE_SIZE = LOG_FILE_SIZE;
int write_journal (int state_code, int err_code); // write to file cyrrent system state and error code

#endif
