#include <MPU6050.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <splash.h>

#define TLOAD 3 //Loading time
#define TRESET 20 //Time while Ang>LANGLE for reset
#define LANGLE 3 //Limit angle
#define G 9.80665
//G == 1 acceleration in g
//G == 9.80665 acceleration in m/s2

Adafruit_SSD1306 display;

MPU6050 mpu;

int Ax, Ay, Az, Wx, Wy, Wz; //Raw values

float Angx, Angy, dt; //Operatings
long t0, tRes; //Time operatings

int Ang, T; //Outputs
float Acc; //Outputs

boolean AngFl, AccFl, TempFl; //Flags

void setup()
{
	Wire.begin();
	configMPU();
	configDisplay();
}

void loop()
{
	readMPU();
	calculate();
	check();

	if (millis() > 1000 * TLOAD) disp();

	delay(10);
}

//Functions

void configMPU()
{
	mpu.initialize();
	mpu.setFullScaleAccelRange(1);
	mpu.setFullScaleGyroRange(0);
}

/*
void configDisplay()
{
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.setTextWrap(false);
	display.setTextColor(WHITE);
	display.clearDisplay();

	display.setCursor(32, 23);
	display.setTextSize(1);
	display.print("by KAWASAKI");

	display.setCursor(31, 0);
	display.setTextSize(3);
	display.print("Z650");

	display.fillRect(5, 1, 10, 30, WHITE);
	display.fillRect(113, 1, 10, 30, WHITE);

	display.display();
}
*/

void readMPU()
{
	mpu.getAcceleration(&Ax, &Ay, &Az);
	mpu.getRotation(&Wx, &Wy, &Wz);
	T = mpu.getTemperature();
}

void resetMPU()
{
	mpu.reset();
	configMPU();
	Angx = 0;
	Angy = 0;
	tRes = millis();
}

void calculate()
{
	dt = (millis() - t0) / 1000.0;
	t0 = millis();

	Acc = abs(sqrt(pow(Ax, 2) + pow(Ay, 2) + pow(Az, 2)) * G / 8192.0 - G);
	Angy = 0.98 * (Angy + (Wy / 131.072) * dt) + 0.02 * atan(-Ax / sqrt(pow(Ay, 2) + pow(Az, 2))) * (180.0 / PI);
	Angx = 0.98 * (Angx + (Wx / 131.072) * dt) + 0.02 * atan(Ay / sqrt(pow(Ax, 2) + pow(Az, 2))) * (180.0 / PI);
	Ang = abs(Angy);

	T = T / 340.0 + 36.53;
}

void check()
{
	//Angle
	if (Ang > 15 && AccFl == false) AngFl = true;
	if (Ang < 8 || AccFl == true) AngFl = false;

	//Acceleration
	if (Acc > 0.35 * G) AccFl = true;
	if (Acc < 0.25 * G) AccFl = false;

	//Temperature
	if (AngFl == false && AccFl == false) TempFl = true;
	if (AngFl == true || AccFl == true) TempFl = false;

	//Correct Angle errors
	if (Ang >= LANGLE && Ang <= LANGLE + 1) tRes = millis();
	if ((millis() - tRes) / 1000 > TRESET && Ang > LANGLE + 1) resetMPU();
	if (Ang > 50) resetMPU();
}

/*
void disp()
{
	display.clearDisplay();
	display.setTextSize(3);

	//Decoration
	display.fillRect(10, 1, 5, 3, WHITE);
	display.fillRect(5, 4, 5, 3, WHITE);
	display.fillRect(10, 7, 5, 3, WHITE);
	display.fillRect(5, 10, 5, 3, WHITE);
	display.fillRect(10, 13, 5, 3, WHITE);
	display.fillRect(5, 16, 5, 3, WHITE);
	display.fillRect(10, 19, 5, 3, WHITE);
	display.fillRect(5, 22, 5, 3, WHITE);
	display.fillRect(10, 25, 5, 3, WHITE);
	display.fillRect(5, 28, 5, 3, WHITE);

	display.fillRect(113, 1, 5, 3, WHITE);
	display.fillRect(118, 4, 5, 3, WHITE);
	display.fillRect(113, 7, 5, 3, WHITE);
	display.fillRect(118, 10, 5, 3, WHITE);
	display.fillRect(113, 13, 5, 3, WHITE);
	display.fillRect(118, 16, 5, 3, WHITE);
	display.fillRect(113, 19, 5, 3, WHITE);
	display.fillRect(118, 22, 5, 3, WHITE);
	display.fillRect(113, 25, 5, 3, WHITE);
	display.fillRect(118, 28, 5, 3, WHITE);

	display.fillRect(20, 1, 88, 1, WHITE);
	display.fillRect(20, 30, 88, 1, WHITE);

	//Acceleration
	if (AccFl == true)
	{
		if (G == 1) {
			display.setCursor(33, 5);
			display.print((int)Acc);
			display.print(".");
			display.print((int)((Acc - (int)Acc) * 10));
			display.print("g");
		}
		else {
			if (Acc > 9) display.setCursor(25, 5);
			else display.setCursor(33, 5);
			display.print((int)Acc);
			display.setTextSize(2);
			display.print("m/s2");
		}
	}

	//Angle
	if (AngFl == true)
	{
		if (Ang > 9) display.setCursor(40, 5);
		else display.setCursor(50, 5);
		display.print(Ang);
		display.setTextSize(2);
		display.print((char)167);
	}

	//Temperature
	if (TempFl == true)
	{
		display.setCursor(35, 5);
		display.print(T);
		display.setTextSize(2);
		display.print((char)167);
		display.setTextSize(3);
		display.print("C");
	}

	display.display();
}
*/