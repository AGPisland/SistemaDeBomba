//PARA MAYOR INFORMACION LEA
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

float V_Baterias=0.0;
float V_solar=0.0;
float C_solar=0.0;
//float max_volt_baterias=48;
float max_volt_solar=95;

bool Led_VB=false;
bool Led_VS=false;
bool Led_CS=false;
bool Led_BB=false;
bool k=false;

int RL_Bomba=10;
int RL_VS=11;
int RL_VB=12;
int LD_VB=9;
int LD_VS=8;
int LD_CS=7;
int LD_BB=6;

const int Sensor_C=A0;
const int DV_VS=A1;
const int DV_VB=A2;
const int INT=A3;
int contador=0;

void DisplayLCD(char up[], char down[], int J)
{
    lcd.clear();
    if (J>0 & 11>J){
        lcd.begin(16, 2);
        lcd.print(up);
        lcd.setCursor(0, 1);
        String o=Estados_Registro(J);
        int str_len=o.length()+1;
        char char_array[str_len];
        o.toCharArray(char_array, str_len);
        lcd.print(o);
        return;   
    }
    if (J==-1){
        lcd.begin(16, 2);
        lcd.print(up);
        return;
    }
    if (J==-2){
        lcd.setCursor(0, 1);
        lcd.print(down);
        return;
    }
    lcd.begin(16, 2);
    lcd.print(up);
    lcd.setCursor(0, 1);
    lcd.print(down);   
}
void setup(){
    DisplayLCD('INICIANDO','CARGANDO...', 12);
    pinMode(Sensor_C, INPUT);
    pinMode(DV_VS, INPUT);
    pinMode(DV_VB,INPUT);
    pinMode(INT,INPUT);
    pinMode(RL_Bomba, OUTPUT);
    pinMode(RL_VB, OUTPUT);
    pinMode(RL_VS, OUTPUT);
    pinMode(LD_BB, OUTPUT);
    pinMode(LD_CS, OUTPUT);
    pinMode(LD_VB, OUTPUT);
    pinMode(LD_VS, OUTPUT);
    digitalWrite(RL_Bomba, HIGH);
    digitalWrite(RL_VB, HIGH);
    digitalWrite(RL_VS, HIGH);
    digitalWrite(LD_BB, HIGH);
    digitalWrite(LD_CS, HIGH);
    digitalWrite(LD_VB, HIGH);
    digitalWrite(LD_VS, HIGH);
    Serial.begin(9600);
    delay(2000);
    digitalWrite(LD_BB, LOW);
    digitalWrite(LD_CS, LOW);
    digitalWrite(LD_VB, LOW);
    digitalWrite(LD_VS, LOW);
}
void loop(){
    String con;    
    con.concat("CONTADOR: ");
    con.concat(String(contador));
    int str_con=con.length()+1;
    char char_con[str_con];
    con.toCharArray(char_con, str_con);
    DisplayLCD("MODO LECTURA", char_con, 12);
    Serial.println("-------------------------");
    Serial.print(  "-MODO LOOP, CONTADOR:");
    Serial.println(contador);
    Serial.println("-------------------------");
    Serial.print(  "-VOLTAJE BATERIAS:");
    V_Baterias=Read_V(1, RL_VB, DV_VB, 50, 100);
    Serial.print(V_Baterias);
    Serial.println(" V");
    if (V_Baterias>0.5*48){
        Led_VB=false;
        digitalWrite(LD_VB, LOW);
    }else{
        Led_VB=true;
        digitalWrite(LD_VB, HIGH);
        Led_BB=false;
        digitalWrite(RL_Bomba, HIGH);
        digitalWrite(LD_BB, LOW);
    }
    Serial.println("-------------------------");
    Serial.print(  "-VOLTAJE DE PANELES: ");
    V_solar=Read_V(2, RL_VS, DV_VB, 50, 100);
    Serial.print(V_solar);
    Serial.print(" V");
    if (V_solar>0.5*max_volt_solar){
        Led_VS=false;
        digitalWrite(LD_VS, LOW);
    }else{
        Led_VS=true;
        digitalWrite(LD_VS, HIGH);
    }
    if (k){
        Led_VS=false;
        digitalWrite(LD_VS, LOW);
    }
    Serial.println("-------------------------");
    Serial.print("-CORRIENTE DE CARGA: ");
    C_solar=Read_C(Sensor_C, 100, 20);
    Serial.print(C_solar);
    Serial.println(" A");
    if (C_solar>5.0 & C_solar<17.0){
        Led_CS=true;
        digitalWrite(LD_CS, HIGH);
    }else{
        Led_CS=false;
        digitalWrite(LD_CS, LOW);
    }
    
    String display;
    display.concat(String(V_Baterias));
    display.concat(" V ");
    display.concat(String(C_solar*V_solar));
    display.concat(" W");
    int str_len=display.length()+1;
    char char_array[str_len];
    display.toCharArray(char_array, str_len);
    DisplayLCD("MODO LECTURA", char_array, 12);
    delay(5000);
    Serial.println("-------------------------");
    Serial.println("---CONLUSIONES::---------");
    // led_vd: falso cuando las condiciones son buena bateria, true baterias descargadas
    // led_vs: falso cuando esta soleado, verdadero cuando esta nublado
    // led_cs: falso cuando la corriente es menor que 5 A, quiere decir que no se esta cargando la bateria, true si la corriente esta entre
    // 5 y 17 A ya que el cargador si esta cargando la bateria.-
    if (!Led_VB & !Led_VS & !Led_CS){
        Led_BB=true;
        digitalWrite(LD_BB, HIGH);
        digitalWrite(RL_Bomba, LOW);
        Serial.println("TODO EN BUENAS CONDICIONES SE PROCEDE A ENCENDER LA BOMBA");
        Serial.println(Estados_Registro(1));
        DisplayLCD("ESTADO: ","",1);
        delay(10000);
    }else{
        
        if (Led_BB){
            Serial.println("CAMBIARON LAS CONDICIONES POR TANTO SE PROCEDE A APAGAR LA BOMBA");            
            Serial.println(Estados_Registro(2));
            DisplayLCD("ESTADO: ","",2);
            delay(5000);
            Led_BB=false;
            digitalWrite(LD_BB, LOW);
            digitalWrite(RL_Bomba, HIGH);
        }
        if (Led_VB){
            //LA BATERIA SE ENCUENTRA DESCARGADA POR TANTO EL LED ROJO DEBE ESTAR ENCENDIDO.
            Serial.println("LAS BATERIAS ESTAN DESCARGADAS");
            Serial.println(Estados_Registro(3));
            DisplayLCD("ESTADO: ","",3);
            delay(5000);
            Serial.println(Estados_Registro(4));
            DisplayLCD("ESTADO: ","",4);
            delay(5000);
            Serial.print("RESULTADO: ");
            if (Led_VS & Led_CS){
                //LED VERDE ENCENDIDO, LED AMARILLO TAMBIEN, LED ROJO TAMBIEN , INDICA QUE EL PANEL SOLAR ESTA SIN SOL, EL CARGADOR NO PUEDE
                // CARGAR LAS BATERIAS, Y LAS BATERIAS ESTAN DESCARGADAS, TODOS LOS LEDS DEBEN ESTAR ENCENDIDOS PARA LLEGAR A ESTE NIVEL
                Serial.println("TODOS LOS NIVELES SON BAJOS, POR FAVOR CARGAR LAS BATERIAS");
                Serial.println(Estados_Registro(5));
                DisplayLCD("ESTADO: ","",5);
                delay(10000);
            }
            if (!Led_VS & Led_CS){
                // LED ROJO ENCENDIDO, LED VERDE APAGADO, LED AMARILLO ENCENDIDO
                //LED VERDE APAGADO, LED AMARILLO ENCENDIDO: LOS PANELES ENTREGAN BUANA POTENCIA SOLAR, Y EL CARGADOR SE ENCUENTRA CARGANDO
                //LAS BATERIAS ESTE ESTADO ES PARA ESPERAR.
                Serial.println("PANELES CARGANDO LA BATERIA, TOME UN TIEMPO.");
                Serial.println(Estados_Registro(6));
                DisplayLCD("ESTADO: ","",6);
                int j=0;
                digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, LOW);
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, HIGH);
                    delay(750);
                    if (j == 10){digitalWrite(LD_CS, LOW);  break;}
                    digitalWrite(LD_VS, HIGH); digitalWrite(LD_CS, LOW);
                    delay(750);
                }
            }
            if (!Led_VS & !Led_CS){
                //LED ROJO ENCENDIDO, LED VERDE APAGADO, LED AMARILLO APAGADO:
                //POSIBLE ESTADO DE TERMINO DE CARGA, DEBIESE SER RAPIDO SALIR DE ESTE ESTADO:
                // ESTO INDICA QUE LA BATERIA ESTA DESCARGADA PERO EL PANEL ESTA OPTIMO Y LA CORRIENTE DE CARGA TAMBIEN.
                Serial.println("DEBEMOS ESPERAR, A LA BATERIA LE FALTA CARGA AUN");
                Serial.println(Estados_Registro(7));
                DisplayLCD("ESTADO: ","",7);
                int j=0;
                digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, LOW);
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, HIGH);
                    delay(750);
                    if (j == 5){digitalWrite(LD_CS, LOW);  break;}
                    digitalWrite(LD_VS, HIGH); digitalWrite(LD_CS, LOW);
                    delay(750);
                }
            }
            if (Led_VS & !Led_CS){
                // LED ROJO ENCENDIDO, LED VERDE ENCENDIDO, LED, AMARILLO APAGADO:
                // LAS BATERIAS ESTAN DESCARGADAS, EL PANEL ESTA NUBLADO, PERO HAY CORRIENTE DE POR MEDIO QUE CARGA LAS BATERIAS
                Serial.println(Estados_Registro(8));
                DisplayLCD("ESTADO: ","",8);
                int j=0;
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, LOW);
                    delay(750);
                    if (j == 10){ break;}
                    digitalWrite(LD_VS, HIGH); digitalWrite(LD_CS, HIGH);
                    delay(750);
                }
            }
        }else{
            // LED ROJO APAGADO, LAS BATERIAS SE ENCUENTRAN CARGADAS.
            Serial.println("LAS BATERIAS SI ESTAN CARGADAS HAY OTRA SITUACION DE BLOQUEO");
            Serial.println(Estados_Registro(9));
            DisplayLCD("ESTADO: ","",9);
            delay(10000);
            if (!Led_VS & Led_CS){
                //LED ROJO APAGADO, LED VERDE APAGADO, LED AMARILLO ENCENDIDO
                Serial.println("BATERIAS CARGADAS, MUCHO SOL, PERO MUCHA CORRIENTE DE CARGA, ERROR DE LECTURA DE CORRIENTE??");
                Serial.println(Estados_Registro(10));
                DisplayLCD("ESTADO: ","",10);
                delay(10000);
            }
            if (Led_VS || Led_CS){
                //LED ROJO APAGADO, LED VERDE ENCENDIDO, LED AMARILLO NO IMPORTA,
                //LAS BATERIAS ESTAN CARGADAS, EL PANEL ESTA NUBLADO,
                // BOTON DE AYUDA
                Serial.println("SE NECESITA ASISTENCIA CON EL BOTON!");
                Serial.println(Estados_Registro(11));
                DisplayLCD("ESTADO: ","",11);
                int j=0;
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW);
                    delay(750);
                    if (j == 10 || analogRead(INT)>700){ k=true; break;}else{k=false;}
                    digitalWrite(LD_VS, HIGH);
                    delay(750);   
                }
            }
        }
    }
    Serial.println("-----------------------------------");
    Led_VB=false; Led_VS=false; Led_CS=false;
    digitalWrite(RL_VB, HIGH); digitalWrite(RL_VS, HIGH);
    contador+=1;   
}
String Estados_Registro(int j){
    switch (j){
        case 1:
         return "1 BOMBA PRENDIDA";
        case 2:
         return "2 BOMBA APAGADA ";
        case 3:
         return "3 BAT DESCARGADA";
        case 4:
         return "4 MODO DE CARGA ";                  
        case 5:
         return "5 TODO MUY BAJO ";
        case 6:
         return "6 CARGANDO FULL ";
        case 7:
         return "7 RECOVERY CHARG";
        case 8:
         return "8 ERROR DE CARGA";
        case 9:
         return "9 BATERIA OPTIMA";
        case 10:
         return "10 !NO POSIBLE! ";
        case 11:
         return "11 DIESEL CARGA ";
    }
    return "0 INICIAL";
}
float Read_V(int N, int RL, int AL, int F, int i){
    Serial.print("LECTUA VOLTAJE  ");
    int cont=0;
    if (N==1){
        Serial.print("BATERIA ");
        cont=24;
    }else{
        Serial.print(" PANEL SOLAR ");
        cont=85;
    }
    
    float aux=0.0;
    float sum=0.0;
    float aux2=0;
    digitalWrite(RL, LOW);
    for (int j =0; j<i; j++){
        aux2=0.0;
        //Serial.print(analogRead(AL)); Serial.print(aux2); Serial.println(cont);
        aux2=analogRead(AL);
        aux=cont*(aux2/512);
        delay(10);
        aux= (aux <1.0)? 0.0 : aux;
        Serial.print(aux);
        Serial.println(" V");
        sum=sum+aux;
        aux=0.0;
        delay(F);
    }
    aux=sum/i;
    digitalWrite(RL, HIGH);
    return aux;
}
float Read_C(int AL, int F, int i){
    Serial.println("LECTURA CORRIENTE");
    float aux=0.0;
    float aux3=0.0;
    float sum=0.0;
    float aux2=0.0;
    for (int j=0; j<i;j++){
        aux2=0.0;
        aux2=analogRead(AL);
        aux3=((2.5*aux2)/512);
        aux=(aux3*12)-30;
        if (aux < 0.9){
            aux=0.0;
        }
        if (aux >18.0){
            aux=17.0;
        }
        Serial.print(aux);
        Serial.println(" A");
        sum=sum+aux;
        aux=0;
        delay(F);
    }
    aux=sum/i;
    return aux;
}
