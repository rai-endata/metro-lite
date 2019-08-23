
#ifndef _LED_
  #define _LED_
  
  #include "Definicion de tipos.h"

#define frecuencia1   1
#define frecuencia2   2
#define frecuencia3   3
#define frecuencia4   4
#define frecuencia5   5
#define frecuencia6   6
#define frecuencia7   7
#define frecuencia8   8
#define frecuencia9   9
#define frecuencia10 10


/*  ESTRUCTURAS *
   ****************/
   
 typedef struct {
      byte		 state;       //on, off, toggle
      byte       frecuency;
      byte       cnt;
      byte*      port;
      byte*      portDDR;
      byte       pin;
   }led_TYPE;



/*

inicio                        led apagado
recibe systart                led frecuencia 1
inicia envio de apn,usr,pass  led frecuencia 2
inicia conexioon con host     led frecuencia5
inicia envio de attch         led frecuencia8
registro                      led encendido


*/


extern led_TYPE LED1;
extern led_TYPE LED2;

extern void ini_led1(void);
extern void ini_led2(void);
extern void clr_bit (byte* port, byte pin);
extern void set_bit (byte* port, byte pin);
extern void toggle_bit(led_TYPE* led);
extern void timer_LED (led_TYPE* led);
extern byte test_bit(byte* port, byte pin);
void reloadTOGGLE_time (led_TYPE* led);
void on_LED(led_TYPE* led);
void off_LED(led_TYPE* led);
void toggle_LED(led_TYPE* led,byte frecuency);



#endif   
 

