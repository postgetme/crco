;******************************************************************************
;
;                             INTERRUPT VECTORS
;                                    ARM
;                             KEIL's uVision3 
;                   (RealView Microprocessor Developer Kit)
;
; Filename      : vectors.s
;******************************************************************************

                PRESERVE8
                AREA   VECT, CODE, READONLY                     ; Name this block of code                                   ;
                THUMB

                ENTRY

;******************************************************************************
;                                  IMPORTS
;******************************************************************************

		        IMPORT	ucos_tick_isr
		        IMPORT	OSPendSV
                IMPORT  ResetHndlr
                IMPORT  ||Image$$ARM_LIB_STACK$$ZI$$Limit||     ; Import stack limit from scatter-loading file              ;

;******************************************************************************
;                                  EXPORTS
;******************************************************************************


;******************************************************************************
;                                DEFINITIONS
;******************************************************************************


;******************************************************************************
;                      INITIALIZE EXCEPTION VECTORS
;******************************************************************************

Vectors
        DCD     ||Image$$ARM_LIB_STACK$$ZI$$Limit||         ;  0, SP start value.                                         
        DCD     ResetHndlr                                  ;  1, PC start value.                                         
        DCD     NmiISR                                      ;  2, NMI         
        DCD     FaultISR                                    ;  3, Hard Fault  
        DCD     DefaultISR_Handler                          ;  4, Memory Management
        DCD     DefaultISR_Handler                          ;  5, Bus Fault         
        DCD     DefaultISR_Handler                          ;  6, Usage Fault                
        DCD     0                                           ;  7, Reserved     
        DCD     0                                           ;  8, Reserved   
        DCD     0                                           ;  9, Reserved    
        DCD     0                                           ; 10, Reserved               
        DCD     DefaultISR_Handler                          ; 11, SVCall
        DCD     DefaultISR_Handler                          ; 12, Debug Monitor
        DCD     0                                           ; 13, Reserved
        DCD     OSPendSV                                    ; 14, PendSV Handler
        DCD     ucos_tick_isr                         ; 15, uC/OS-II Tick ISR Handler                    
        DCD     DefaultISR_Handler                          ; 16, INTISR[  0]  GPIOA     
        DCD     DefaultISR_Handler                       	; 17, INTISR[  1]  GPIOB     
        DCD     DefaultISR_Handler                          ; 18, INTISR[  2]  GPIOC     
        DCD     DefaultISR_Handler                          ; 19, INTISR[  3]  GPIOD     
        DCD     DefaultISR_Handler                          ; 20, INTISR[  4]  GPIOE     
        DCD     DefaultISR_Handler			                ; 21, INTISR[  5]  UART0
        DCD     DefaultISR_Handler                          ; 22, INTISR[  6]  UART1
        DCD     DefaultISR_Handler                          ; 23, INTISR[  7]  SSI0
        DCD     DefaultISR_Handler                          ; 24, INTISR[  8]  I2C0
        DCD     DefaultISR_Handler                          ; 25, INTISR[  9]  PWM Fault      
        DCD     DefaultISR_Handler                          ; 26, INTISR[ 10]  PWM Generator 0    
        DCD     DefaultISR_Handler                          ; 27, INTISR[ 11]  PWM Generator 1       
        DCD     DefaultISR_Handler                          ; 28, INTISR[ 12]  PWM Generator 2  
        DCD     DefaultISR_Handler                          ; 29, INTISR[ 13]  QEI 0       
        DCD     DefaultISR_Handler                          ; 30, INTISR[ 14]  ADC Sequence 0 
        DCD     DefaultISR_Handler                          ; 31, INTISR[ 15]  ADC Sequence 1 
        DCD     DefaultISR_Handler                          ; 32, INTISR[ 16]  ADC Sequence 2 
        DCD     DefaultISR_Handler                          ; 33, INTISR[ 17]  ADC Sequence 3 
        DCD     DefaultISR_Handler                          ; 34, INTISR[ 18]  Watchdog Timer
        DCD     DefaultISR_Handler                          ; 35, INTISR[ 19]  Timer0 A
        DCD     DefaultISR_Handler                          ; 36, INTISR[ 20]  Timer0 B
        DCD     DefaultISR_Handler                          ; 37, INTISR[ 21]  Timer1 A
        DCD     DefaultISR_Handler                          ; 38, INTISR[ 22]  Timer1 B
        DCD     DefaultISR_Handler                          ; 39, INTISR[ 23]  Timer2 A
        DCD     DefaultISR_Handler                          ; 40, INTISR[ 24]  Timer2 B
        DCD     DefaultISR_Handler                          ; 41, INTISR[ 25]  Analog Comparator 0 (S/F)
        DCD     DefaultISR_Handler                          ; 42, INTISR[ 26]  Analog Comparator 1 (S/F)
        DCD     DefaultISR_Handler                          ; 43, INTISR[ 27]  Analog Comparator 2 (S/F)
        DCD     DefaultISR_Handler                          ; 44, INTISR[ 28]  System Control
        DCD     DefaultISR_Handler                          ; 45, INTISR[ 29]  Flash Control
        DCD     DefaultISR_Handler                          ; 46, INTISR[ 30]  GPIO Port F            
        DCD     DefaultISR_Handler                          ; 47, INTISR[ 31]  GPIO Port G        
        DCD     DefaultISR_Handler                          ; 48, INTISR[ 32]  GPIO Port H        
        DCD     DefaultISR_Handler                          ; 49, INTISR[ 33]  UART2               
        DCD     DefaultISR_Handler                          ; 50, INTISR[ 34]  SSI1                 
        DCD     DefaultISR_Handler                          ; 51, INTISR[ 35]  Timer3 A          
        DCD     DefaultISR_Handler                          ; 52, INTISR[ 36]  Timer3 B           
        DCD     DefaultISR_Handler                          ; 53, INTISR[ 37]  I2C1
        DCD     DefaultISR_Handler                          ; 54, INTISR[ 38]  QEI1               
        DCD     DefaultISR_Handler           ; CAN0
        DCD     DefaultISR_Handler           ; CAN1
        DCD     DefaultISR_Handler           ; CAN2
        EXTERN  EthernetIntHandler
        DCD     EthernetIntHandler          ; Ethernet
        DCD     DefaultISR_Handler           ; Hibernate
        DCD     DefaultISR_Handler           ; USB0
        DCD     DefaultISR_Handler           ; PWM Generator 3
        DCD     DefaultISR_Handler           ; uDMA Software Transfer
        DCD     DefaultISR_Handler           ; uDMA Error            
        
        
;******************************************************************************
;                          DEFAULT HANDLERS
;******************************************************************************

NmiISR              B       NmiISR

FaultISR            B       FaultISR

DefaultISR_Handler  B       DefaultISR_Handler


                ALIGN
                END