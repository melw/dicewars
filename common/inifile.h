
FILE *iniFile;

boolean initIniFile(char *filename);
void closeIniFile();
char *readIniFileString(char *param);
int readIniFileInt(char *param);
