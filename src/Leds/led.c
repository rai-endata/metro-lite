#include "led.h"

   typedef enum{
      OFF_LED,
      ON_LED,
      TOGGLE_LED,
    }led_STATE;




/*   VARIABLES *
 ***************/
led_TYPE LED1;
led_TYPE LED2;



 /*
void ini_(void){
    .cnt        = frecuencia3;
    .frecuency  = 1;
    .pin        = 2;            //PM2
    .port       = &PTM;
    .portDDR    = &DDRM;
    .state      = OFF_LED;
    set_bit (&DDRM,2);              //pone pm2 como salida 
}

void ini_led2(void){
    led2.pin        = 6;            //PM2
    led2.port       = &PTM;
    led2.portDDR    = &DDRM;
    led2.state      = OFF_LED;
    set_bit (&DDRM,6);              //pone pm2 como salida 
    clr_bit (&PTM,6);              //pone pm2 como salida 
}
*/

void on_LED(led_TYPE* led){
     led->state=ON_LED;
}


void off_LED(led_TYPE* led){
     led->state=OFF_LED;
}


void toggle_LED(led_TYPE* led, byte frecuency){
     led->state=TOGGLE_LED;
     led->frecuency=frecuency;
}

void set_frecuency_LED(void);

void set_frecuency_LED(void){

}






void clr_bit (byte* port, byte pin){
       if(pin==0){
         *port=*port&0xFE;
       }else if(pin==1){
         *port=*port&0xFD;
       }else if(pin==2){
         *port=*port&0xFB;
       }else if(pin==3){
         *port=*port&0xF7;
       }else if(pin==4){
         *port=*port&0xEF;
       }else if(pin==5){
         *port=*port&0xDF;
       }else if(pin==6){
         *port=*port&0xBF;
       }else if(pin==7){
         *port=*port&0x7F;
       }
}




void set_bit (byte* port, byte pin){
       if(pin==0){
         *port=*port|0x01;
       }else if(pin==1){
         *port=*port|0x02;
       }else if(pin==2){
         *port=*port|0x04;
       }else if(pin==3){
         *port=*port|0x08;
       }else if(pin==4){
         *port=*port|0x10;
       }else if(pin==5){
         *port=*port|0x20;
       }else if(pin==6){
         *port=*port|0x40;
       }else if(pin==7){
         *port=*port|0x80;
       }
}




byte test_bit(byte* port, byte pin){
       byte valor;
       
       if(pin==0){
         valor=*port&0x01;
       }else if(pin==1){
         valor=*port&0x02;
       }else if(pin==2){
         valor=*port&0x04;
       }else if(pin==3){
         valor=*port&0x08;
       }else if(pin==4){
         valor=*port&0x10;
       }else if(pin==5){
         valor=*port&0x20;
       }else if(pin==6){
         valor=*port&0x40;
       }else if(pin==7){
         valor=*port&0x80;
       }
       if(valor!=0){
          valor=1;
       }else{
          valor=0;
       }
    return(valor);
}



void toggle_bit(led_TYPE* led){
   byte state;

   state=test_bit(led->port,led->pin);
   if(state==1){
    clr_bit (led->port,led->pin);
   }else{
    set_bit (led->port,led->pin);
   }
}

void reloadTOGGLE_time (led_TYPE* led){
      if(led->frecuency==1){
       led->cnt=frecuencia1;
      }else if (led->frecuency==2){
       led->cnt=frecuencia2;
      }else if (led->frecuency==3){
       led->cnt=frecuencia3;
      }else if (led->frecuency==4){
       led->cnt=frecuencia4;
      }else if (led->frecuency==5){
       led->cnt=frecuencia5;
      }else if (led->frecuency==6){
       led->cnt=frecuencia6;
      }else if (led->frecuency==7){
       led->cnt=frecuencia7;
      }else if (led->frecuency==8){
       led->cnt=frecuencia8;
      }else if (led->frecuency==9){
       led->cnt=frecuencia9;
      }else if (led->frecuency==10){
       led->cnt=frecuencia10;
      }
}




/* timer led control */


void timer_LED (led_TYPE* led){
  
  if(led->state==OFF_LED){
    clr_bit (led->port,led->pin);
  }else if(led->state==ON_LED){
    set_bit (led->port,led->pin);
  }else if(led->state==TOGGLE_LED){
    if(led->cnt!=0){
      led->cnt--;
      if(led->cnt==0){
       toggle_bit(led); 
       reloadTOGGLE_time(led);
      }
    }
  }
}
