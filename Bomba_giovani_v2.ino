#include <Wire.h>
#include <LiquidCrystal_I2C.h>

float Volt_panel_solar=0.0;
float Corriente_panel_solar=0.0;
float Volt_baterias=0.0;
float potencia_solar=0.0;

float max_volt_baterias=48;
float max_potencia=95*17;

bool E_main=false;
bool Baterias_ok=false;
bool panel_ok=false;
bool Off_bomba=true;
bool Apagar_bomba=false;

int Rele_Bomba=12;
int Rele_bateria=13;
int Rele_Volt_Panel=14;

int Led_indicador_uno=11;
int led_indicador_dos= 7;
int led_indicador_tres= 6;
int led_indicador_cuatro=4;

int lect_Volt_panel_solar=10;
int lect_Corriente_panel_solar=9;
int lect_Volt_baterias=8;

const int sensor_Corriente=A0;
const int sensor_Voltaje_panel=A1;
const int sensor_voltaje_bateria=A2;
const int interruptor_auxiliar_panel=5;
const int interruptor_auxiliar_bateria=3;

float constante = 0.0048889;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void DisplayLCD(char up[], char down[], int time, bool clear, bool iteration)
{
    if (clear)
    {
        lcd.clear();
        lcd.begin(16, 2);
     //   lcd.home();
        lcd.print(up);
    }
    if (iteration)
    {
        for (int i = 0; i <= 16; i++)
        {
            lcd.setCursor(i, 1);
            lcd.print('0');
            delay(30000);
            //Deberiamos poner un metodo de time.h
        }
    }
    else
    {
        lcd.setCursor(0, 1);
        lcd.print(down);
        delay(time);
    }
}
void setup(){
    DisplayLCD("INICIO PROGRAMA", "CARGANDO........", 1, true,false);
    pinMode(Rele_Bomba, OUTPUT);
    pinMode(Rele_bateria, OUTPUT);
    pinMode(Rele_Volt_Panel, OUTPUT);
    pinMode(Led_indicador_uno, OUTPUT);
    pinMode(led_indicador_dos,OUTPUT);
    pinMode(led_indicador_tres,OUTPUT);
    digitalWrite(Rele_Bomba, HIGH); 
    digitalWrite(Rele_bateria, HIGH);
    digitalWrite(Rele_Volt_Panel, HIGH);
    Serial.begin(9600);
    delay(5000);
    DisplayLCD("INICIO PROGRAMA", "CARGANDO......OK", 2000, true,false);

}
void bomba(int Rele, bool j){
    if (j){
        digitalWrite(Rele, LOW);
    }
    else{
        digitalWrite(Rele, HIGH);
    }
    
}
float Lectura_Corriente(String Name, int analogo, int frecuencia, int iteraciones, float Sensibilidad){
    Serial.print("Medimos la corriente de :: ");
    Serial.println(Name);
    float aux=0.0;
    float aux2=0.0;
    for (int i =0; i<iteraciones; i++){
        aux2=analogRead(analogo)*5.0/1023.0;
        Serial.println(aux2);
        aux+=(aux2-2.5)/Sensibilidad;
        delay(frecuencia);
    }
    aux2=aux/iteraciones;
    return aux2;
}
float Lecturas(String Name, int rele, int analogo, int frecuencia, int iteraciones){
    Serial.print("Medimos el voltaje de :: ");
    Serial.println(Name);
    float aux=0.0;
    float aux2=0.0;
    digitalWrite(rele, LOW);
    for (int i =0;i<iteraciones; i++){
        aux2=analogRead(analogo);
        Serial.println(aux2);
        aux=aux + ((aux2)*constante < 2.0) ? 0.0 : aux2*constante;
        delay(frecuencia);
    }
    aux2=aux/100;
    digitalWrite(rele, HIGH);
    return aux2;
}
void loop(){
    Serial.println("MEDIMOS---");

    Volt_baterias=Lecturas("VOLT BATERIA", Rele_bateria, sensor_voltaje_bateria, 100, 10);
    Serial.println(Volt_baterias);
    Baterias_ok=(Volt_baterias>(0.7*max_volt_baterias))? true: false;
    Serial.print('RESULTADO MEDICION VOLTA BATERIAS: ');
    Serial.println(Baterias_ok);
    
    if (Baterias_ok){

        digitalWrite(led_indicador_dos, LOW);
        Serial.println('BATERIAS EN ESTADO OPTIMO SE PROCEDE A SEGUIR CON LAS MEDICIONES..');
        int contador_aux=0;
        while (!panel_ok){
            Volt_panel_solar=Lecturas("VOLT PANEL SOLAR", Rele_Volt_Panel, sensor_Voltaje_panel, 100,10);
            Serial.println(Volt_panel_solar);
            Corriente_panel_solar=Lectura_Corriente("AP PANEL SOLAR", sensor_Corriente, 100, 10, 0.1);
            Serial.println(Corriente_panel_solar);
            potencia_solar=Corriente_panel_solar*Volt_panel_solar;
            panel_ok=(potencia_solar>(0.7*max_potencia))? true : false;
            Serial.print('RESULTADO MEDICION POTENCIA SOLAR: ');
            Serial.println(panel_ok);
            contador_aux+=1;
            if (interruptor_auxiliar_panel>4){
                Serial.println('SE HA ACCIONADO EL INTERRUPTOR AUXILIAR');
                panel_ok=true;
                digitalWrite(led_indicador_cuatro, HIGH);
            }else{
                digitalWrite(led_indicador_cuatro,LOW);
            }
            if (!panel_ok){
                Serial.println('RESULTADO BAJO SOLAR AVISO..!');
                digitalWrite(led_indicador_tres, HIGH);
                if (!Off_bomba){
                    Apagar_bomba=true;
                    break;
                }
            }
            if (contador_aux > 5){
                break;
            }}
            
    }else{
        Serial.println('RESULTADO BAJO BATERIAS AVISO..!');
        digitalWrite(led_indicador_dos, HIGH);
        if (!Off_bomba){
            //si la bomba esta encendida debemos terminar el ciclo e indicar que se debe apagar la bomba, 
            Apagar_bomba=true;
            }else{
                delay(360000);//5 minutos de carga a la bateria y procedemos
            }
    }
    if (panel_ok){
        digitalWrite(led_indicador_tres, LOW);
        Serial.println('PANELES EN ESTADO OPTIMO SE PROCEDE A SEGUIR');
    }
    if (Baterias_ok & panel_ok){
        Serial.println('CONDICIONES FAVORABLES, SE PROCEDE A ENCENDER LA BOMBA');
        Off_bomba=false;
        bomba(Rele_Bomba, true);
        digitalWrite(Led_indicador_uno,HIGH);
        delay(360000);
    }else{
        if (Apagar_bomba){
            Serial.println('SE DEBE APAGAR LA BOMBA LAS CONDICIONES NO SON LAS OPTIMAS');
            bomba(Rele_Bomba, false);
            digitalWrite(Led_indicador_uno,LOW);
            Apagar_bomba=false;
            Off_bomba=true;
            delay(60000);
        }
    }


    String display;
    display.concat(String(Volt_panel_solar));
    display.concat("V");
    display.concat(" ");
    display.concat(String(Corriente_panel_solar));
    display.concat("A");
    display.concat(" ");
    display.concat(String(Volt_baterias));
    display.concat("V");
    int str_len=display.length()+1;
    char char_array[str_len];
    display.toCharArray(char_array, str_len);
    Serial.println(display);
    DisplayLCD("Lectura Inicial", char_array, 1, true, false);


}



