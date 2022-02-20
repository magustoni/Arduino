#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>
#include <SPI.h>
#include <stdlib.h>

#define TCUARTO 600 //s
#define TBOCINA 2 //s
#define DELAY 350 //ms
#define CUARTOS 4

typedef struct {
	int puntosA, puntosB;
}partido;	

void Bocina();
void Encender(int);
void Apagar(int);
void FinCuarto(boolean *crono, byte *faltasA, byte *faltasB, float *time, byte *cuarto, boolean *faltasAFl, boolean *faltasBFl, boolean *dispCuartoFl);
void FinPartido(byte *cuarto, int *puntosA, int*puntosB, boolean *dispPuntosFl, boolean *dispCuartoFl, Adafruit_SSD1306 *display, boolean *displayFl);
void Save(int puntosA, int puntosB, Adafruit_SSD1306 *display, boolean *displayFl, boolean *dispPuntosFl, boolean *dispCuartoFl);

void setup(){

	pinMode(2, INPUT); //Reset
	pinMode(3, INPUT); //Falta A
	pinMode(4, INPUT); //Falta B
	pinMode(11, OUTPUT); //Bocina auto
	pinMode(12, INPUT); //Continuar / mostrar siguiente partido
	pinMode(13, INPUT); //Play / pause
	pinMode(A0, INPUT); //Puntos A++
	pinMode(A1, INPUT); //Puntos A--
	pinMode(A2, INPUT); //Puntos B++
	pinMode(A3, INPUT); //Puntos B--
	Apagar(11);
}

void loop() {

	//Asignaciones iniciales
	static float time = 0, instI = 0, instF = 0;
	static boolean crono = false, displayFl = true, dispPuntosFl = true, dispCuartoFl = true, faltasAFl = true, faltasBFl = true;
	static int puntosA = 0, puntosB = 0, tCuarto;
	static byte cuarto = 5 - CUARTOS, faltasA = 0, faltasB = 0;
	static Adafruit_SSD1306 display;

	if (displayFl == true) { //Inicio del display

		delay(100);
		display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
		display.clearDisplay();
		display.display();
		display.setTextSize(2);
		display.setTextColor(WHITE);
		display.setRotation(0);
		display.setTextWrap(false);
		display.dim(0);
		display.setCursor(10, 10);
		display.print("A");
		display.setCursor(110, 10);
		display.print("B");
		displayFl = false;
	}

	//Tiempo
	if (digitalRead(13) == HIGH && crono == false) { //Play time

		crono = true;
		delay(DELAY);
	}
	if (digitalRead(13) == HIGH && crono == true) { //Pause time

		crono = false;
		delay(DELAY);
	}

	if (digitalRead(2) == HIGH) { //Reset time

		time = 0;
		crono = false;
	}

	instI = instF;
	instF = millis() / 1000;
	tCuarto = TCUARTO - time;
	if (crono == true) //Cuenta tiempo (si activado)
		time += instF - instI;

	//Puntos equipo A
	if (digitalRead(A0) == HIGH) { //Puntos +1

		puntosA++;
		delay(DELAY);
		dispPuntosFl = true;
	}
	if (digitalRead(A1) == HIGH) { //Puntos -1

		puntosA--;
		delay(DELAY);
		dispPuntosFl = true;
	}
	if (puntosA < 0)
		puntosA = 0;

	//Puntos equipo B
	if (digitalRead(A2) == HIGH) { //Puntos +1

		puntosB++;
		delay(DELAY);
		dispPuntosFl = true;
	}
	if (digitalRead(A3) == HIGH) { //Puntos -1

		puntosB--;
		delay(DELAY);
		dispPuntosFl = true;
	}
	if (puntosB < 0)
		puntosB = 0;

	//Faltas equipo A
	if (digitalRead(3) == HIGH) { //Faltas +1

		faltasA++;
		delay(DELAY);
		faltasAFl = true;
	}

	if (faltasAFl == true) { //LEDs faltas (tecnica charlieplexing)
		if (faltasA == 0) {
			pinMode(5, OUTPUT);
			Encender(5);
			pinMode(6, OUTPUT);
			Apagar(6);
			pinMode(7, INPUT);
			delay(DELAY);
			faltasAFl = false;
		}
		if (faltasA == 1) {
			pinMode(7, OUTPUT);
			Encender(7);
			pinMode(5, OUTPUT);
			Apagar(5);
			pinMode(6, INPUT);
			delay(DELAY);
			faltasAFl = false;
		}
		if (faltasA == 2) {
			pinMode(6, OUTPUT);
			Encender(6);
			pinMode(7, OUTPUT);
			Apagar(7);
			pinMode(5, INPUT);
			delay(DELAY);
			faltasAFl = false;
		}
		if (faltasA == 3) {
			pinMode(5, OUTPUT);
			Encender(5);
			pinMode(7, OUTPUT);
			Apagar(7);
			pinMode(6, INPUT);
			delay(DELAY);
			faltasAFl = false;
		}
		if (faltasA == 4) {
			pinMode(6, OUTPUT);
			Encender(6);
			pinMode(5, OUTPUT);
			Apagar(5);
			pinMode(7, INPUT);
			delay(DELAY);
			faltasAFl = false;
		}
		if (faltasA == 5) {
			pinMode(7, OUTPUT);
			Encender(7);
			pinMode(6, OUTPUT);
			Apagar(6);
			pinMode(5, INPUT);
			delay(DELAY);
			faltasAFl = false;
		}
	}
	if (faltasA >5)
		faltasA = 5;

	//Faltas equipo B
	if (digitalRead(4) == HIGH) { //Faltas +1

		faltasB++;
		delay(DELAY);
		faltasBFl = true;
	}

	if (faltasBFl == true) { //LEDs faltas (tecnica charlieplexing)
		if (faltasB == 0) {
			pinMode(8, OUTPUT);
			Encender(8);
			pinMode(9, OUTPUT);
			Apagar(9);
			pinMode(10, INPUT);
			delay(DELAY);
			faltasBFl = false;
		}
		if (faltasB == 1) {
			pinMode(10, OUTPUT);
			Encender(10);
			pinMode(8, OUTPUT);
			Apagar(8);
			pinMode(9, INPUT);
			delay(DELAY);
			faltasBFl = false;
		}
		if (faltasB == 2) {
			pinMode(9, OUTPUT);
			Encender(9);
			pinMode(10, OUTPUT);
			Apagar(10);
			pinMode(8, INPUT);
			delay(DELAY);
			faltasBFl = false;
		}
		if (faltasB == 3) {
			pinMode(8, OUTPUT);
			Encender(8);
			pinMode(10, OUTPUT);
			Apagar(10);
			pinMode(9, INPUT);
			delay(DELAY);
			faltasBFl = false;
		}
		if (faltasB == 4) {
			pinMode(9, OUTPUT);
			Encender(9);
			pinMode(8, OUTPUT);
			Apagar(8);
			pinMode(10, INPUT);
			delay(DELAY);
			faltasBFl = false;
		}
		if (faltasB == 5) {
			pinMode(10, OUTPUT);
			Encender(10);
			pinMode(9, OUTPUT);
			Apagar(9);
			pinMode(8, INPUT);
			delay(DELAY);
			faltasBFl = false;
		}
	}
	if (faltasB > 5)
		faltasB = 5;

	//Display
	if (tCuarto == 600) {//Tiempo

		display.fillRect(35, 15, 70, 25, BLACK);
		display.setCursor(35, 25);
		display.print("10:00");
	}
	else {

		display.fillRect(35, 15, 70, 25, BLACK);
		display.setCursor(35, 25);
		display.setTextColor(WHITE);
		display.print("0");
		display.print((int)tCuarto / 60);
		display.print(":");
		if ((tCuarto % 60) < 10) {

			display.print("0");
			display.print((int)tCuarto % 60);
		}
		else {

			display.print((int)(tCuarto % 60));
		}
	}

	display.display();

	if (dispCuartoFl == true) { //Cuarto

		display.fillRect(60, 40, 20, 25, BLACK);
		display.setCursor(60, 40);
		display.print(cuarto);
		display.display();
		dispCuartoFl = false;
	}

	if (dispPuntosFl == true) { //Puntos A y B

		display.fillRect(5, 45, 25, 25, BLACK);
		if (puntosA < 10) {
			display.setCursor(10, 45);
			display.print(puntosA);
		}
		else {
			if (puntosA < 100) {
				display.setCursor(5, 45);
				display.print(puntosA);
			}
			else {
				display.setCursor(0, 45);
				display.print(puntosA);
			}
		}

		display.fillRect(105, 45, 25, 25, BLACK);
		if (puntosB < 10) {
			display.setCursor(110, 45);
			display.print(puntosB);
		}
		else {
			if (puntosB < 100) {
				display.setCursor(105, 45);
				display.print(puntosB);
			}
			else {
				display.setCursor(100, 45);
				display.print(puntosB);
			}
		}
		display.display();
		dispPuntosFl = false;
	}

	//Final
	if (tCuarto == 0 && crono == true)
		FinCuarto(&crono, &faltasA, &faltasB, &time, &cuarto, &faltasAFl, &faltasBFl, &dispCuartoFl);
	if (cuarto > 4)
		FinPartido(&cuarto, &puntosA, &puntosB, &dispPuntosFl, &dispCuartoFl, &display, &displayFl);
}

//Funciones
void Bocina() {
	Encender(11);
	delay(1000 * TBOCINA);
	Apagar(11);
}
void Encender(int Pin) {
	digitalWrite(Pin, HIGH);
}
void Apagar(int Pin) {
	digitalWrite(Pin, LOW);
}
void FinCuarto(boolean *crono, byte *faltasA, byte *faltasB, float *time, byte *cuarto, boolean *faltasAFl, boolean *faltasBFl, boolean *dispCuartoFl){

	*crono = false;
	Bocina();
	*faltasA = 0;
	*faltasB = 0;
	*time = 0;
	*cuarto = *cuarto + 1;
	*faltasAFl = true;
	*faltasBFl = true;
  if(*cuarto < 5)
	  *dispCuartoFl = true;
}
void FinPartido(byte *cuarto, int *puntosA, int*puntosB, boolean *dispPuntosFl, boolean *dispCuartoFl, Adafruit_SSD1306 *display, boolean *displayFl) {

	Save(*puntosA, *puntosB, display, displayFl, dispPuntosFl, dispCuartoFl);
	*cuarto = 5 - CUARTOS;
	*puntosA = 0;
	*puntosB = 0;
	*dispPuntosFl = true;
	*dispCuartoFl = true;
}
void Save(int puntosA, int puntosB, Adafruit_SSD1306 *display, boolean *displayFl, boolean *dispPuntosFl, boolean *dispCuartoFl) {

	static int cont = 0;
	cont++;

	//Guardar resultados
	partido *log, *aux;
	if (cont == 1) {
		log = (partido *)malloc(sizeof(partido));
	}
	if (cont == 2) {
		aux = (partido *)malloc(sizeof(partido));
		aux[0].puntosA = log[0].puntosA;
		aux[0].puntosB = log[0].puntosB;
		log = (partido *)realloc(log, 2 * sizeof(partido));
		log[0].puntosA = aux[0].puntosA;
		log[0].puntosB = aux[0].puntosB;
	}
	if(cont>2) {
		aux = (partido*)realloc(aux, (cont - 1) * sizeof(partido));
		for (int i = 0;i < cont - 1;i++) {
			aux[i].puntosA = log[i].puntosA;
			aux[i].puntosB = log[i].puntosB;
		}
		log = (partido *)realloc(log, cont * sizeof(partido));
		for (int i = 0;i < cont - 1;i++) {
			log[i].puntosA = aux[i].puntosA;
			log[i].puntosB = aux[i].puntosB;
		}
	}

	log[cont-1].puntosA = puntosA;
	log[cont-1].puntosB = puntosB;

	//Mostrar resultados guardados por pantalla
	(*display).fillRect(35, 15, 70, 40, BLACK);
	(*display).setCursor(36, 15);
	(*display).print("Pulse");
	(*display).setCursor(41, 35);
	(*display).print("CONT");
	(*display).display();

	int k = 0;
	while (k<cont) {

		if (digitalRead(12) == HIGH) {
			(*display).clearDisplay();
			(*display).setCursor(15,20);
			(*display).print("Partido ");
			(*display).print(k+1);
			if (log[k].puntosA > 9)
				if(log[k].puntosB > 9)
					(*display).setCursor(25, 40);
				else
					(*display).setCursor(30, 40);
			else
				(*display).setCursor(35, 40);
			(*display).print(log[k].puntosA);
			(*display).print(" - ");
			(*display).print(log[k].puntosB);
			(*display).display();
			delay(DELAY);
			k++;
			while (digitalRead(12) != HIGH) {}
		}
	}

	//Flags para reiniciar display
	*displayFl = true;
	*dispPuntosFl = true;
	*dispCuartoFl = true;
}
