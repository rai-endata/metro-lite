/* File <Manejo de Buffers.c> */
/*
  Aqui se encuentran todas la rutinas referentes al manejo de buffers. NO A SU IMPLEMENTACION
  DIRECTA CON EL SOFT EN DESARROLLO, solo las rutinas de manejo. Toda operacion que se realice
  sobre un buffer deberá hacerse invocando a alguna de estas funciones para de este modo crear
  bloques de codigo independientes aplicables a cualquier programa.
*/


/* INCLUDES */
/************/
  //#include "- Visor Config -.h"
  //#include "General\HAL.h"                      // Hardware Abstraction Layer
  #include "Manejo de Buffers.h"
  #include "Eeprom.h"
    
/*********************************************************************************************/
/* PROTOTIPOS */
/**************/

  
  // RUTINAS INTERNAS
  static void guardar_en_buffer (byte** ptr_address, byte data_in);
  static byte extraer_de_buffer (byte** ptr_address);
  //static byte verificar_desbordamiento (byte** ptr_address, byte* fin_address);

  static void verificar_ini_buffer (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long);
  static void verificar_iniWord_buffer (word** ptr_address, word* ptr_ini_address, uint16_t buffer_long);
  static void verificar_fin_buffer (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long);
  static void verificar_finWord_buffer (word** ptr_address, word* ptr_ini_address, uint16_t buffer_long);

  static void guardarWord_en_buffer (word** ptr_address, word data_in);
  static word extraerWord_de_buffer (word** ptr_address);
  



/*********************************************************************************************/
/* VARIABLES */
/*************/
  buffers_flags   buffers_F1;    // Banderas de Control de Buffers


/*********************************************************************************************/
/* RUTINAS */
/***********/
  
  /* CALCULAR DISTANCIA ENTRE PUNTEROS */
  /*************************************/  
    uint16_t diferencia_punteros (byte* put_ptr, byte* get_ptr, uint16_t buffer_long){
      // Toma 2 punteros (no sus direcciones) y devuelve la distancia entre los mismos
      // Pensada para ser usada para saber si hay lugar entre el PUT y el GET.
      // Si solo se pretende saber la distancia entre punteros cualesquiera, la
      // distancia que calcula es GET->PUT
       uint16_t dist;
      
      if (put_ptr >= get_ptr){
        dist = put_ptr - get_ptr;
        dist = buffer_long - dist;
      }else{
        dist = get_ptr - put_ptr;
      }
      
      return(dist);
    }
    
  
   
  /* CARGAR BUFFER CIRCULAR DESDE BUFFER CIRCULAR  */
  /*************************************************/
    void cargar_buffer (byte** putB_ptr_address, byte** getB_ptr_address, byte** getA_ptr_address, uint16_t long_paquete, byte* ptrB_ini_address, uint16_t bufferB_long, byte* ptrA_ini_address,uint16_t bufferA_long ){
      /*
        Hay 2 buffers en juego, el de origen de los datos (A) y el de destino de los datos (B). Se pasan como argumentos las direcciones de los punteros correspondientes a los buffers en cuestion.
        De este modo:
        
          &putB_ptr_address   = $4000  (direccion)
            *putB_ptr_address = $4500  (contenido)
          
          &getB_ptr_address   = $4002  (direccion)
            *getB_ptr_address = $4510  (contenido)           
          
          &getA_ptr_address   = $4004  (direccion)
            *getA_ptr_address = $4140  (contenido)
            
            																											BUFFER A  													BUFFER B
            																										  (origen)	 									  		  (destino)	 
       MAPA DE MEMORIA 
         +---------+                                            +------------+                      +------------+
      	 |  $4500  |  <-- $4000  (putB)													|            | <-- getA							|            |
      	 |---------|																						|            |											|            |
      	 |  $4510	 |  <-- $4002  (getB)													|            |						putB	-->	|            |
      	 |---------|																						|            |											|            |
      	 |	$4140	 |  <-- $4004  (getA)													|            |											|            |
      	 |---------|																						|            |											|            |
      	 |				 |																						|            |											|            |
      	 |				 |																						|            |											|            |
      	 |				 |																						|            |											|            | <-- getB
      	 |				 |																						|            |											|            |
      	 |				 |																						+------------+											+------------+
      	 |				 |
         |---------|
      	 | BUFFER A| <- Aca dentro estaria apuntando getA
      	 |---------|
      	 | BUFFER B| <- Aca dentro estaria apuntando getB y putB
      	 |---------|
        
        
        Otros datos que se pasan son:
          - Longitud del paquete: cantidad de bytes a copiar desde BUFFER A hacia BUFFER B. Esto no solo sirve para saber cuando terminar de copiar de A
                                  sino que tambien lo utilizo para saber si hay espacio en B para esa cantidad de datos. Es decir que la distancia entre
                                  los punteros get y put de B sea tal que me permita copiar esa cantidad de datos
                                  
          - Direccion de inicio del puntero y longitud del buffer que maneja: Esto es tanto para A como para B y se usa para 'dar la vuelta' al buffer
                                  circular. Xq con estos dos datos se donde termina el buffer (inicio + longitud) y al darlo vuelta hago que el puntero
                                  apunte al inicio del buffer
                                  
                                  
           BUFFER A  													      BUFFER B
           (origen)	 									  		        (destino)	 
       
         +------------+ <-- inicio Buffer A       +------------+ <-- inicio Buffer B	 /
      	 |  /         | <-- getA  		  			    |            |											 |
      	 |  |         |					      						|            |											 |
      	 |  | long.   |						      putB	-->	| /          |											 |
      	 |  | paquete |									      		| |          |											 |
      	 |  |         |	      										| | dist.    |											 | longitud de Buffer
      	 |  /         |	      										| | entre    |											 | (no necesariamente
      	 |            |			      								| | punteros |											 | iguales)
      	 |            |					      						| |          |											 |
      	 |            |						      					| /          | <-- getB							 |
      	 |            |									      		|            |											 |
      	 +------------+											      +------------+											 /
      	               
       
        La rutina va tomando datos de A siguiendo a getA y los va a colocar en B, segun putB, siempre y cuando haya lugar en B. De esta forma me aseguro
        de no pisar el puntero getB. E ira copiando los datos hasta alcanzar la longitud del paquete que se indico como argumento.
      */
      byte* putB_ptr;
      byte* getB_ptr;
      uint16_t  dist_ptrB;
      byte dato;
      
      putB_ptr = *(putB_ptr_address);       // Puntero PUT apuntando al contenido de la direccion de memoria del puntero de PUT
      getB_ptr = *(getB_ptr_address);       // Puntero GET apuntando al contenido de la direccion de memoria del puntero de GET
      
      dist_ptrB = diferencia_punteros (putB_ptr, getB_ptr, bufferB_long);  // Distancia entre punteros de destino

      // La distancia entre punteros debe ser mayor a la long_paquete xq si es igual, cuando termina de hacer la copia de buffer, ambos
      // punteros (put y get) quedan igualados. Esto hace que la proxima vez que compare distancia de punteros, le de como resultado
      // que hay TODO EL BUFFER de espacio, cuando en realidad al puintero GET, le falta dar toda una vuelta y sacar los paquetes
      // acumulados previamente
      if (dist_ptrB > long_paquete){
        while (long_paquete != 0){
          dato = get_byte(getA_ptr_address, ptrA_ini_address, bufferA_long);
          put_byte(putB_ptr_address, dato, ptrB_ini_address, bufferB_long);
          long_paquete--;
        }
        buffer_full = 0;            // hubo lugar para el paquete en el Buffer
      }else{
        buffer_full = 1;            // NO hay lugar para el paquete en el Buffer
      }
    }
    
  
  
  /* GUARDAR BYTE EN BUFFER CIRCULAR */
  /***********************************/
    void put_byte (byte** put_ptr_address, byte data_in, byte* ptr_ini_address, uint16_t buffer_long){
      /*
        Dada la direccion de memoria de un puntero, va colocando el dato que se le pasa como argumento
        en la direccion apuntada por el contenido de la direccion del puntero.
        Al guardar un byte, incrementa el puntero y se fija si debe dar la vuelta o no
      */
      guardar_en_buffer(put_ptr_address, data_in);   // Guarda Byte e Incrementa puntero
      verificar_fin_buffer(put_ptr_address, ptr_ini_address, buffer_long);     // Verifica que el incremento no rebase el destino y si lo hace da la vuelta
    }


/* GUARDAR WORD EN BUFFER CIRCULAR */
/***********************************/
void put_word (word** put_ptr_address, word data_in, word* ptr_ini_address, uint16_t buffer_long){
  /*
	Dada la direccion de memoria de un puntero, va colocando el dato que se le pasa como argumento
	en la direccion apuntada por el contenido de la direccion del puntero.
	Al guardar un byte, incrementa el puntero y se fija si debe dar la vuelta o no
  */
  guardarWord_en_buffer(put_ptr_address, data_in);   // Guarda Byte e Incrementa puntero
  verificar_finWord_buffer(put_ptr_address, ptr_ini_address, buffer_long);     // Verifica que el incremento no rebase el destino y si lo hace da la vuelta
}

    

  /* EXTRAER BYTE DE BUFFER CIRCULAR */
  /***********************************/ 
    byte get_byte (byte** get_ptr_address, byte* ptr_ini_address, uint16_t buffer_long){
      /*
        Analogo a GUARDAR BYTE... Se le pasa la direccion de memoria del puntero segun el cual se quieren
        extraer los datos, extrae los datos en una variable volatil y luego incrementa este puntero. Y se
        fija si debe dar la vuelta o no.
        El valor extraido es devuelto por la funcion como un argumento de salida
      */
       byte data_out;
      
      data_out = extraer_de_buffer(get_ptr_address);                           // extrae Byte e Incrementa puntero
      verificar_fin_buffer(get_ptr_address, ptr_ini_address, buffer_long);     // verifica que el incremento no rebase el destino y si lo hace da la vuelta
      
      return(data_out);
    }

    /* EXTRAER WORD DE BUFFER CIRCULAR */
     /***********************************/
       word get_word (word** get_ptr_address, word* ptr_ini_address, uint16_t buffer_long){
         /*
           Analogo a GUARDAR BYTE... Se le pasa la direccion de memoria del puntero segun el cual se quieren
           extraer los datos, extrae los datos en una variable volatil y luego incrementa este puntero. Y se
           fija si debe dar la vuelta o no.
           El valor extraido es devuelto por la funcion como un argumento de salida
         */
         word data_out;

         data_out = extraerWord_de_buffer(get_ptr_address);                           // extrae Byte e Incrementa puntero
         verificar_finWord_buffer(get_ptr_address, ptr_ini_address, buffer_long);     // verifica que el incremento no rebase el destino y si lo hace da la vuelta

         return(data_out);
       }


  /* INCREMENTAR PUNTERO A BUFFER CIRCULAR */
  /*****************************************/  
    void inc_ptr (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long){
    /*
      Incrementa el puntero y verifica que no se rebase. Es decir, da la vuelta si corresponde
    */
      (*ptr_address)++;               // Incrementar el contenido de la posicion de memoria del puntero => o sea, el puntero
      verificar_fin_buffer(ptr_address, ptr_ini_address, buffer_long);     // Verifica que el incremento no rebase el destino y si lo hace da la vuelta
    }

	/* INCREMENTAR PUNTERO A BUFFER CIRCULAR */
	/*****************************************/
	void dec_ptr (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long){
	/*
	  decrementa el puntero y verifica que no se rebase. Es decir, da la vuelta si corresponde
	*/
	  (*ptr_address)--;               // Incrementar el contenido de la posicion de memoria del puntero => o sea, el puntero
	  verificar_ini_buffer(ptr_address, ptr_ini_address, buffer_long);     // Verifica que el incremento no rebase el destino y si lo hace da la vuelta
	}


    /* INCREMENTAR PUNTERO A BUFFER CIRCULAR */
    /*****************************************/
      void incWord_ptr (word** ptr_address, word* ptr_ini_address, uint16_t buffer_long){
      /*
        Incrementa el puntero y verifica que no se rebase. Es decir, da la vuelta si corresponde
      */
        (*ptr_address)++;               // Incrementar el contenido de la posicion de memoria del puntero => o sea, el puntero
        verificar_finWord_buffer(ptr_address, ptr_ini_address, buffer_long);     // Verifica que el incremento no rebase el destino y si lo hace da la vuelta
      }


/* INCREMENTAR PUNTERO A BUFFER CIRCULAR */
  /*****************************************/  
    void decWord_ptr (word** ptr_address, word* ptr_ini_address, uint16_t buffer_long){
    /*
      decrementa el puntero y verifica que no se rebase. Es decir, da la vuelta si corresponde
    */
      (*ptr_address)--;               // Incrementar el contenido de la posicion de memoria del puntero => o sea, el puntero
      verificar_iniWord_buffer(ptr_address, ptr_ini_address, buffer_long);     // Verifica que el incremento no rebase el destino y si lo hace da la vuelta
    }


  /* STRING COPY */
  /***************/
    void string_copy (byte* dest_ptr, byte* orig_ptr){
      while (*orig_ptr != 0){
        *dest_ptr++ = *orig_ptr++;
      }
      
      *dest_ptr = 0;
    }


    /* STRING COPY */
      /***************/
        void string_copy_incDest (byte** dest_ptr, byte* orig_ptr){
          while (*orig_ptr != 0){
            *(*dest_ptr) = *orig_ptr++;
            (*dest_ptr)++;
          }

          *(*dest_ptr) = 0;
        }


        /* STRING COPY DEVUELVE CANTIDAD DE DATOS COPIADOS */
          /***************/

        uint16_t string_copy_returnN (byte* dest_ptr, byte* orig_ptr){
            uint16_t n;
            n=0;
        	while (*orig_ptr != 0){
               *dest_ptr++ = *orig_ptr++;
               n++;
             }

             *dest_ptr = 0;
             return(n);
        }


  /* STRING N COPY */
  /*****************/
    void string_N_copy (byte* dest_ptr, byte* orig_ptr, byte N){
      while (*orig_ptr != 0 && N>0){
        *dest_ptr++ = *orig_ptr++;
        N--;
      }
      
      *dest_ptr = 0;
    }

    void stringCopyN (byte* dest_ptr, byte* orig_ptr, uint16_t N){
          while (N>0){
            *dest_ptr++ = *orig_ptr++;
            N--;
          }

          *dest_ptr = 0;
        }

    void bufferNcopy (byte* dest_ptr, byte* orig_ptr, byte N){
       while (N>0){
         *dest_ptr++ = *orig_ptr++;
         N--;
       }
    }

    void bufferNcopy_incDst (byte** dest_ptr, byte* orig_ptr, byte N){
           while (N>0){
             *(*dest_ptr) = *orig_ptr++;
             (*dest_ptr)++;
             N--;
           }
    }


  /* STRING CONCAT */
  /*****************/
    void string_concat (byte* dest_ptr, byte* orig_ptr){
      while (*dest_ptr != 0){
        dest_ptr++;
      }
      
      string_copy(dest_ptr, orig_ptr);
    }


  /* STRING CONCAT */
  /*****************/
    void string_N_concat (byte* dest_ptr, byte* orig_ptr, byte N){
      while (*dest_ptr != 0){
        dest_ptr++;
      }
      
      string_N_copy(dest_ptr, orig_ptr, N);
    }    


  /* STRING LENGTH */
  /*****************/
    uint16_t string_length (byte* orig_ptr){
      uint16_t len;
      
      len = 0;
      while (*orig_ptr++ != 0){
        len++;
      }
      
      return(len);
    }


  /* STRING N COMPARE */
  /********************/
    byte string_N_compare (byte* ptr1, byte* ptr2, byte N){
      // EDIT 03/10/12
      //  ERROR!!!!!! Lo primero que hace es asumir que NO son
      // diferentes. Pero en realidad, si alguno de los 2 es
      // el FIN de cadena => No ingresa al WHILE y por lo tanto
      // no compara nada y sale diciendo que NO SON DIFERENTES,
      // cuando en realidad podrian serlo
      // => Antes de ingresar al while comparo el contenido de
      // los punteros
      byte differents;
      
      if (*ptr1 != *ptr2){
        differents = 1;       // Al arrancar son diferentes
      }else{
        differents = 0;       // Al arrancar son iguales
      }
      
      // Si el N no es nulo, le resto uno, xq lo acabo de comparar
      if (N>0){
        N--;
      }
      
      while (N>0 && *ptr1 != 0 && *ptr2 != 0 && !differents){
        N--;
        ptr1++;
        ptr2++;
        if (*ptr1 != *ptr2){
          differents = 1;
        }
      }
      
      return(differents);
    }    
    
/* STRING N COMPARE BUFCIR */
/***************************/
    
    byte string_N_compareBUFCIR (buffcirTYPE* rxDATA, byte offsetDATA, byte* ptr2, byte N){
      // EDIT 03/10/12
      //  ERROR!!!!!! Lo primero que hace es asumir que NO son
      // diferentes. Pero en realidad, si alguno de los 2 es
      // el FIN de cadena => No ingresa al WHILE y por lo tanto
      // no compara nada y sale diciendo que NO SON DIFERENTES,
      // cuando en realidad podrian serlo
      // => Antes de ingresar al while comparo el contenido de
      // los punteros
      // Compara dos cadenas de texto. Uno esta ubicado en un buffer circular cuyo inicio viene
      // dado por ptr1, y un buffer lineal cuyo inicio viene dado por ptr2.
      
      byte differents;
      byte *ptr1;
      
      
      //para apuntar a inicio de datos hago ptr1 = rxDATA->get + offsetDATA 
      while (offsetDATA>0){
      incGET_BUFCIR (rxDATA); 
       offsetDATA--;
      }
      ptr1 = rxDATA->get;               //ptr1 = rxDATA->get + offsetDATA;  
      rxDATA->get = ptr1;               //guardo inicio de datos
       
      if (*ptr1 != *ptr2){
        differents = 1;       // Al arrancar son diferentes
      }else{
        differents = 0;       // Al arrancar son iguales
      }
      
      // Si el N no es nulo, le resto uno, xq lo acabo de comparar
      if (N>0){
        N--;
      }
      
      while (N>0 && *ptr1 != 0 && *ptr2 != 0 && !differents){
        N--;
        incGET_BUFCIR (rxDATA);
        ptr1 = rxDATA->get;
        ptr2++;
        if (*ptr1 != *ptr2){
          differents = 1;
        }
      }
      
      return(differents);
    }       



  


  /* STRING COMPARE */
  /******************/
    byte string_compare (byte* ptr1, byte* ptr2){
      // EDIT 02/10/12
      //  ERROR!!!!!! Lo primero que hace es asumir que NO son
      // diferentes. Pero en realidad, si alguno de los 2 es
      // el FIN de cadena => No ingresa al WHILE y por lo tanto
      // no compara nada y sale diciendo que NO SON DIFERENTES,
      // cuando en realidad podrian serlo
      // => Antes de ingresar al while comparo el contenido de
      // los punteros
      byte differents;
      
      if (*ptr1 != *ptr2){
        differents = 1;       // Al arrancar son diferentes
      }else{
        differents = 0;       // Al arrancar son iguales
      }
      
      while (*ptr1 != 0 && *ptr2 != 0 && !differents){
        ptr1++;
        ptr2++;
        if (*ptr1 != *ptr2){
          differents = 1;
        }
      }
      
      return(differents);
    }

/* BUFFER N COMPARE */
  /********************/
    byte buffer_N_compare (byte* ptr1, byte* ptr2, byte N){
      // EDIT 03/10/12
      //  ERROR!!!!!! Lo primero que hace es asumir que NO son
      // diferentes. Pero en realidad, si alguno de los 2 es
      // el FIN de cadena => No ingresa al WHILE y por lo tanto
      // no compara nada y sale diciendo que NO SON DIFERENTES,
      // cuando en realidad podrian serlo
      // => Antes de ingresar al while comparo el contenido de
      // los punteros
      byte differents;
      
      if (*ptr1 != *ptr2){
        differents = 1;       // Al arrancar son diferentes
      }else{
        differents = 0;       // Al arrancar son iguales
      }
      
      // Si el N no es nulo, le resto uno, xq lo acabo de comparar
      if (N>0){
        N--;
      }
      
      while (N>0 && !differents){
        N--;
        ptr1++;
        ptr2++;
        if (*ptr1 != *ptr2){
          differents = 1;
        }
      }
      
      return(differents);
    }    


  /* BUFFER N COPY */
  /*****************/
    void buffer_N_copy (byte* dest_ptr, byte* orig_ptr, byte N){
      while (N > 0){
        *dest_ptr++ = *orig_ptr++;
        N--;
      }
      
      *dest_ptr = 0;
    }    

  
      
/*********************************************************************************************/
/* RUTINAS INTERNAS */
/********************/

  /* GUARDAR BYTE EN BUFFER */
  /**************************/
    static void guardar_en_buffer (byte** ptr_address, byte data_in){
      /*
         LE PASO COMO ARGUMENTO LA DIRECCION DE MEMORIA DEL PUNTERO Y NO EL PUNTERO, PARA
         PODER INCREMENTAR EL PUNTERO. SINO LO QUE HARIA ES INCREMENTAR EL PUNTERO VOLATIL3
         CREADO AL PASARLO COMO ARGUMENTO Y NO EL PUNTERO REAL
         
      	 &puntero = $4000  (direccion)
      	  *puntero = $4011 (contenido)
      	 
      	 &BUFFER = $4011   (direccion)
      	 
      	 notar que el puntero apunta al inicio del BUFFER, xq su contenido
      	 es la direccion del BUFFER
      	 
      	 +---------+
      	 |  $4011  |  <-- $4000  (puntero)
      	 |---------|
      	 |				 |
      	 |				 |
      	 |---------|
      	 | BUFFER  |  <-- $4011
      	 |---------|
      
      	 - A la rutina le paso la direccion del puntero => ptr_address = $4000 (x eso la defino uint16_t*)
      	 
      	 - Luego hago que mi puntero auxiliar apunte a al contenido de ptr_address
      	   => aux_ptr = *(ptr_address) = *($4000) = $4011
      	   
      	   por lo tanto, apunto al buffer
      	 
      	 - Luego ahi guardo el dato
      	 
      	 - Finalmente, incremento el contenido de la direccion del puntero, o sea
      	   incremento la direccion donde aputa mi puntero
      	   => (*ptr_address)++ = (*$4000)++ = ($4011)++ = $4012 apunto a prox posicion del buffer
      	   
      	   luego debere comparar que no rebase el buffer
      */
      byte* aux_ptr;
      
      aux_ptr = *ptr_address;
      *aux_ptr = data_in;
      
      
      (*ptr_address)++;
    }


    /* GUARDAR BYTE EN BUFFER */
      /**************************/
static void guardarWord_en_buffer (word** ptr_address, word data_in){

/*
	 LE PASO COMO ARGUMENTO LA DIRECCION DE MEMORIA DEL PUNTERO Y NO EL PUNTERO, PARA
	 PODER INCREMENTAR EL PUNTERO. SINO LO QUE HARIA ES INCREMENTAR EL PUNTERO VOLATIL3
	 CREADO AL PASARLO COMO ARGUMENTO Y NO EL PUNTERO REAL
*/
	**ptr_address = data_in;
    (*ptr_address)++;
}



  /* EXTRAE BYTE DE BUFFER */
  /*************************/
    static byte extraer_de_buffer (byte** ptr_address){
      // Simetrica a guardar_en_buffer
       byte data_out;

      data_out = **ptr_address;
      
      (*ptr_address)++;
      
      return(data_out);
    }    

    /* EXTRAE WORD DE BUFFER */
      /*************************/
	static word extraerWord_de_buffer (word** ptr_address){
	  // Simetrica a guardar_en_buffer
	   word data_out;

	  data_out = **ptr_address;

	  (*ptr_address)++;

	  return(data_out);
	}

	/* VERIFICAR FIN BUFFER CIRCULAR */
	  /*********************************/
	  static  void verificar_fin_buffer (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long){
	      /*
	        Toma la direccion de un puntero, la direccion de inicio del buffer que maneja y la longitud
	        de dicho buffer.
	        Un puntero auxliar (ptr_fin) apunta al fin del buffer, o sea la direccion de inicio del buffer
	        + la longitud.
	        Luego se comparan estos dos punteros y si son iguales, es decir, se alcanzo el fin; se modifica
	        el contenido de la posicion de memoria del puntero, para hacer que el mismo apunte al inicio de
	        su buffer
	      */
	      byte* ptr_fin;
	      byte* ptr;


	      ptr_fin = (ptr_ini_address);
	      ptr_fin += buffer_long;

	      ptr = *(ptr_address);               // Puntero apuntando al contenido de la direccion de memoria del puntero

	      if (ptr >= ptr_fin){
	        (*ptr_address) = ptr_ini_address; // Contenido de la direccion del puntero que apunte al inicio del buffer
	      }
	    }



	    /* VERIFICAR FIN BUFFER CIRCULAR */
	    /*********************************/
	    static  void verificar_finWord_buffer (word** ptr_address, word* ptr_ini_address, uint16_t buffer_long){
	        /*
	          Toma la direccion de un puntero, la direccion de inicio del buffer que maneja y la longitud
	          de dicho buffer.
	          Un puntero auxliar (ptr_fin) apunta al fin del buffer, o sea la direccion de inicio del buffer
	          + la longitud.
	          Luego se comparan estos dos punteros y si son iguales, es decir, se alcanzo el fin; se modifica
	          el contenido de la posicion de memoria del puntero, para hacer que el mismo apunte al inicio de
	          su buffer
	        */
	        word* ptr_fin;
	        word* ptr;


	        ptr_fin = (ptr_ini_address);
	        ptr_fin += buffer_long;

	        ptr = *(ptr_address);               // Puntero apuntando al contenido de la direccion de memoria del puntero

	        if (ptr >= ptr_fin){
	          (*ptr_address) = ptr_ini_address; // Contenido de la direccion del puntero que apunte al inicio del buffer
	        }
	      }


  /* VERIFICAR INICIO BUFFER CIRCULAR */
  /************************************/  
    static void verificar_ini_buffer (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long){

      if (*ptr_address < ptr_ini_address){
        *ptr_address = (ptr_ini_address + buffer_long) - 1;  // Contenido de la direccion del puntero que apunte al inicio del buffer
      }
    }      
    
	/* VERIFICAR INICIO BUFFER CIRCULAR */
	/************************************/
	static void verificar_iniWord_buffer (word** ptr_address, word* ptr_ini_address, uint16_t buffer_long){

	  if (*ptr_address < ptr_ini_address){
		*ptr_address = (ptr_ini_address + buffer_long) - 1;  // Contenido de la direccion del puntero que apunte al inicio del buffer
	  }
	}


  /* VERIFICAR DESBORDAMIENTO */
  /****************************/
   /*
    static byte verificar_desbordamiento (byte** ptr_address, byte* fin_address){

         //Se fija si el BUFFER LINEAL no fue desbordado, es decir que el puntero
         //no supero el fin del buffer.
         //Tambien necesito pasar la direccion del puntero, para asi poder modificar
         //a donde apunta el puntero, modificando el contedido de su direccion

      if (*ptr_address > fin_address){
        return(1);          // error de desbordamiento de buffer
      }else{
        return(0);          // NO error de desbordamiento de buffer
      }
    }  
  */
 /******************************/
 /* MANEJO DE BUFFERS CIRCULAR */
 /**************************************************************************************************/
  
      
  
/* AGREGAR DATOS (bytes) AL BUFFER  */
/************************************/
void putBUFCIR (byte dat, buffcirTYPE* DATA){
  if(!DATA->fullBUFFER){
    put_byte(&(DATA->put), dat, DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntBYTE++;
    if(DATA->put == DATA->get){
     DATA->fullBUFFER = 1;
    }
  }
}

/* AGREGAR DATOS (words) AL BUFFER  */
/************************************/
void putWord_BUFCIR (word dat, buffWordTYPE* DATA){
  if(!DATA->fullBUFFER){
    put_word(&(DATA->put), dat, DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntWORD++;
    if(DATA->put == DATA->get){
     DATA->fullBUFFER = 1;
    }
  }
}

/* SACAR DATO DEL BUFFER  */
/*******************************/

byte getBUFCIR (buffcirTYPE* DATA){
  byte dato;
  if(DATA->cntBYTE > 0){
    dato = get_byte(&(DATA->get), DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntBYTE--;
    DATA->fullBUFFER = 0;
    if(DATA->get == DATA->put && DATA->cntBYTE > 0){
     //nunca tendria que pasar por aca (programacion defensiva)
     DATA->cntBYTE = 0;
    }
  }
  return (dato);
}


/* SACAR DATO DEL BUFFER  */
/*******************************/

word getWord_BUFCIR (buffWordTYPE* DATA){
  word dato;
  if(DATA->cntWORD > 0){
    dato = get_word(&(DATA->get), DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntWORD--;
    DATA->fullBUFFER = 0;
    if(DATA->get == DATA->put && DATA->cntWORD > 0){
     //nunca tendria que pasar por aca (programacion defensiva)
     DATA->cntWORD = 0;
    }
  }
  return (dato);
}



/* INCREMENTAR ptr word GET  */
/*****************************/

/* INCREMENTAR ptr GET  */
/************************/

void incGET_BUFCIR (buffcirTYPE* DATA){
  if((DATA->cntBYTE > 0) && (DATA->get!= DATA->put)){
    inc_ptr(&(DATA->get), DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntBYTE--;
  }
}


/* DECREMENTAR ptr GET  */
/************************/

void decGET_BUFCIR (buffcirTYPE* DATA){
  if(DATA->get!= DATA->put){
    dec_ptr(&(DATA->get), DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntBYTE++;
  }
}


void incWord_BUFCIR (buffWordTYPE* DATA){
  if((DATA->cntWORD > 0) && (DATA->get!= DATA->put)){
    incWord_ptr(&(DATA->get), DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntWORD--;
  }
}


/* DECREMENTAR ptr GET  */
/************************/

void decWord_BUFCIR (buffWordTYPE* DATA){
  if(DATA->get!= DATA->put){
    decWord_ptr(&(DATA->get), DATA->iniBUFFER, (uint16_t) DATA->sizeBUFFER);
    DATA->cntWORD++;
  }
}

/* incrementar en N ptr  */
/*************************/


void incN_ptrDATA (byte* ptrDATA, byte* iniBUFFER, uint16_t sizeBUFFER, byte N){
   while(N>0){
    inc_ptr(ptrDATA, iniBUFFER, (uint16_t) sizeBUFFER);
    N--;
   }
}

/* decrementar en N ptr  */
/*************************/




void decN_ptrDATA (byte* ptrDATA, byte* iniBUFFER, uint16_t sizeBUFFER, byte N){
   while(N>0){
    dec_ptr(ptrDATA, iniBUFFER, (uint16_t) sizeBUFFER);
    N--;
   }
}

/* DECREMENTAR CONTADOR DE RECEPCIONES  */
/****************************************/

void dec_cntDATOS_BUFCIR (buffcirTYPE* DATA){
  DATA->cntDATOS--;
}


/* CHECKEAR CONTADOR DE RECEPCIONES  */
/*************************************/


byte check_cntDATOS_BUFCIR(buffcirTYPE* DATA){
   byte status;
   status = 0;
   if(DATA->cntDATOS > 0){
    status = 1;
   }
   return(status);
}

byte check_cntBYTE_BUFCIR(buffcirTYPE* DATA){
   byte status;
   status = 0;
   if(DATA->cntBYTE > 0){
    status = 1;
   }
   return(status);
}
/* TOMA PUNTERO GET  */
/*********************/

byte* take_ptrGET_BUFCIR(buffcirTYPE* DATA){
  return(DATA->get);
}



/* CALCULAR ESPACIO DISPONIBLE EN Buffer */
/*****************************************/
      uint16_t chkSpace_BuffCIR (buffcirTYPE* buff){
        uint16_t space;
        uint16_t queue_cnt;
        byte* GET;
        byte* PUT;

        GET = buff->get;
        PUT = buff->put;
        queue_cnt = buff->cntDATOS;     // Extriago cantidad de encolados

        if ((PUT == GET) && (queue_cnt != 0)){
          // Si los punteros GET y PUT son iguales y HAY paquetes encolados
          // => No hay mas lugar
          space = 0;

        }else if (GET > PUT){
          /*
                   +---------+
                   |         |
                   |         |
            PUT -> |         |
                   |*********|
                   |*********|
                   |*********|
                   |         | -> GET
                   |         |
                   +---------+


              => SPACE = GET - PUT
          */
          space = GET - PUT;

        }else{
          /*
                   +---------+
                   |*********|
                   |*********|
                   |         | -> GET
                   |         |
                   |         |
                   |         |
            PUT -> |         |
                   |*********|
                   +---------+


              => SPACE = DIM - (PUT - GET) = DIM - PUT + GET
          */
          space = buff->sizeBUFFER;
          space -= (PUT - GET);
        }

        return(space);
      }

  	word create_dataWord(byte hi, byte lo){

  		// resultado
  		// +------+------+
  		// |  hi  |  lo  |
  		// +------+------+

  		word resultado;
  		word aux;

  		resultado = (word)hi;
  		resultado = hi<<8;
  		aux = (word)lo;
  		resultado = resultado | aux;
  		return(resultado);
  	}

