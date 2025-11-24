/*
   ================================
   Ligações do RFID RC522 com Arduino UNO
   ================================

   RC522       →     Arduino UNO
   --------------------------------
   SDA (SS)    →     D10
   SCK         →     D13
   MOSI        →     D11
   MISO        →     D12
   RST         →     D9
   3.3V        →     3.3V   (NÃO usar 5V!)
   GND         →     GND
   IRQ         →     (não usar)
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>


#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

// --Definindo pinos de leds
#define LEN_KEY_0 2
#define LEN_KEY_1 3
#define LEN_KEY_2 4

// Butao de reset
#define BUTTON_PIN 7

// Vetores contendo strings (char*)
#define LEN_KEY 3
const char* Keys_original[LEN_KEY] = {"B5F821", "99FC77A","BDF6CECE"};
char* Keys_not_used[LEN_KEY] = {"B5F821", "99FC77A","BDF6CECE"};
char* Keys_used[LEN_KEY]     = {NULL, NULL};


void moveKey(const char* key); //funcao de mover chaves

void resetKeys(); //reseta keys

void printKeys(); // Imprimir no serial estado dos vetores

void update_leds();

void applayKeys(int idx);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Aproxime um cartão...");
  printKeys();

  //Definidno leds
  pinMode(LEN_KEY_0, OUTPUT);
  pinMode(LEN_KEY_1, OUTPUT);
  pinMode(LEN_KEY_2, OUTPUT);

  //Inicializando teclado
  Keyboard.begin();

}

unsigned long lastPrint = 0;

void loop() {

  update_leds();

  // Imprime o estado das chaves a cada 2 segundos
  if (millis() - lastPrint >= 2000) {
    printKeys();
    lastPrint = millis();
  }

  // Se nenhum cartão estiver presente, sai
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    return;

  // Lê UID do cartão e transforma em string HEX
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();

  Serial.print("Detectado UID: ");
  Serial.println(uidString);

  // Compara com todas as chaves
  for (int i = 0; i < LEN_KEY; i++) {
    if (Keys_not_used[i] != NULL && uidString == Keys_not_used[i]) {
      Serial.println("Chave encontrada! Movendo para 'used'");
      moveKey(Keys_not_used[i]);
      break;
    }
  }

  mfrc522.PICC_HaltA();

}

void applayKeys(int idx){
  if(Keys_used[idx]!=NULL) Keyboard.write(idx+'1');
}



// ---------- Função para mover chave ----------
void moveKey(const char* key) {
  for (int i = 0; i < LEN_KEY; i++) {
    if (Keys_not_used[i] != NULL && strcmp(Keys_not_used[i], key) == 0) {
      Keys_used[i] = Keys_not_used[i]; // mesma posição
      Keys_not_used[i] = NULL;
      applayKeys(i);
      break;
    }
  }
}

// ------- Função para reseta as chaves

void resetKeys() {
  for(int i =0 ; i<LEN_KEY ;i++){
    Keys_used[i]=NULL;
    Keys_not_used[i]=Keys_original[i];
  }
  Serial.println("Vetores de chaves resetadas");
}

// --Funcao para atualiza estado de leds

void update_leds(){
  for(int i=0; i<LEN_KEY; i++){
    int pin=i+2;
    if(Keys_not_used[i]!=NULL) digitalWrite(pin, HIGH);
    else digitalWrite(pin, LOW);
  }
}

// ---------- Imprime o estado dos vetores ----------
void printKeys() {
  Serial.println("------ STATUS DAS CHAVES ------");

  Serial.print("Not used: ");
  for (int i = 0; i < LEN_KEY; i++) {
    if (Keys_not_used[i] == NULL) Serial.print("[ ] ");
    else Serial.print(Keys_not_used[i]), Serial.print(" ");
  }
  Serial.println();

  Serial.print("Used:     ");
  for (int i = 0; i < LEN_KEY; i++) {
    if (Keys_used[i] == NULL) Serial.print("[ ] ");
    else Serial.print(Keys_used[i]), Serial.print(" ");
  }
  Serial.println();
  Serial.println("--------------------------------");
}