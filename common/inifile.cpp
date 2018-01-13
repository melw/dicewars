
boolean seekLine(char *line, char *param)
{
	int i;
	
	fseek(iniFile, 0, SEEK_SET);
	memset(line, 0, 256);
	while(fgets(line, 256, iniFile)!=NULL)
	{
		if(strlen(line)>strlen(param)) // check if there's a param
		{
			if(strlen(line)>0&&line[0]!='#') // not a comment line
			{
				boolean foundLine = true;
				for(i=0;i<(int)strlen(param);i++)
				{
					if(line[i]!=param[i])
						foundLine = false;
				}
				if(foundLine)
					return true;
			}
		}
		memset(line, 0, 256);
	}
	
	return false;
}

boolean initIniFile(char *filename)
{
	if(iniFile==NULL)
		iniFile = fopen(filename, "r");
	
	if(iniFile==NULL)
	{
		logPrintf("!!! Failed opening .ini file!\n");
		return false;
	}
	
	return true;
}

void closeIniFile()
{
//	logPrintf("Close .ini file\n");
	if(iniFile!=NULL)
		fclose(iniFile);
}

char* readIniFileString(char *param)
{
	int i,j;
	if(iniFile==NULL) return NULL;
	char line[256];

	char *value = (char*)malloc(256); memset(value, 0, 256);
	
	if(!seekLine(line, param))
		return NULL;
		
	boolean parse = false;
	boolean strip = false;
	j=0;
	for(i=0;i<256;i++)
	{
		if(parse)
		{
			if(line[i]!=' ')
				strip = true;
			
			if(strip && line[i] > 0x13)
			{
				value[j] = line[i];
				j++;
			}
		}
		if(line[i]=='=')
			parse = true;
	}

	return value;
}

int readIniFileInt(char *param)
{
	int i,j;
	if(iniFile==NULL) return -1;
	char line[256];
	
	if(!seekLine(line, param))
		return -1;
		
	char value[16]; memset(value, 0, 16);
	boolean parse = false;
	boolean strip = false;
	j=0;
	for(i=0;i<(int)strlen(line);i++)
	{
		if(parse)
		{
			if(line[i]!=' ')
				strip = true;
			
			if(strip && line[i] > 0x13)
			{
				value[j] = line[i];
				j++;
			}
		}
		if(line[i]=='=')
			parse = true;
	}
	
	return atoi(value);
}
