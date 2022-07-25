/**************************************
   utils. h - various support functions

   version 1.1.0   Dec 2019

   Various utility and helper functions


 ***************************************/



// prototypes
void printValue(const char* msg);
void printValue(String, int);
void printValue(String, long);
void printValue(String, float);
void printValue(const char*, int, float);
void printHexValue(const char*, int);
void printArryValue(const char* , uint8_t, float);
void printAudioMemUsage();





// simple helper functions that print data to the serial port
// depending if debugPrint is true
void printValue(const char* msg)
{
  if (debugPrint)
  {
    Serial.print("msg = ");
    Serial.println(msg);
  }
}


void printValue(const char* msg, int val)
{
  if (debugPrint)
  {
    Serial.print(msg);
    Serial.print(" = ");
    Serial.println(val);
  }
}


void printValue(const char* msg, long val)
{
  if (debugPrint)
  {
    Serial.print(msg);
    Serial.print(" = ");
    Serial.println(val);
  }
}


void printValue(const char* msg, float val)
{
  if (debugPrint)
  {
    Serial.print(msg);
    Serial.print(" = ");
    Serial.println(val, 1);
  }
}


void printValue(const char* msg, int val1, float val2)
{
  if (debugPrint)
  {
    Serial.print(msg);
    Serial.print(" ");
    Serial.print(val1);
    Serial.print(" = ");
    Serial.println(val2);
  }
}


void printHexValue(const char* msg, int val)
{
  if (debugPrint)
  {
    Serial.print(msg);
    Serial.print(" = 0x");
    Serial.println(val, HEX);
  }
}




void printArryValue(const char* msg, uint8_t index, float val)
{
  if (debugPrint)
  {
    Serial.print(msg);
    Serial.print("[");
    Serial.print(index);
    Serial.print("] = ");
    Serial.println(val);
  }
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
  Serial.println(F("CPU Usage"));
  Serial.print(F("Freeverb Usage : ")); Serial.println(freeverb1.processorUsage());
  Serial.print(F("Delayer Usage  : ")); Serial.println(delayExt1.processorUsage());

  Serial.print(F("AudioProcessor : ")); Serial.print(AudioProcessorUsage());
  Serial.print(F("   max: "));          Serial.println(AudioProcessorUsageMax());

  Serial.print(F("Audio Mem      : ")); Serial.print(AudioMemoryUsage());
  Serial.print(F("   max: "));      Serial.println(AudioMemoryUsageMax());

  Serial.print(F("Free RAM       : ")); Serial.println(teensyFreeMem());
  Serial.println();
}
