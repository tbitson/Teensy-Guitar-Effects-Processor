/*   utils. h - various support functions

   version 1.1.0   Dec 2019

   Various utility and helper functions





*/



// prototypes
void printValue(const char* msg);
void printValue(String, int);
void printValue(String, long);
void printValue(String, float);
void printValue(const char*, int, float);
void printArryValue(const char* , uint8_t, float);
void printAudioMemUsage();
void printCPUinfo();
void rebootTeensy();





// simple helper functions that print data to the serial port
// undefining DEBUG_PRINT disables all output
void printValue(const char* msg)
{
#ifdef DEBUG_PRINT
  Serial.print("msg = ");
  Serial.println(msg);
#endif
}


void printValue(const char* msg, int val)
{
#ifdef DEBUG_PRINT
  Serial.print(msg);
  Serial.print(" = ");
  Serial.println(val);
#endif
}


void printValue(const char* msg, long val)
{
#ifdef DEBUG_PRINT
  Serial.print(msg);
  Serial.print(" = ");
  Serial.println(val);
#endif
}


void printValue(const char* msg, float val)
{
#ifdef DEBUG_PRINT
  Serial.print(msg);
  Serial.print(" = ");
  Serial.println(val, 1);
#endif
}


void printValue(const char* msg, int val1, float val2)
{
#ifdef DEBUG_PRINT
  Serial.print(msg);
  Serial.print(" ");
  Serial.print(val1);
  Serial.print(" = ");
  Serial.println(val2);
#endif
}


void printArryValue(const char* msg, uint8_t index, float val)
{
#ifdef DEBUG_PRINT
  Serial.print(msg);
  Serial.print("[");
  Serial.print(index);
  Serial.print("] = ");
  Serial.println(val);
#endif
}



uint32_t teensyFreeMem()
{
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);
    return stackTop - heapTop;
}




void printAudioMemUsage()
{
  Serial.println(F("Memory Usage"));
  Serial.print(F("Freeverb Usage : ")); Serial.print(freeverb1.processorUsage());
  Serial.print(F("   max: "));       Serial.println(AudioProcessorUsageMax());

  Serial.print(F("AudioProcessor : ")); Serial.print(AudioProcessorUsage());
  Serial.print(F("   max: "));          Serial.println(AudioProcessorUsageMax());

  Serial.print(F("AudioMem       : ")); Serial.print(AudioMemoryUsage());
  Serial.print(F("   max: "));      Serial.println(AudioMemoryUsageMax());
  
  Serial.print(F("Free RAM       : ")); Serial.println(teensyFreeMem());  
  Serial.println();
}


// debugging aid - call to reboot teensy
//#define RESTART_ADDR 0xE000ED0C
//#define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
//#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

void rebootTeensy()
{
  /*  
  Serial.println("Are you sure?");
  uint32_t start = millis();
  while (millis() - start < 10000)
  {
    if (Serial.available())
    {
      char c = Serial.read();
      if (c == 'Y' || c == 'y')
      {
        WRITE_RESTART(0x5FA0004);
        delay(1000);
      }
      else
        Serial.println("Reboot Cancelled by user");
    }
    Serial.println("Reboot Cancelled by Timeout");
  }
 
  WRITE_RESTART(0x5FA0004);
  Serial.println("bye!");
  */
}



void printCPUinfo()
{
  Serial.println();
  Serial.print("Device is ");
#if defined(__MK20DX128__)
  Serial.println( "TEENSY_LC");
#define TEENSY_LC

#elif defined(__MKL26Z64__)
  Serial.println( "TEENSY 3.0");
#define TEENSY_30

#elif defined(__MK20DX256__)
  Serial.println( "TEENSY 3.2");
#define TEENSY_32

#elif defined(__MK64FX512__)
  Serial.println( "CTEENSY 3.5");
#define TEENSY_35

#elif defined(__MK66FX1M0__)
  Serial.println( "TEENSY 3.6");
#define TEENSY_36
#endif

  Serial.print( "F_CPU = ");   Serial.println( F_CPU );
  Serial.print( "F_PLL = ");   Serial.println( F_PLL );
  Serial.print( "F_BUS = ");   Serial.println( F_BUS );
  Serial.print( "F_MEM = ");   Serial.println( F_MEM );
  Serial.println();
  Serial.print( "NVIC_NUM_INTERRUPTS = ");   Serial.println( NVIC_NUM_INTERRUPTS );
  Serial.print( "DMA_NUM_CHANNELS = ");   Serial.println( DMA_NUM_CHANNELS );
  Serial.print( "CORE_NUM_TOTAL_PINS = ");   Serial.println( CORE_NUM_TOTAL_PINS );
  Serial.print( "CORE_NUM_DIGITAL = ");   Serial.println( CORE_NUM_DIGITAL );
  Serial.print( "CORE_NUM_INTERRUPT = ");   Serial.println( CORE_NUM_INTERRUPT );
  Serial.print( "CORE_NUM_ANALOG = ");   Serial.println( CORE_NUM_ANALOG );
  Serial.print( "CORE_NUM_PWM = ");   Serial.println( CORE_NUM_PWM );
  Serial.print( "ARDUINO = ");   Serial.println( ARDUINO );
  Serial.print( "TEENSYDUINO = ");   Serial.println( TEENSYDUINO );
  Serial.println();

  //#if defined(KINETISK)
  if ((RCM_SRS0 & RCM_SRS0_PIN))
    Serial.println( "RTC hardware Present");
  //#endif
}
