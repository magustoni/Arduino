
//Limites Pot1
#define Min1 295
#define Max1 1010 

//Limites Pot2
#define Min2 5
#define Max2 1020 

#include <LiquidCrystal_I2C.h>
#include <Keypad.h> //Necesaria descarga libreria Keypad.h
#include <PID_v1.h>

//Pines
#define PIN_INPUT1 0 //Input del potenciometro 1 entrada analogica
#define PIN_INPUT2 1 //Input del potenciometro 2 entrada analogica
#define PIN_OUTPUT1 3 //Output del motor 1 salida PWM
#define PIN_OUTPUT2 5 //Output del motor 2 salida PWM
#define PIN_CAMBIO11 2 //Pin de cambio de sentido del motor 1 salida digital
#define PIN_CAMBIO12 4 //Pin de cambio de sentido del motor 1 salida digital
#define PIN_CAMBIO21 7 //Pin de cambio de sentido del motor 2 salida digital
#define PIN_CAMBIO22 8 //Pin de cambio de sentido del motor 2 salida digital
#define BOMBA 12 //Pin de activacion de la bomba de agua


//Constantes
#define pi 3.14159
#define FILAS 4    //Filas del teclado matricial
#define COLUMNAS 3  //Columnas del teclado matricial
#define Coord_Length 13 // Pares de coordenadas + 1

//Constantes proporcionales PID
double Kp1=0.52, Ki1=0.45, Kd1=0.05;
double Kp2=0.25, Ki2=0.44, Kd2=0.07;

//Variables

int Estados=0; //Control de estados
int vasos=0,aux_v=1, aux_c=0; //Axuiliares para el control de los ciclos
int i=0,x,y; //Coordenadas para la cinematica inversa
double Setpoint1, Setpoint2; //Valor al que tiene que llegar el input
double Pot1, Pot2; //Variable que se intenta controlar
double Motor1, Motor2; //Variable que se ajusta al pid
double valor1, valor2, v; //Valor del potenciometro en angulo

//Variables de temporizacion
double tiempo1 = 0, tiempo2 = 0, tiempo3 = 0, tiempobomba=0, tiempobombaoff=0, instI = 0, instF = 0;

//Variables de coordenadas y pantallas LCD
int Data[Coord_Length];
int Result[Coord_Length];
int Coord_Less=0;
int pantalla=0;
int Coordenadas[4];

// Character to hold coordinates input
//linea para los setCursor
int linea=1;
int columna=15;
// Counter for character entries
byte data_count = 0;
// Character to hold key input
char tecla;



//Definir el teclado
static char teclado[FILAS][COLUMNAS] = {
{ '1','2','3' },
{ '4','5','6' },
{ '7','8','9' },
{ '*','0','#' }
};

//Definiir los pines del arduino
static byte pinColumnas[COLUMNAS] = { 32,30,28 }; 
static byte pinFilas[FILAS] = { 40,38,36,34 };

static Keypad miteclado = Keypad{ makeKeymap(teclado), pinFilas, pinColumnas, FILAS, COLUMNAS }; //rastreo del teclado

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,20,4);  //


//Definir los PID de regulacion con sus respectivas variables
PID myPID1(&Pot1, &Motor1, &Setpoint1, Kp1, Ki1, Kd1, DIRECT);
PID myPID2(&Pot2, &Motor2, &Setpoint2, Kp2, Ki2, Kd2, DIRECT);



void setup()
{
  Serial.begin(9600);

  //Establecer entradas y salidas
  pinMode(PIN_INPUT1,INPUT);
  pinMode(PIN_INPUT2,INPUT);  
  pinMode(PIN_OUTPUT1,OUTPUT);
  pinMode(PIN_OUTPUT2,OUTPUT);
  pinMode(PIN_CAMBIO11,OUTPUT);
  pinMode(PIN_CAMBIO12,OUTPUT);
  pinMode(PIN_CAMBIO21,OUTPUT);  
  pinMode(PIN_CAMBIO22,OUTPUT);
  pinMode(BOMBA,OUTPUT);

//Iniciar variables y funciones PID
//Iniciar LCD
inicializa();
}

void loop()
{
  //Iniciar temporizadores
   instI=instF;
   instF=millis();

  
  //Imprimir variables de los potenciometros 
   if(random(0,100)<20){
  //Pot1
  valor1=Pot1;
  valor2=Pot2;
  

  //Imprimimos por el monitor serie
  Serial.print(valor1);
  Serial.print(" , ");
  Serial.print(valor2);
  Serial.print("        ");
  Serial.print(Setpoint1);
  Serial.print(" , ");
  Serial.println(Setpoint2);
}
  
  //Control de estados
  switch (Estados)
  {
    case 0: //Pedir coordenadas por teclado matricial
    coord();
    break;
    
    case 1: //Pasar las variables del vector coordenadas a x, y
    x=Result[aux_c]*10+Result[aux_c+1]-15;
    y=Result[aux_c+2]*10+Result[aux_c+3]+22;
   
    pid(x,y); //Iniciar regulacion mediante PID
    break;
    
    case 2: //Regar con la bomba
    regar();
    break;
  }
}
void inicializa()
{
   // Inicializar el LCD
  lcd.init();
  
  //Encender la luz de fondo.
  lcd.backlight();
  lcd.clear();
  
  //Activar la bomba
  analogWrite(11,100);

  //Iniciar PID
    myPID1.SetSampleTime(100);//Establecer en ms cada cuanto tiempo el pid evalua
  Pot1 = analogRead(PIN_INPUT1);
 
  myPID2.SetSampleTime(100);//Establecer en ms cada cuanto tiempo el pid evalua
  Pot2 = analogRead(PIN_INPUT1);

  //Llamar a la funcion PID (Activado)
  myPID1.SetMode(AUTOMATIC);
  myPID2.SetMode(AUTOMATIC);
}


void coord()
{
   // Busqueda de teclas introducidas, se guardarán en la variable tecla.
   tecla = miteclado.getKey();
   

  if (pantalla==0){      // Pantalla 0                                /////////////////////////
      lcd.setCursor(3, 1);                                            //                     //
      lcd.print("Presione # para");                                   //   Presione # para   //     
      lcd.setCursor(5, 2);                                            //      comenzar       //
      lcd.print("comenzar");                                          //                     // 
  if (tecla=='#'){     // Pantalla 0 -> Pantalla 1.                   /////////////////////////
    pantalla=1; 
    lcd.clear();
    }
  }

  if(pantalla==1){
      lcd.setCursor(1, 1);                                            /////////////////////////
      lcd.print(" How many plants");                                  //                     //
      lcd.setCursor(0, 2);                                            //  How many plants    //
      lcd.print("do you want to water ");                             //do you want to water //
      lcd.setCursor(10, 3);                                           //         ?           //
      lcd.print("?");                                                 /////////////////////////
      
    switch ( tecla ){       //Casos de 1, 2 ó 3 vasos (coordenadas).
    
    case '1':   
        vasos=1;            //1 vaso
        Coord_Less=8;       //Variable que modifica la longitud final del vector de coordenadas, en este caso restará 8 dígitos.
        lcd.clear();        
        pantalla=2;         // Pantalla 1 -> Pantalla 2.
        break;
       
    case '2':   
        vasos=2;            //2vasos
        Coord_Less=4;       //Variable que modifica la longitud final del vector de coordenadas, en este caso restará 4 dígitos.
        lcd.clear();
        pantalla=3;         // Pantalla 1 -> Pantalla 3.
        break;
        
    case '3': 
        vasos=3;           //3vasos
        Coord_Less=0;      //Variable que modifica la longitud final del vector de coordenadas, en este caso no restará dígitos.
        lcd.clear();
        pantalla=4;        // Pantalla 1 -> Pantalla 4.
        break;
        
    }  
  }
        
  if(pantalla>=2){        // Pantalla 2: caso de 1 solo vaso.    //
                                                                 //
      lcd.setCursor(0, 0);                                       //
      lcd.print("Set Coordinates:");                             //
      lcd.setCursor(0, 1);                                       //
      lcd.print("Coordinates 1:");    // 1 vaso -> 1 coordenada. //
                                                             
      if(pantalla>2){     // si pantalla=3: añade la linea para  // 
      lcd.setCursor(0, 2);// pedir las coordenadas del 2do vaso. //  
      lcd.print("Coordinates 2:");  // 2 vasos -> 2 coordenadas. //
      }
      if(pantalla>3){     // si pantalla=4: añade la linea para  // 
      lcd.setCursor(0, 3);// pedir las coordenadas del 3er vaso. //
      lcd.print("Coordinates 3:");  // 3 vasos -> 3 coordenadas. //
      }
      lcd.setCursor(0, linea);  // Formato de visualización de las coordenadas que vamos introduciendo //
                                                                                                       //
      if (tecla=='*')           // tecla * equivale a espacio (' ')                                    //   
      {                                                                                                //
      columna++;                                                                                       //
      }                                                                                                //
                                                                                                       //      
      if (tecla=='#')           // tecla # equivale a intro (salto de linea)                           //
      {                                                                                                //
      columna=15;                                                                                      //
      linea++;                                                                                         //
      }                                                                                        //////////
    
  if (tecla != '#' && tecla != '*' && tecla != NO_KEY){
   // Introducimos las coordenadas que van conformando el vector Data.
    int numero= tecla - 48;              // Conversión de caracter a numero entero (ascii)
    Data[data_count] = numero;           // numero -> al vector
    lcd.setCursor(columna, linea);       ////   Este proceso se ejecutara por cada pulsación de tecla 
    if(data_count>0){                    ////   siempre que cumpla las condiciones.
    lcd.print(Data[data_count]);         //
    columna++;                           //
    }                             
    data_count++;
  }
  
}
    // Limite de introducción de coordenadas:
     
  if (data_count == Coord_Length - Coord_Less ) {     //Cuando se haya rellenado todas las coordenadas
    lcd.clear();                                      //Paramos la introducción 
    lcd.setCursor(0, 0);
    lcd.print("Resultado:");
    lcd.setCursor(0, 2);
    for(int y=0;y<Coord_Length - Coord_Less -1 ;y++){ //Conformamos el vector resultado que contiene
    Result[y]=Data[y+1];                              //Las coordenadas en el formato adecuado
    lcd.print(Result[y]);
    lcd.setCursor(y+1, 2);
    Estados=1;                                        //Empezamos el proceso de máquina de estados.
    }
    lcd.clear();
    }
}

//Cinematica inversa
void ikine(double x, double y, double* q1, double* q2) 
{
  float aux=x;
  x=y;
  y=-aux;
  float L1 = 25.5, L2 = 26;
  float A = pow(x, 2);
  A += pow(y, 2);
  A += -2 * L1 * L2;
  A /= 2 * L1 * L2;
  *q2 = atan2(sqrt(1 - pow(A, 2)), A);  //atan(num,den)
  if (*q2 < -2.3562 || *q2 > 2.3562) {
    *q2 = atan2(-sqrt(1 - pow(A, 2)), A);
  }
  *q1 = atan2(y, x) - atan2(sin(*q2), 1 + cos(*q2))+pi/2;
  (*q2)=(*q2)*180/pi;
   (*q1)=(*q1)*180/pi;
  (*q2)=(*q2)*(Max2-Min2)/290+515;
  (*q1)=(*q1)*(Max1-Min1)/180+Min1;
}

//Regulacion PID
void pid(int x, int y)
{ 
  //Activacion de los temporizadores por ciclos
   if(aux_v==1) //Ciclo 1
    tiempo1+=instF-instI;
    
   if(aux_v==2) //Ciclo 2
    tiempo2+=instF-instI;
    
   if(aux_v==3) //Ciclo 3
    tiempo3+=instF-instI;

 //Llamada a la cinematica inversa en funcion de las coordenadas
  ikine(x,y,&Setpoint1, &Setpoint2); //Devuelve las posiciones articulares

  
   //Control motor 1
  Pot1 = analogRead(PIN_INPUT1);
  //Control del PID1
  myPID1.Compute();
  analogWrite(PIN_OUTPUT1, Motor1);
  
 //Control motor 1
  Pot2 = analogRead(PIN_INPUT2);
  //Control del PID1
  myPID2.Compute();
  analogWrite(PIN_OUTPUT2, Motor2);

  //Cambio de sentido de los motores en funcion de su posicion con respecto a la cinematica inversa
  //Motor 1
  digitalWrite(PIN_CAMBIO11, Pot1<Setpoint1); 
  digitalWrite(PIN_CAMBIO12, Pot1>=Setpoint1);
  //Motor 2
  digitalWrite(PIN_CAMBIO21, Pot2<Setpoint2);
  digitalWrite(PIN_CAMBIO22, Pot2>=Setpoint2);

  //Detener los motores una vez pasado el tiempo de regulacion
       if(aux_v==1 and tiempo1>4000)
       {    
          digitalWrite(PIN_CAMBIO11, 0);
          digitalWrite(PIN_CAMBIO12, 0);
          digitalWrite(PIN_CAMBIO21, 0);
          digitalWrite(PIN_CAMBIO22, 0);
          
          Estados=2; //Cambiar de estado
       }
       if(aux_v==2 and tiempo2>5000)
       {
          digitalWrite(PIN_CAMBIO11, 0);
          digitalWrite(PIN_CAMBIO12, 0);
          digitalWrite(PIN_CAMBIO21, 0);
          digitalWrite(PIN_CAMBIO22, 0);
          
          Estados=2; //Cambiar de estado
       }
       if(aux_v==3 and tiempo3>6000)
       {  
          digitalWrite(PIN_CAMBIO11, 0);
          digitalWrite(PIN_CAMBIO12, 0);
          digitalWrite(PIN_CAMBIO21, 0);
          digitalWrite(PIN_CAMBIO22, 0);
          
          Estados=2; //Cambiar de estado
       }
}

//Activacion de la bomba de agua
void regar()
{
  //Activar el pin y activar temporizador
  digitalWrite(BOMBA, 1); 
  tiempobomba+=instF-instI;
 
    //Pasado el tiempo de temporizacion desactivar la bomba.
    //Si los ciclos han acabado, resetear todas las variables y pedir coordenadas por pantalla
    if(aux_v==vasos and tiempobomba>3000)
    {
         digitalWrite(BOMBA, 0);
      aux_c=0;
      aux_v=1;
      tiempobomba=0;
      tiempo1=0;
      tiempo2=0;
      tiempo3=0;
      pantalla=0;
      vasos=0;
      clearCoord();
      columna=15;
      linea=1;
      
      Estados=0; //Ir al estado inicial
    }

    //Si quedan ciclos por realizar aumentar las auxiliares y desactivar la bomba pasado 1seg
    if(aux_v<vasos and tiempobomba>3000)
    {  
      digitalWrite(BOMBA, 0);  
      aux_c+=4;  
      aux_v++;
      tiempobomba=0;
      delay(1000);
      
      Estados=1; //Volver al estado de regulacion de PID
      }
}
