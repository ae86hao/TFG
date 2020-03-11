                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           /* 
  Ana Isabel Sotomayor Romillo
  TFM: Diseño de interfaces cerebro máquina con señalización mixta
  
  Código Arduino V3: Código para encender 4 LEDs a la vez que oscilen a distinta frecuencia
  Las frecuencias se definen a traves del puerto serie de la siguiente manera:
  
        6 12 15 20
*/

/////////////////////////////////////////////
// Inicializacion de constantes.
/////////////////////////////////////////////

// Numero de puertos a los que se conectan los LEDs. Se seleccionan los pines PWM
const int led1Pin =  3;   
const int led2Pin =  5;   
  
const int led4Pin =  9; 
const int led5Pin =  10; 


// Estado inicial de los LEDs a LOW
int led1State = LOW;            
int led2State = LOW;            

int led4State = LOW;       
int led5State = LOW; 


//Almacena el tiempo desde la ultima vez que se actualiza el LED, en microsegundos
//Constante de tipo long porque tipo int se llenaria muy deprisa
//Microsegundos porque las funciones millis() y micros() devuelven enteros, y micros sera mas preciso 
unsigned long previousMicros1 = 0;
unsigned long previousMicros2 = 0;        
        
unsigned long previousMicros4 = 0;        
unsigned long previousMicros5 = 0;  


// Inicializacion de las variables auxiliares
double auxi1;
double auxi2;

double auxi4;
double auxi5;


// cast para pasar double a long
unsigned long interval1;     
unsigned long interval2;          

unsigned long interval4; 
unsigned long interval5; 

  

//frecuencias
int trama_config = 0;
float led1f;     
float led2f;    

float led4f;
float led5f;   


unsigned long currentMicros1 = 0;
unsigned long currentMicros2 = 0;

unsigned long currentMicros4 = 0;
unsigned long currentMicros5 = 0;


//contador de 30 segundos

long contador;
long milis_inicio;
long milis_fin;

/////////////////////////////////////////////
// Setup del arduino (Declaracion de pines)
/////////////////////////////////////////////

void setup() {
  // Se declaran los pines como salidas
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
  
  Serial.begin(9600);

  while (1){
        if (Serial.available() > 0) {
                      while (trama_config !=99){
                        trama_config=Serial.parseInt();
                      }
                      trama_config=0;
                      led1f=Serial.parseFloat();
                      led2f=Serial.parseFloat();
                      
                      led4f=Serial.parseFloat();
                      led5f=Serial.parseFloat();

                      // Paso de las frecuencias a periodo en microsegundos
					  // Las frecuencias con valor 0 indican luces fijas
            // Las frecuencias con valor -1 indican luces apagadas(auxi = 1)
                      if (led1f == 0) auxi1 = 0;
                      else if (led1f == -1) auxi1 = 1;
                      else auxi1= (1000000/(2*led1f));
                      
                      if (led2f == 0) auxi2 = 0;
                      else if (led2f == -1) auxi2 = 1;
                      else auxi2= (1000000/(2*led2f));
                      
                      if (led4f == 0) auxi4 = 0;
                      else if (led4f == -1) auxi4 = 1;
                      else auxi4= (1000000/(2*led4f));
                      
                      if (led5f == 0) auxi5 = 0;
                      else if (led5f == -1) auxi5 = 1;
                      else auxi5= (1000000/(2*led5f));
                      
                          
                      // cast para pasar double a long
                      interval1 = (long)auxi1;     
                      interval2 = (long)auxi2;          
                      
                      interval4 = (long)auxi4; 
                      interval5 = (long)auxi5; 
                      

                      //Repetir el programa durante 10 segundos

                      while(1) {
                         EncenderLeds(interval1, interval2, interval4, interval5);
                         if (Serial.available() > 0) {
                            trama_config=Serial.parseInt();
                            if (trama_config == 66){
                              EncenderLeds(1, 1, 1, 1);
                              break;
                            }
                         }
                      }
        }
  }
}


/////////////////////////////////////////////
// Codigo de programa
/////////////////////////////////////////////
void loop(){}


void EncenderLeds(unsigned long interval1, unsigned long interval2, unsigned long interval4, unsigned long interval5)
{
  //Obtenemos para cada pasada de bucle el tiempo actual en microsegundos
  currentMicros1 = micros();
  currentMicros2 = micros();
  
  currentMicros4 = micros();
  currentMicros5 = micros();
  
  
// Codigo LED1

   // Si la diferencia entre el tiempo actual (currentMicros) y la ultima vez que 
  //el led parpadeo (previousMicros) es mayor que el periodo de oscilacion, el LED
  //cambia de estado (Si esta a HIGH pasa a LOW y viceversa).
  //Este procedimiento se repite para los 6 LEDS
  
     if (interval1==0)
          analogWrite(led1Pin, 255);
     else if (interval1==1)
          digitalWrite(led1Pin, LOW);
      
     else if(currentMicros1 - previousMicros1 >= interval1) 
     {
          //Guardo el microsegundo en el que cambio el estado 
          previousMicros1 = currentMicros1;   
      
          if (led1State == LOW)
              led1State = 255;
          else
              led1State = LOW;
      
          // Paso al pin el valor del estado 
          analogWrite(led1Pin, led1State);
      
      }
    
    
// Codigo LED2 
 
     if (interval2==0)
          analogWrite(led2Pin, 255);
     else if (interval2==1)
          digitalWrite(led2Pin, LOW);
        
     else if(currentMicros2 - previousMicros2 >= interval2) 
     {
          previousMicros2 = currentMicros2;   
      
      
          if (led2State == LOW)
              led2State = 255;
          else
              led2State = LOW;
      
          analogWrite(led2Pin, led2State);
      }
  

// Codigo LED4
 
    
      if (interval4==0)
          analogWrite(led4Pin, 255);
      else if (interval4==1)
          digitalWrite(led4Pin, LOW);
        
      else if(currentMicros4 - previousMicros4 >= interval4) 
      {
          previousMicros4 = currentMicros4;   
      
          if (led4State == LOW)
              led4State = 255;
          else
              led4State = LOW;
      
          analogWrite(led4Pin, led4State);
      }

// Codigo LED5 
 
    
      if (interval5==0)
          analogWrite(led5Pin, 255);
      else if (interval5==1)
          digitalWrite(led5Pin, LOW);
        
      else if(currentMicros5 - previousMicros5 >= interval5) 
      {
          previousMicros5 = currentMicros5;   
      
          if (led5State == LOW)
              led5State = 255;
          else
              led5State = LOW;
      
          analogWrite(led5Pin, led5State);
      }
  
  
}


