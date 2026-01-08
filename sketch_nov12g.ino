#include <SD.h>
#include <SPI.h>

const int chipSelect = 10;         
const int ledPin = 7;             
const unsigned long logInterval = 600000UL;   
const unsigned long fileInterval = 86400000UL;  


unsigned long lastLog = 0;
unsigned long fileStartTime = 0; 
File dataFile;
int fileIndex = 0;

const int SMOOTH_N = 10;
int buffer[6][SMOOTH_N];
int bufIndex = 0;


void newFile() {
  if (dataFile) dataFile.close();

  char filename[20];
  sprintf(filename, "LOG_%03d.TXT", fileIndex);
  fileIndex++;

  dataFile = SD.open(filename, FILE_WRITE);


  if (dataFile) {
    if (Serial) {
      Serial.print("Created new file: ");
      Serial.println(filename);
    }

    dataFile.println("timestamp_ms, A0, A1, A2, A3, A4, A5");
    dataFile.flush();

    digitalWrite(ledPin, HIGH); delay(200);
    digitalWrite(ledPin, LOW);
  } else {
    if (Serial) {
      Serial.print("ERROR creating file: ");
      Serial.println(filename);
    }

    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH); delay(120);
      digitalWrite(ledPin, LOW); delay(120);
    }
  }

  // Reset the file timer AND timestamp zero
  fileStartTime = millis();
  lastLog = 0;  
}


int smooth(int channel) {
  long sum = 0;
  for (int i = 0; i < SMOOTH_N; i++) {
    sum += buffer[channel][i];
  }
  return sum / SMOOTH_N;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);


  for (int i = 0; i < 2; i++) {
    digitalWrite(ledPin, HIGH); delay(150);
    digitalWrite(ledPin, LOW); delay(150);
  }

  Serial.begin(9600);
  delay(500);

  if (Serial) Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    if (Serial) Serial.println("SD FAIL — STOPPING.");
    digitalWrite(ledPin, HIGH); delay(1000);
    digitalWrite(ledPin, LOW);
    while (1); // stop
  }

  if (Serial) Serial.println("SD OK!");


  digitalWrite(ledPin, HIGH); delay(400);
  digitalWrite(ledPin, LOW);

  
  newFile();
}


void loop() {
  unsigned long now = millis();


  if (now - fileStartTime >= fileInterval) {
    newFile();
  }


  int raw[6];
  raw[0] = analogRead(A0);
  raw[1] = analogRead(A1);
  raw[2] = analogRead(A2);
  raw[3] = analogRead(A3);
  raw[4] = analogRead(A4);
  raw[5] = analogRead(A5);

  for (int ch = 0; ch < 6; ch++) {
    buffer[ch][bufIndex] = raw[ch];
  }
  bufIndex = (bufIndex + 1) % SMOOTH_N;

  if (now - lastLog >= logInterval) {

    unsigned long t = now - fileStartTime;

    int a0 = smooth(0);
    int a1 = smooth(1);
    int a2 = smooth(2);
    int a3 = smooth(3);
    int a4 = smooth(4);
    int a5 = smooth(5);

    char line[100];
    sprintf(line, "%lu, %d, %d, %d, %d, %d, %d",
            t, a0, a1, a2, a3, a4, a5);

    if (dataFile) {
      dataFile.println(line);
      dataFile.flush();
    }


    if (Serial) Serial.println(line);


    digitalWrite(ledPin, HIGH); delay(80);
    digitalWrite(ledPin, LOW);

    lastLog = now;
  }
}
