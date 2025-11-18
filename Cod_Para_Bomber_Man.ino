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

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

// --Definindo pinos de leds
#define LEN_KEY_0 0
#define LEN_KEY_1 1
#define LEN_KEY_2 2

// Butao de reset
#define BUTTON_PIN 7

// Vetores contendo strings (char*)
#define LEN_KEY 2
const char* Keys_original[LEN_KEY] = {"B5F821", "72EF80"};
char* Keys_not_used[LEN_KEY] = {"B5F821", "72EF80"};
char* Keys_used[LEN_KEY]     = {NULL, NULL};


void moveKey(const char* key); //funcao de mover chaves

void resetKeys(); //reseta keys

void printKeys(); // Imprimir no serial estado dos vetores

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Aproxime um cartão...");
  printKeys();

  //Definidno leds
  pinMode(LEN_KEY_0, OUTPUT);
  pinMode(LEN_KEY_1, OUTPUT);

}

unsigned long lastPrint = 0;

void loop() {
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





// ---------- Função para mover chave ----------
void moveKey(const char* key) {
  for (int i = 0; i < LEN_KEY; i++) {
    if (Keys_not_used[i] != NULL && strcmp(Keys_not_used[i], key) == 0) {
      Keys_used[i] = Keys_not_used[i]; // mesma posição
      Keys_not_used[i] = NULL;
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