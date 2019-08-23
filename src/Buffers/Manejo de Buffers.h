/* File <Manejo de Buffers.h> */
#include "stdint.h"

#ifndef _MANEJO_DE_BUFFERS_
  #define _MANEJO_DE_BUFFERS_

  /* ESTRUCTURAS */
  /***************/
   // #include "- Visor Config -.h"
 #include "Definicion de tipos.h"
    // BUFFERS FLAGS
      typedef union {
          byte Byte;
          struct {
            byte  full    :1;   // Indica que no hay espacio en el Buffer correspondiente (poco distancia entre Rx y Tx)
            byte  :1;
            byte  :1;
            byte  :1;
            byte  :1;
            byte  :1;
            byte  :1;
            byte  :1;
          } Bits;
          
          struct {
            byte          :8;
          } MergedBits;
      } buffers_flags;

     extern buffers_flags           buffers_F1;
      #define _buffers_F1           buffers_F1.Byte
      #define buffer_full           buffers_F1.Bits.full


  typedef struct{
	   byte* 	put;          //direccion donde se va agregar el proximo dato
       byte* 	get;          //direccion de donde se va sacar el proximo dato
       byte* 	iniBUFFER;    //direccion del buffer
       uint16_t sizeBUFFER;   //tamaño del buffer
       uint16_t cntBYTE;      //contador de cantidad de bytes ingresados al buffer
       byte  	cntDATOS;     //cuenta cuantos datos se ingresaron al buffer (cantidad de comandos, de string, ..etc)
       byte  	fullBUFFER;   //indica buffer lleno
  }buffcirTYPE;




  /* RUTINAS */
  /***********/
    // MANEJO DE BUFFERS y PUNTEROS
    extern uint16_t diferencia_punteros (byte* put_ptr, byte* get_ptr, uint16_t buffer_long);
    extern void cargar_buffer (byte** putA_ptr_address, byte** getA_ptr_address, byte** getB_ptr_address, uint16_t long_paquete, byte* ptrA_ini_address, uint16_t bufferA_long, byte* ptrB_ini_address, uint16_t bufferB_long);
    extern void put_byte (byte** put_ptr_address, byte data_in, byte* ptr_ini_address, uint16_t buffer_long);
    extern byte get_byte (byte** get_ptr_address, byte* ptr_ini_address, uint16_t buffer_long);
    extern void inc_ptr (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long);
    extern void dec_ptr (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long);

    
    extern void string_copy (byte* dest_ptr, byte* orig_ptr);
    extern void string_copy_incDest (byte** dest_ptr, byte* orig_ptr);
    extern void string_N_copy (byte* dest_ptr, byte* orig_ptr, byte N);
    void stringCopyN (byte* dest_ptr, byte* orig_ptr, uint16_t N);
    extern void string_concat (byte* dest_ptr, byte* orig_ptr);
    extern void string_N_concat (byte* dest_ptr, byte* orig_ptr, byte N);
    extern uint16_t string_length (byte* orig_ptr);
    extern byte string_N_compare (byte* ptr1, byte* ptr2, byte N);
    extern byte string_N_compareBUFCIR (buffcirTYPE* rxDATA, byte offsetDATA, byte* ptr2, byte N);
    extern byte string_compare (byte* ptr1, byte* ptr2);
    extern byte buffer_N_compare (byte* ptr1, byte* ptr2, byte N);

    extern void buffer_N_copy (byte* dest_ptr, byte* orig_ptr, byte N);
    
    extern void verificar_fin_buffer (byte** ptr_address, byte* ptr_ini_address, uint16_t buffer_long);
    
    /* manejo de BUFCIR*/
    extern void putBUFCIR (byte dat, buffcirTYPE* DATA);
    extern byte getBUFCIR (buffcirTYPE* DATA);
    extern void incGET_BUFCIR (buffcirTYPE* DATA);
    extern void decGET_BUFCIR (buffcirTYPE* DATA);
    extern void dec_cntDATOS_BUFCIR (buffcirTYPE* DATA);
    extern byte check_cntDATOS_BUFCIR(buffcirTYPE* DATA);
    extern byte* take_ptrGET_BUFCIR(buffcirTYPE* DATA);
    extern void incN_ptrDATA (byte* ptrDATA, byte* iniBUFFER, uint16_t sizeBUFFER, byte N);
    extern void decN_ptrDATA (byte* ptrDATA, byte* iniBUFFER, uint16_t sizeBUFFER, byte N);
    extern uint16_t chkSpace_BuffCIR (buffcirTYPE* buff);
  
  #ifdef VISOR_HCS12X    
    extern void put_byte_PAGED (byte*__rptr* put_ptr_address, byte data_in, byte*__rptr ptr_ini_address, uint16_t buffer_long);
    extern byte get_byte_PAGED (byte*__rptr* get_ptr_address, byte*__rptr ptr_ini_address, uint16_t buffer_long);
    extern void inc_ptr_PAGED (byte*__rptr* ptr_address, byte*__rptr ptr_ini_address, uint16_t buffer_long);
    extern void verificar_fin_buffer_PAGED (byte*__rptr* ptr_address, byte*__rptr ptr_ini_address, uint16_t buffer_long);
  #endif    
#endif
