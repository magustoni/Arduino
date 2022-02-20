//////////////////////////////////
//			     AIRDUINO		       	//
//		  by Irene & Mateo	     	//
//////////////////////////////////

//Pushed button --> LOW

#include <LiquidCrystal.h>

#define DELAY 100 // Delay in ms
#define TIME 30 // Game time in s

#define NEXT 16 // NEXT button
#define MORE 17 // + button
#define LESS 18 // - button
#define BUZZ 19 // Buzzer

LiquidCrystal lcd;

boolean placed[16], game[16], emptyFlag = false;
int unput = 0, players = 1, gamemode = 1, *score, *time;
long t0, time;

void setup()
{
	for (int i = 0; i < 19; i++)
		pinMode(i, INPUT_PULLUP);
	pinMode(BUZZ, OUTPUT);

	lcd.begin(20, 4);

	config();

	score = (int *)malloc(players * sizeof(int));
	time = (int *)malloc(players * sizeof(int));
}
void loop()
{
	// Beginning
	lcd.clear();
	lcd.setCursor(5, 1);
	lcd.print("Press NEXT");
	lcd.setCursor(6, 2);
	lcd.print("to begin");
	lcd.display();

	pause();

	// Game
	for (int i = 0;i < players;i++)
	{
		// If layout changes
		if (!check())
		{
			lcd.clear();
			lcd.setCursor(2, 0);
			lcd.print("Changes detected");
			lcd.setCursor(1, 1);
			lcd.print("Press NEXT to keep");
			lcd.setCursor(0, 2);
			lcd.print("this layout or reset");
			lcd.setCursor(0, 3);
			lcd.print("the previous one 2go");
			lcd.display();

			while (!check())
				if (!digitalRead(NEXT))
				{
					for (int i = 0;i < 16;i++)
						placed[i] = !digitalRead(i);
					unput = 0;
					for (int i = 0;i < 16;i++)
						if (digitalRead(i))
							unput++;
				}

			delay(DELAY);
		}

		// Gamemodes
		switch (gamemode)
		{
		case 1:
			//Time Trial
			t0 = millis();
			while (!emptyFlag)
			{
				emptyFlag = true;
				for (int j = 0;j < 16;j++)
					if (digitalRead(j) == LOW)
						emptyFlag = false;

				*(time + i) = (int)((millis() - t0) / 1000);

				lcd.clear();

				lcd.setCursor(6, 0);
				lcd.print("Player ");
				lcd.print(i + 1);

				lcd.setCursor(5, 2);
				lcd.print("Time: ");
				lcd.print(*(time + i));
				lcd.print("s");

				lcd.setCursor(2, 3);
				lcd.print("Targets left: ");
				lcd.print(16 - count());

				lcd.display();
			}
			break;
		case 2:
			//Score Dash
			while (digitalRead(NEXT) && !emptyFlag)
			{
				emptyFlag = true;
				for (int j = 0;j < 16;j++)
					if (digitalRead(j) == LOW)
						emptyFlag = false;

				*(score + i) = count();

				lcd.clear();

				lcd.setCursor(6, 0);
				lcd.print("Player ");
				lcd.print(i + 1);

				lcd.setCursor(6, 1);
				lcd.print("Score: ");
				lcd.print(count());

				lcd.setCursor(0, 2);
				lcd.print("Press NEXT when");
				lcd.setCursor(2, 3);
				lcd.print("you run out of ammo");
			}
			delay(DELAY);
			break;
		case 3:
			//Gun vs Clock
			t0 = millis();
			while ((int)((millis() - t0) / 1000) < TIME && !emptyFlag)
			{
				emptyFlag = true;
				for (int j = 0;j < 16;j++)
					if (digitalRead(j) == LOW)
						emptyFlag = false;

				*(score + i) = count();

				lcd.clear();

				lcd.setCursor(6, 0);
				lcd.print("Player ");
				lcd.print(i + 1);

				lcd.setCursor(3, 2);
				lcd.print("Time left: ");
				lcd.print(TIME - (int)((millis() - t0) / 1000));
				lcd.print("s");

				lcd.setCursor(6, 3);
				lcd.print("Score: ");
				lcd.print(count());

				lcd.display();
			}
			break;
		}

		// Next turn
		if (players > 1 && i < players - 1 && gamemode != 2)
		{
			lcd.clear();

			lcd.setCursor(3, 1);
			lcd.print("Press NEXT to");
			lcd.setCursor(2, 2);
			lcd.print("begin next turn");

			pause();
		}
	}

	end();

	pause();
}

void config()
{
	// Target acquiring
	lcd.clear();
	lcd.setCursor(3, 0);
	lcd.print("Place targets");
	lcd.setCursor(8, 1);
	lcd.print("and");
	lcd.setCursor(5, 2);
	lcd.print("Press NEXT");
	lcd.setCursor(6, 3);
	lcd.print(".......");
	lcd.display();

	pause();

	for (int i = 0;i < 16;i++) 
		placed[i] = !digitalRead(i);
	for (int i = 0;i < 16;i++)
		if (digitalRead(i))
			unput++;

	// Player registering
	while (digitalRead(NEXT))
	{
		lcd.clear();
		lcd.setCursor(6, 0);
		lcd.print("Players");

		lcd.setCursor(10, 1);
		if (((int)(millis() / 1000)) % 2 == 0)
			lcd.print(players);
		else
			lcd.print("_");

		lcd.setCursor(6, 2);
		lcd.print("Gamemode");
		lcd.setCursor(3, 3);
		lcd.print("Time Trial -->");
		lcd.display();

		if (!digitalRead(MORE) && players < 4)
			players++;
		if (!digitalRead(LESS) && players > 1)
			players--;

		if (!digitalRead(MORE) || !digitalRead(LESS))
			delay(DELAY);
	}

	delay(DELAY);

	// Gamemode choosing
	while (digitalRead(NEXT))
	{
		lcd.clear();
		lcd.setCursor(6, 0);
		lcd.print("Players");
		lcd.setCursor(10, 1);
		lcd.print(players);

		lcd.setCursor(6, 2);
		lcd.print("Gamemode");
		if (((int)(millis() / 1000)) % 2 == 0)
		{
			switch (gamemode)
			{
			case 1:
				lcd.setCursor(3, 3);
				lcd.print("Time Trial -->");
				break;
			case 2:
				if (unput == 0)
				{
					lcd.setCursor(1, 3);
					lcd.print("<-- Score Dash -->");
				}
				else
				{
					lcd.setCursor(3, 3);
					lcd.print("<-- Score Dash");
				}
				break;
			case 3:
				lcd.setCursor(2, 4);
				lcd.print("<-- Gun vs Clock");
				break;
			}
		}
		lcd.display();

		if (!digitalRead(MORE) && gamemode < 3 && unput == 0)
			gamemode++;
		if (!digitalRead(MORE) && gamemode < 2 && unput != 0)
			gamemode++;
		if (!digitalRead(LESS) && gamemode > 1)
			gamemode--;

		if (!digitalRead(MORE) || !digitalRead(LESS))
			delay(DELAY);
	}

	delay(DELAY);
}
void end()
{
	// Display results
	lcd.clear();
	if (gamemode == 1)
	{
		if (players == 1)
		{
			lcd.clear();
			lcd.setCursor(5, 1);
			lcd.print("Good aim!");
			lcd.setCursor(4, 2);
			lcd.print("Time: ");
			lcd.print(*(time));
		}
		if (players == 2)
		{
			decorate();
			lcd.setCursor(1, 1);
			lcd.print("-Player 1 Time ");
			lcd.print(*time);
			lcd.setCursor(1, 2);
			lcd.print("-Player 2 Score ");
			lcd.print(*(time + 1));
		}
		if (players == 3)
		{
			lcd.setCursor(1, 0);
			lcd.print("-Player 1 Time ");
			lcd.print(*time);
			lcd.setCursor(1, 1);
			lcd.print("-Player 2 Time ");
			lcd.print(*(time + 1));
			lcd.setCursor(1, 2);
			lcd.print("-Player 3 Time ");
			lcd.print(*(time + 2));
		}
		if (players == 4)
		{
			lcd.setCursor(1, 0);
			lcd.print("-Player 1 Time ");
			lcd.print(*time);
			lcd.setCursor(1, 1);
			lcd.print("-Player 2 Time ");
			lcd.print(*(time + 1));
			lcd.setCursor(1, 2);
			lcd.print("-Player 3 Time ");
			lcd.print(*(time + 2));
			lcd.setCursor(1, 3);
			lcd.print("-Player 4 Time ");
			lcd.print(*(time + 3));
		}
	}
	else
	{
		if (players == 1)
		{
			lcd.clear();
			lcd.setCursor(5, 1);
			lcd.print("Good aim!");
			lcd.setCursor(4, 2);
			lcd.print("Score: ");
			lcd.print(*score);
		}
		if (players == 2)
		{
			decorate();
			lcd.setCursor(1, 1);
			lcd.print("-Player 1 Score ");
			lcd.print(*score);
			lcd.setCursor(1, 2);
			lcd.print("-Player 2 Score ");
			lcd.print(*(score + 1));
		}
		if (players == 3)
		{
			lcd.setCursor(1, 0);
			lcd.print("-Player 1 Score ");
			lcd.print(*score);
			lcd.setCursor(1, 1);
			lcd.print("-Player 2 Score ");
			lcd.print(*(score + 1));
			lcd.setCursor(1, 2);
			lcd.print("-Player 3 Score ");
			lcd.print(*(score + 2));
		}
		if (players == 4)
		{
			lcd.setCursor(1, 0);
			lcd.print("-Player 1 Score ");
			lcd.print(*score);
			lcd.setCursor(1, 1);
			lcd.print("-Player 2 Score ");
			lcd.print(*(score + 1));
			lcd.setCursor(1, 2);
			lcd.print("-Player 3 Score ");
			lcd.print(*(score + 2));
			lcd.setCursor(1, 3);
			lcd.print("-Player 4 Score ");
			lcd.print(*(score + 3));
		}
	}
	lcd.display();

	pause();
}
boolean check()
{
	boolean flag = true;
	for (int i = 0;i < 16;i++)
		if (placed[i] == digitalRead(i))
			flag = false;
	return flag;
}
int count()
{
	int cont = 0;
	for (int i = 0;i < 16;i++)
		if (digitalRead(i) == HIGH)
			cont++;
	return cont - unput;
}
void pause()
{
	while (digitalRead(NEXT));
	delay(DELAY);
}
void decorate()
{
	lcd.setCursor(0, 0);
	lcd.print("********************");
	lcd.setCursor(0, 3);
	lcd.print("********************");
	lcd.setCursor(1, 1);
	lcd.print("*");
	lcd.setCursor(1, 19);
	lcd.print("*");
	lcd.setCursor(2, 1);
	lcd.print("*");
	lcd.setCursor(2, 19);
	lcd.print("*");
}
