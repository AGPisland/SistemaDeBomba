//VERSION 3 DEL PROGRAMA, ESTE DEBE SER MAS CONSISTENTE EN LA ESTRUCTURA DE CONTROL
//PRIMERO: LECTURA DEL VOLTAJE DE LA BATERIA, UNA BATERIA DESCARGADA ES CUANDO LA 
//DIFERENCIA DE POTENCIAL NO ES LA MAXIMA. SI LA COMPROBACION ES DE DESCARGA SE DEBE
//ENCENDER EL LED DE COLOR ROJO Y APAGAR LA BOMBA DE FORMA BRUTA, APAGANDO CON ESTA
//EL LED AZUL QUE REPRESENTA EL ESTADO DE LA BOMBA, SI LA CARGA ES COMPLETA SE APAGA EL LED ROJO.
//SEGUNDO: LECTURA DEL VOLTAJE SOLAR, CUANDO HAY SOL MAXIMO LA DIFERENCIA POTENCIAL DEL PANEL
//ES LA MAS ALTA, SI EL CLIMA SE ENCUENTRA NUBLADO LA DIFERENCIA DE POTENCIAL VA A SER LA MINIMA
//EN EL CASO DE QUE LA DIFF. POTENCIAL ES MAX SE SIGUE CON EL PROGRAMA APAGANDO LA LED VERDE SI ES QUE
//ESTA ENCENDIDA. SI LA DIFF. PÓTENCIAL ES MIN SE ENCIENDE EL LED VERDE.
//TERCERO: LECTURA DEL SENSOR DE CORRIENTE CONECTADO EN EL CABLE DEL PANEL SOLAR HACIA EL CARGADOR DE BATERIAS
//SI LA CORRIENTE QUE LEE EL SENSOR ES LA MAX REPRESENTA QUE EL CARGADOR ESTA CARGANDO LAS BATERIAS. EN ESTE
//CASO SE DEBE ENCENDER EL LED DE COLOR AMARILLO.
//SI LA CORRIENTE QUE LEE EL SENSOR ES LA MIN REPRESENTA QUE NO ES NECESARIO CARGAR LAS BATERIAS Y ESTO PUEDE SIGNIFICAR
//DOS COSAS QUE LAS BATERIAS SI ESTAN CARGADAS O QUE NO HAY SUFICIENTE POTENCIA SOLAR PARA CARGAR LAS BATERIAS.
//CUARTO: TENIENDO LAS 3 MEDIDAS DEL PROGRAMA, SE PROCEDE A COMPARAR LOS LED;
// ESTADO (1) BOMBA ENCENDIDA: SI Y SOLO SI LOS 3 LED ESTAN APAGADOS SE ENCIENDE LA BOMBA Y SU LED AZUL.
// ESTADO (2) BOMBA APAGADA: SI LA CONDICION ANTERIOR NO ES VERDADERA, Y EL LED AZUL ESTA ENCENDIDO SE DEBE APAGAR LA BOMBA Y SU LED AZUL.
// ESTADO (3) BATERIA DESCARGADA: INDEPENDIENTE DEL ESTADO (2), SI LA BATERIA ESTA DESCARGADA DEBEMOS ESPERAR A QUE SE CARGE.
// ESTADO (4) PANEL CARGANDO BATERIA: SI EL ESTADO (3) ES VERDAD, DEBEMOS CALCULAR EL TIEMPO DE CARGA, PARA ESTO HAY 4 COMBINACIONES DE LED:
//            LED VERDE ON Y LED AMARILLO ON: (POSIBLEMENTE NUBLADO Y PERO CARGANDO AL MAX LA BATERIA): 5 MIN SIN PARPADEO
//            LED VERDE ON Y LED AMARILLO OFF: (POSIBLEMENTE NUBLADO Y PERO CARGANDO AL MIN LA BATERIA): 10 MIN PARPADEO ALTERNADAMENTE DE LOS DOS LED.
//                                                                                                       SIGNIFICA QUE EL SISTEMA NO TIENE SUFICIENTE POTENCIA PARA 
//                                                                                                       ENCENDER LA BOMBA Y CARGAR LA BATERIA.
//            LED VERDE OFF Y LED AMARILLO ON: (POSIBLEMENTE SOLEADO Y PERO CARGANDO AL MAX LA BATERIA): 15 MIN PARPADEO JUNTOS DE LOS DOS LED.
//                                                                                                       SIGINIFICA QUE EL SISTEMA SI PUEDE CARGAR
//                                                                                                       LAS BATERIAS Y SE LE DA 15 MIN DE TIEMPO.
//            LED VERDE OFF Y LED AMARILLO OFF: (POSIBLEMENTE SOLEADO Y PERO CARGANDO AL MIN LA BATERIA): 5 MIN SIN PARPADEO
// ESTADO (5) BATERIA CARGADA: SI EL ESTADO (4) NO ES VERDADERO LA BATERIA SI ESTA CARGADA PERO ESTA NUBLADO TENEMOS 2 CASOS:
//            LED VERDE OFF Y LED AMARILLO ON: (BATERIA CARGADA, CLIMA SOLEADO, PERO CORRIENTE ALTA OSEA CARGANDO)- POSIBLE SALIDA DE ESTADO DESCARGADO.
//                                                                                                                  LA LUZ AMARILLA PARPADEA DURANTE 5 MIN.
//            LED VERDE ON Y LED AMARILLO X: (BATERIA CARGADA, CLIMA NUBLADO): LUZ VERDE PARPADEA HASTA QUE EL BOTON SEA PRESIONADO DURANTE 10 MIN SI SE PRESIONA SE SALE DEL CICLO.
// 
float V_Baterias=0.0;
float V_solar=0.0;
float C_solar=0.0;
float max_volt_baterias=48;
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
char Estado='INICIAL';

void setup(){
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
    Serial.println('------------------------------------');
    Serial.print('MODO LOOP, CONTADOR ACTIVADO: ');
    Serial.println(contador);
    Serial.print('ESTADO DEL PROGRAMA ACTUAL: ');
    Serial.println(Estado);
    Serial.println('------------------------------------');

    Serial.println('MEDICION DE VOLTAJE DE BATERIAS');
    V_Baterias=Read_V(' BATERIAS ', RL_VB, DV_VB, 800, 20);
    Serial.print('RESULTADO DE LECTURA: ');
    Serial.print(V_Baterias);
    Serial.print(' V');
    if (V_Baterias>0.7*max_volt_baterias){
        Led_VB=false;
        digitalWrite(LD_VB, LOW);
        //LA CARGA DE LA BATERIA NO ESTÁ BAJA
    }else{
        Led_VB=true;
        digitalWrite(LD_VB, HIGH);
        Led_BB=false;
        digitalWrite(RL_Bomba, HIGH);
        digitalWrite(LD_BB, LOW);
    }
    Serial.print('RESULTADO LOGICO: ');
    Serial.println(Led_VB);
    Serial.println('------------------------------------');


    Serial.println('------------------------------------');
    Serial.println('MEDICION DE VOLTAJE DEL PANEL');
    V_solar=Read_V(' PANEL SOLAR ', RL_VS, DV_VS, 800, 20);
    Serial.print('RESULTADO DE LECTURA: ');
    Serial.print(V_solar);
    Serial.print(' V');
    if (V_solar>0.7*max_volt_solar){
        Led_VS=false;
        digitalWrite(LD_VS, LOW);
    }else{
        // si el panel solar entrega un voltaje bajo el nivel
        Led_VS=true;
        digitalWrite(LD_VS, HIGH);
    }
    if (k){
        Led_VS=false;
        digitalWrite(LD_VS, LOW);
    }
    Serial.print('RESULTADO LOGICO: ');
    Serial.println(Led_VS);
    Serial.println('------------------------------------');


    Serial.println('------------------------------------');
    Serial.println('MEDICION DE LA CORRIENTE DE CARGA PANEL');
    C_solar=Read_C(Sensor_C, 800, 20);
    Serial.print('RESULTADO DE LECTURA: ');
    Serial.print(C_solar);
    Serial.print(' Amp');
    contador+=1;
    if (C_solar>2.0 & C_solar<17.0){
        // EL PANEL ESTA EN MODO CARGA
        Led_CS=true;
        digitalWrite(LD_CS, HIGH);
    }else{
        Led_CS=false;
        digitalWrite(LD_CS, LOW);
    }
    Serial.print('RESULTADO LOGICO: ');
    Serial.println(Led_CS);
    Serial.println('------------------------------------');


    Serial.println('LECTURA DE SENSORES FINALIZADA');
    Serial.println('RESULTADO DE LA LECTURA: ');
    if (!Led_VB & !Led_VS & !Led_CS){
        Estado='(1) BOMBA ENCENDIDA';
        //todas las condiciones se cumplen
        Led_BB=true;
        digitalWrite(LD_BB, HIGH);
        digitalWrite(RL_Bomba, LOW);
        Serial.print('SE CUMPLEN LAS CONDICIONES, LA BOMBA SE ENCIENDE, NOMBRE DEL ESTADO: '); Serial.print(Estado); Serial.print(' REGISTRO NUM: '); Serial.print(contador);

        delay(1*60000);
        
    }else{
        if (Led_BB){
            Estado='(2) BOMBA APAGADA';
            Serial.println('NO SE DAN LAS CONDICIONES MANTENER LA BOMBA ENCENDIDA LA BOMBA SE APAGA. NOMBRE DEL ESTADO: '); Serial.print(Estado); Serial.print(' REGISTRO NUM: '); Serial.print(contador);
            Led_BB=false;
            digitalWrite(LD_BB, LOW);
            digitalWrite(RL_Bomba, HIGH);
        }
        if (Led_VB){
            Estado='(3) BATERIA DESCARGADA |1 Y 2|';
            Serial.println('LAS BATERIAS ESTAN DESCARGADAS NOMBRE DEL ESTADO: '); Serial.print(Estado); Serial.print(' REGISTRO NUM: '); Serial.print(contador);
            Estado='(4) PANEL CARGANDO BATERIA |SI 3|';
            Serial.println('COMPROBACION DE CARGA 4 COMBINACIONES 2 ENTRADAS NOMBRE DEL ESTADO: '); Serial.print(Estado); Serial.print(' REGISTRO NUM: '); Serial.print(contador);
            Serial.print('RESULTADO: ');
            if (Led_VS & Led_CS){
                Estado='trabajo del cargador de baterias, C.C: 11';
                Serial.println('Potencia solar regular. Estado solar malo. Nivel de cargando alto. C.C: ');Serial.print(Estado);
                delay(60000);
            }
            if (!Led_VS & Led_CS){
                Estado='las baterias se carga bien, un poco de tiempo, C.C: 01';
                Serial.println('Potencia solar muy buena. Estado solar bueno. Nivel de cargando alto. C.C: ');Serial.print(Estado);
                int j=0;
                digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, LOW);
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, HIGH);
                    delay(750);
                    if (j == 200){digitalWrite(LD_CS, LOW);  break;}
                    digitalWrite(LD_VS, HIGH); digitalWrite(LD_CS, LOW);
                    delay(750);
                
                }
            
            }
            if (!Led_VS & !Led_CS){
                Estado='final recuperado de descarga, C.C: 00';
                Serial.println('Potencia solar media. Estado solar bueno. Nivel de cargando bajo. C.C: ');Serial.print(Estado);
                delay(60000);
            }
            if (Led_VS & !Led_CS){
                Estado='ayuda, las baterias se siguen descargando, C.C: 10';
                Serial.println('Potencia solar muy baja. Estado solar malo. Nivel de cargando bajo. C.C: ');Serial.print(Estado);
                int j=0;
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW); digitalWrite(LD_CS, LOW);
                    delay(750);
                    if (j == 200){ break;}
                    digitalWrite(LD_VS, HIGH); digitalWrite(LD_CS, HIGH);
                    delay(750);
                
                }
            
            }               
            
            
        }else{
            Estado='(5) BATERIA CARGADA |1 Y 2|';
            Serial.println('BATERIA CARGADA PERO CUIDAMOS EL MAX DE LA FUNCION OBJETIVO: '); Serial.print(Estado); Serial.print(' REGISTRO NUM: '); Serial.print(contador);
            if (!Led_VS & Led_CS){
                Estado='estado poco posible, C.C: 01';
                Serial.println('Potencia solar muy buena. Estado solar bueno. Nivel de cargando alto. C.C: ');Serial.print(Estado);
            }
            if (Led_VS || Led_CS){
                Estado='posible carga de generador de combustible a la bateria, C.C: 1X';
                Serial.println('Potencia solar regular. Estado solar bajo. no se evalua nivel de cargando. C.C: ');Serial.print(Estado);
                int j=0;
                while (true){
                    //5 min
                    j+=1;
                    digitalWrite(LD_VS, LOW);
                    delay(750);
                    if (j == 200 || analogRead(INT)>700){ k=true; break;}else{k=false;}
                    digitalWrite(LD_VS, HIGH);
                    delay(750);
                
                }
            }

        }
    }
    Serial.println('------------------------------------');
    Serial.println('-                                  -');
    Serial.println('-                                  -');
    Serial.println('-                                  -');
    Serial.println('-                                  -');
    Serial.println('------------------------------------');
    Led_VB=false; Led_VS=false; Led_CS=false;
    digitalWrite(RL_VB, HIGH); digitalWrite(RL_VS, HIGH);
    Serial.println('FIN----FIN----FIN----FIN----FIN----FIN----FIN----FIN----FIN');
    contador+=1;
    
}



float Read_V(int N, int RL, int AL, int F, int i){
    Serial.print('LEYENDO SENSOR DE VOLTAJE DE ');
    int cont=0;
    if (N==1){
        Serial.println(' BATERIA.');
        cont=24;
    }else{
        Serial.println(' PANEL SOLAR.');
        cont=85;
    }
    
    float aux=0.0;
    float sum=0.0;
    digitalWrite(RL, LOW);
    for (int j =0; j<i; j++){
        aux=(cont*analogRead(AL))/512;
        aux= (aux <1.0)? 0.0 : aux;
        Serial.print('VOLTAJE: ');
        Serial.print(aux);
        Serial.println(' V');
        sum=sum+aux;
        aux=0.0;
        delay(F);
    }
    aux=sum/i;
    digitalWrite(RL, HIGH);
    return aux;

}

float Read_C(int AL, int F, int i){
    Serial.println('LEYENDO SENSOR DE CORRIENTE');
    float aux=0.0;
    float sum=0.0;
    for (int j=0; j<i;j++){
        aux=((2.5*analogRead(AL))/512)*12-30;
        if (aux < 0.9){
            aux=0.0;
        }
        Serial.print('CORRIENTE: ');
        Serial.print(aux);
        Serial.println( 'Amp');

        sum=sum+aux;
        aux=0;
        delay(F);
    }
    aux=sum/i;
    return aux;
}
