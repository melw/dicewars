void kbdInit()
{
	logPrintf("Init keyboard!\n");
	int i;
	
	kbdString = (char*)malloc(255);
	memset(kbdString, 0x20, 255);
	
	for(i=0;i<KBD_NUM_BUTTONS;i++)
	{
		KBD_BUTTON_X[i] = -1;
		KBD_BUTTON_Y[i] = -1;
		KBD_BUTTON_WIDTH[i] = -1;
		KBD_BUTTON_HEIGHT[i] = -1;
	}
	
	for(i=0;i<KBD_ARRAY_LENGTH;i++)
	{
		if(KBD_ARRAY_DATA[i]<KBD_NUM_BUTTONS)
		{
			if(KBD_BUTTON_X[KBD_ARRAY_DATA[i]]==-1) // first occurence, upper left corner
			{
				KBD_BUTTON_X[KBD_ARRAY_DATA[i]] = i%IMAGE_KBD_LOWER_WIDTH;
				KBD_BUTTON_Y[KBD_ARRAY_DATA[i]] = i/IMAGE_KBD_LOWER_WIDTH;
			}
		}
	}
	for(i=KBD_ARRAY_LENGTH-1;i>=0;i--)
	{
		if(KBD_ARRAY_DATA[i]<KBD_NUM_BUTTONS)
		{
			if(KBD_BUTTON_WIDTH[KBD_ARRAY_DATA[i]]==-1) // first occurence, lower right corner
			{
				KBD_BUTTON_WIDTH[KBD_ARRAY_DATA[i]] = (i%IMAGE_KBD_LOWER_WIDTH)-KBD_BUTTON_X[KBD_ARRAY_DATA[i]]+1;
				KBD_BUTTON_HEIGHT[KBD_ARRAY_DATA[i]] = (i/IMAGE_KBD_LOWER_WIDTH)-KBD_BUTTON_Y[KBD_ARRAY_DATA[i]]+1;
			}
		}
	}

	kbdInited = true;	
}

void kbdPressed()
{
	if(kbdDown<KBD_NUM_BUTTONS)
	{
		playSound(SOUND_KEYBOARD_INDEX);
		kbdUpdated = true;
		char c = KBD_LOWER_BUTTONS[kbdDown];
		if(c==KBD_BACKSPACE)
		{
			if(kbdIndex>0)
			{
				kbdString[kbdIndex] = ' ';
				kbdIndex--;
				kbdString[kbdIndex] = ' ';
			}
		}
		else if(c==KBD_CAPS) kbdCaps = !kbdCaps;
		else if(c==KBD_SHIFT) kbdShift = true;
		else if(c==KBD_ENTER)
		{
			if(prevState==GAME_STATE_MENU) // return correct value to whatever we were editing
			{
				menuUpdated = true;
				char sPort[5]; memset(sPort, 0, 5);
				for(int i=0;i<255;i++)
				{
					if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_PLAYER_NAME)
					{
						if(i<kbdIndex)
						{
							playerName[kbdPlayerIndex][i] = kbdString[i];
						}
						else
						{
							playerName[kbdPlayerIndex][i] = 0;
							#ifdef LOBBY
								LOBBY_SetOwnName(playerName[kbdPlayerIndex]);
							#endif
							break;
						}
					}
					else if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_SERVER_ADDRESS)
					{
						if(i<kbdIndex)
						{
							serverAddress[i] = kbdString[i];
						}
						else
						{
							serverAddress[i] = 0;
							break;
						}
					}
					else if(getMenuData(currentMenu, currentMenuIndex)==MENU_EDIT_SERVER_PORT)
					{
						if(i<kbdIndex)
						{
							sPort[i] = kbdString[i];
						}
						else
						{
							serverPort = atoi(sPort);
							break;
						}
					}
				}				
			}
			gameState = prevState;
		}
		else if(kbdIndex<MAX_NAME_LENGTH)
		{
			if(kbdCaps||kbdShift)
			{
				c = KBD_UPPER_BUTTONS[kbdDown];
				if(kbdShift) kbdShift = false;
			}
			kbdString[kbdIndex] = c;
			kbdIndex++;
		}
		kbdDown = 255;
	}
}

void kbdLogic()
{
	if(!kbdInited) return;
	if(pointerDown)
	{
		if(pointerX>=kbdAnchorX&&pointerX<kbdAnchorX+IMAGE_KBD_LOWER_WIDTH&&
		   pointerY>=kbdAnchorY&&pointerY<kbdAnchorY+IMAGE_KBD_LOWER_HEIGHT) // pointer is in the keyboard area
		{
			int val = KBD_ARRAY_DATA[(pointerX-kbdAnchorX)+(pointerY-kbdAnchorY)*IMAGE_KBD_LOWER_WIDTH];
			if(val<KBD_NUM_BUTTONS) // key pressed
			{
				kbdDown = val;
				kbdUpdated = true;
			}
		}
	}
	if((game[LOCAL].gameTime>>9)%2==0) kbdString[kbdIndex] = '_';
	else kbdString[kbdIndex] = ' ';
}

void kbdPaint()
{
	if(!kbdInited) return;
	if(kbdUpdated)
	{
		drawImage(IMAGE_INSTRUCTIONS_BG_INDEX, 0, 0, ALIGN_LT);
		drawImage(IMAGE_KBD_LOWER_INDEX+(kbdCaps|kbdShift)*2, kbdAnchorX, kbdAnchorY, ALIGN_LT);
		kbdUpdated = false;
	}
	
	if(kbdDown<KBD_NUM_BUTTONS) // draw pressed key
	{
		drawSubImage(IMAGE_KBD_LOWER2_INDEX+(kbdCaps|kbdShift)*2, kbdAnchorX+KBD_BUTTON_X[kbdDown], kbdAnchorY+KBD_BUTTON_Y[kbdDown], ALIGN_LT, KBD_BUTTON_WIDTH[kbdDown], KBD_BUTTON_HEIGHT[kbdDown], KBD_BUTTON_X[kbdDown], KBD_BUTTON_Y[kbdDown]);
		//drawSubImage(IMAGE_DICE_SMALL_INDEX, xAnchor+xPos+x*hexagonWidth+hexagonWidth+xAdd, yAnchor+yPos+y*hexagonHeight+hexagonHeight-(i%4)*4+yAdd, ALIGN_HCVC, 9, 9, game[LOCAL].playerArea[area]*9, 0);
	}
	
	setColor(0);
	drawSubImage(IMAGE_INSTRUCTIONS_BG_INDEX, 28, 25, ALIGN_LT, 200, FONT_TRUETYPE_HEIGHT, 28, 25);
	drawString(FONT_TRUETYPE, kbdString, 28, 25, ALIGN_LT);	
}
