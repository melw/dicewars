/** 
  
  dicewars ds game & player statistics theme for server 
  
- 50% share: DOM (land mass)
- 50% share: placing (1..8)
- each turn: put 2 points into pot - can't go into minus from 0
- surrender: surrenders immediately, land turns into gray and inactive and placing is given accordingly
- can't surrender for the first 3 rounds, being away for 3 turns, should also autoflag a player

// kdice scoring
Q: What's this I hear about a new scoring system? The new scoring system is somewhat like gpokr. You can think of the points as money: the only way to get them
is to win them directly from other players. Everyone starts at 0, and you're not allowed to go negative, so the only place to go is up! 

Q: So how exactly do
you win points? At the beginning of the game, every player puts a few points into the pot as a buy-in. Then every round, you pay 2 more points of ante. When
you leave the game, get killed, or win, you get to claim a certain percentage of the pot, your "share". Half of your share is based on DOM (that is, your
average land count at the beginning of your turn throughout the game), and the other half is based on your finish (7th through 1st). 

Q: Wait, you said I have
to pay 2 points per round? That's right. That means the longer you stay in the game, the more total points you end up paying into the pot. Which is fine if you
do well, because you get them all back, with interest. But if do poorly, you can lose a lot of points. It's better to get out of a game early if you're doing
really badly to minimize your losses. On the higher tables, it scales up; for example, at the 100 tables you pay 4 points per round, so you can win more and
also lose more. 

Q: OMG, people will just leave me alive with 1 land and steal all my points! This is terrible! Oh, I forgot to explain about flagging. The old
concept of flagging has been totally changed. In the new game, flagging means that you resign the game immediately. You are instantly removed from the game,
and all of your lands and dice turn grey (neutral). You get last place among the players still alive: if there are 5 players in the game when you flag, you get
5th place, no matter how many lands you have. 

Q: Wow, so flagging is like instadeath? But that means no more ninjas, no more flag whining, and no more dom stealing! Um, yeah, it does mean that. :) 

Q: So like, how do I know when I should flag? Generally speaking, you should flag if you're in last place and don't
have a reasonable chance of getting a higher place. Sometimes the best time to flag is right after someone else flags. Now for those of you who love to know
the details, here's another handy rule of thumb: if your eventual finish is in the upper half of the remaining players, you're generally gaining points each
round. If your eventual finish is in the lower half of the remaining players, you're generally losing points each round. 

Q: Anything else I should know? You can't flag until round 3. Being "away" for three turns in row will autoflag you. Finally, being at -15 points at the end 
of your turn will autoflag you (this is to prevent abuse).


*/


struct gameStatistics
{
  int gamesPlayed;
  int openGames;
  int activeGames;
  int lastPlayerConnected;
  int lastPlayerConnectedBuffer;
  int lastGameStarted;
  int lastGameEnded;
  int playersOnline;
  int serverStarted;
};

