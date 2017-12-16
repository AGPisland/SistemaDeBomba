#include <Wire.h>
#include <LiquidCrystal_I2C.h>
float constante = 0.0048889;
int TimeOff = 0;
int TimeBomba = 300000; //5 minutos
int add = 1;
bool StatusBateria = false;
bool StatusPanel = false;
bool StatusCompleto = false;
int ReleLecturaBateria = 12;
int ReleLecturaPanelSolar = 11;
int ReleCargaBateria = 10;
int LedCharge = 13;
char Val1[8];
char Val2[8];
char LCDMSG[16];
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
void setup()
{
    DisplayLCD("INIT CHARGE BAT", "12,11,10,A1,A2", 1, true,false);
    pinMode(ReleLecturaBateria, OUTPUT);
    digitalWrite(ReleLecturaBateria, HIGH);
    pinMode(ReleLecturaPanelSolar, OUTPUT);
    digitalWrite(ReleLecturaPanelSolar, HIGH);
    pinMode(ReleCargaBateria, OUTPUT);
    digitalWrite(ReleCargaBateria, HIGH);
    pinMode(LedCharge, OUTPUT);
    digitalWrite(LedCharge, LOW);
    delay(2000);
    DisplayLCD("INIT CHARGE BAT", "STATUS SERIAL OK", 2000, true,false);
    Serial.begin(9600);
}
void loop()
{
    //ProgramUno();
    ProgramaDos();
}
bool LecturaVoltajeBaterias(int PIN, int POUT, int Time, int Muestra, float VolMax, char display1[], int select)
{
    digitalWrite(PIN, HIGH);
    DisplayLCD(display1, "0.0", 1, true,false);
    Time = 1000 * Time;
    float Sampl1 = 0.0;
    float Sampl2 = 0.0;
    int Time2 = 0;
    digitalWrite(PIN, LOW);
    while (Time2 != Time)
    {
        for (int i = 0; i < Muestra; i++)
        {
            Sampl2 = analogRead(POUT);
            Sampl2 = ((Sampl2)*constante < 1.8) ? 0.0 : Sampl2;
            Sampl1 = Sampl1 + Sampl2;
            Sampl2 = Sampl2 * constante;
            Serial.print("Volt Medicion: ");
            Serial.println(Sampl2);
            delay(100);
            Time2 = Time2 + 100;
            dtostrf(Sampl2, 4, 2, LCDMSG);
            DisplayLCD(display1, LCDMSG, 1, false,false);
        }
        Serial.print("Time: ");
        Serial.println(Time2);
    }
    Sampl1 = (Sampl1 / (10 * (Time / 1000))) * constante;
    Serial.println();
    Serial.print("Muestra Promedio de carga: ");
    Serial.println(Sampl1);
    digitalWrite(PIN, HIGH);
    if (select == 1)
    {
        dtostrf(Sampl1, 5, 2, Val1);
        DisplayLCD(display1, Val1, 5000, true,false);
    }
    else
    {
        dtostrf(Sampl1, 5, 2, Val2);
        DisplayLCD(display1, Val2, 5000, true,false);
    }
    return (Sampl1 > VolMax) ? true : false;
}
void ProgramaDos()
{
    DisplayLCD("STADOS DE LOOP", "----------------", 1, true,false);
    StatusPanel = LecturaVoltajeBaterias(ReleLecturaPanelSolar, A2, 20, 10, 3.8, "Read Panel Solar", 1);
    while (StatusPanel)
    {
        StatusBateria = !LecturaVoltajeBaterias(ReleLecturaBateria, A1, 20, 10, 3, "Read Baterias", 2);
        while (StatusBateria)
        {
            DisplayLCD("Inicio Carga", "--", 1, true,false);
            Serial.println("Valores");
            Serial.print(Val1);
            Serial.print("Volt  ");
            Serial.print(Val2);
            Serial.println("Volt  ");
            digitalWrite(LedCharge, HIGH);
            digitalWrite(ReleCargaBateria, LOW);
            DisplayLCD("Inicio Carga", "--", 1, false,true);
            DisplayLCD("Fin Carga", "--", 10000, true,false);
            digitalWrite(LedCharge, LOW);
            digitalWrite(ReleCargaBateria, HIGH);
            StatusBateria = !LecturaVoltajeBaterias(ReleLecturaBateria, A1, 20, 10, 3, "Read Baterias", 2);
            delay(60000);
        }
    }
}
void ProgramUno()
{
    //1- Flotador de estanque enciende el arduino.
    //2- Por medio de la carga de bateria el arduino debe funcionar.
    //3- Imprimir en display estado de bateria,
    //4-
    DisplayLCD("STADOS DE LOOP", "----------------", 1, true,false);
    while (!StatusCompleto)
    {
        Serial.println();
        Serial.println("Lectura Panel ");
        StatusPanel = LecturaVoltajeBaterias(ReleLecturaPanelSolar, A2, 20, 10, 3.8, "Read Panel Solar", 1);
        if (StatusPanel)
        {
            while (!StatusBateria)
            {
                Serial.println("Comenzamos comprobacion de carga de bateria");
                TimeOff = TimeOff * add;
                Serial.print("Tiempo Stand-by: ");
                Serial.println(TimeOff);
                delay(TimeOff);
                Serial.println();
                Serial.println("Lectura bateria: ");
                add = add * 2;
                TimeOff = 2000;
                StatusBateria = !LecturaVoltajeBaterias(ReleLecturaBateria, A1, 20, 10, 3, "Read Baterias", 2);
                //si deseamos cargar la bateria pq está con falta de carga el retorno de esta funcion debe
                // ser falso, por tanto saldra del ciclo while y comenzara la carga, Si la bateria esta bajo 3Volt
                // la funcion retorna Falso, la negacion hara StatusBateria= true, pero la condicion negativa del ciclo
                // while no seguira, por tanto se sale del cilo cuandola funcion retorna False que esta bajo de 3V
            }
            StatusCompleto = true;
        }
        if (StatusCompleto)
        {
            DisplayLCD("Carga baterias", Val2, 1, true,false);
            Serial.print("Estado en carga de baterias, time programado por ciclo: ");
            Serial.println(TimeBomba);
            digitalWrite(LedCharge, HIGH);
            digitalWrite(ReleCargaBateria, LOW);
            delay(TimeBomba);
            StatusCompleto = false;
            StatusBateria = false;
            StatusPanel = false;
        }
    }
}
void DisplayLCD(char up[], char down[], int time, bool clear, bool iteration)
{
    if (clear)
    {
        lcd.clear();
        lcd.begin(16, 2);
        lcd.home();
        lcd.print(up);
    }
    if (iteration)
    {
        for (int i = 0; i <= 16; i++)
        {
            lcd.setCursor(i, 1);
            lcd.print('0');
            delay(30000);
        }
    }
    else
    {
        lcd.setCursor(0, 1);
        lcd.print(down);
        delay(time);
    }
}

