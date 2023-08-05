#include <CAN.h> //Inclui a biblioteca CAN

// ----- DEFINE OS IDENTIFICADOS DO módulo -----
#define SECTION 0 // Seção ao qual o módulo diz respeito
#define NODE 0 // Identificador do módulo na seção
// -----------------------------------------

// ----- DEFINE PINOS DOS BICOS -----
#define tip1 27
#define tip2 25
#define tip3 32
// ----------------------------------


// ----- ENUMS -----
enum packetIterator {
  pSECTION = 0, pNODE = 1, TIP_ONE = 2, TIP_TWO = 3, TIP_THREE = 4
};

enum tipsStatus {
  OPEN = 0, CLOSED = 1
};
// -----------------

// ----- Variáveis de uso geral -----
uint8_t received[5];
// ----------------------------------

// ----- Funções -----
void tipsControl(uint8_t *packet);
// -------------------

void setup() {
  Serial.begin(115200); // Inicia a serial para debug
  while (!Serial);

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(125E3)) {
    Serial.println("Falha ao iniciar o controlador CAN"); // Caso não seja possível iniciar o controlador
    while (1);
  }

  // Definição dos pinos como saída
  pinMode(tip1, OUTPUT);
  pinMode(tip2, OUTPUT);
  pinMode(tip3, OUTPUT);

  // Imprime dados de identificação do módulo
  Serial.println("*** Receptor CAN ***");
  Serial.print("-> ");
  Serial.print(NODE + 1);
  Serial.print("º módulo da ");
  Serial.print(SECTION + 1);
  Serial.println("ª seção da barra.");
}

void loop() {
  // Tenta verificar o tamanho do pacote recebido
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    if (!CAN.packetRtr()) { // Só faz o processamento se não for um pacote de requsição
      Serial.print("Recebido: ");
      int i = 0;
      while (CAN.available()) { // Montagem e impressão do vetor recebido
        received[i] = CAN.read();
        Serial.print(received[i]);
        i++;
      }
      Serial.println();
      tipsControl(received); // Chamada da função de controle dos bicos
    }
  }
}

void tipsControl(uint8_t *packet) {
  if (packet[pNODE] == NODE && packet[pSECTION] == SECTION) {
    // Controle bico 1
    if (packet[TIP_ONE] == CLOSED) {
      Serial.println("Bico 1 recebeu o comando para ser fechado. Fechando bico 1...");
      digitalWrite(tip1, HIGH); // Fecha bico
    } else {
      Serial.println("Bico 1 recebeu o comando para ser aberto. Abrindo bico 1...");
      digitalWrite(tip1, LOW); // Abre bico
    }

    // Controle bico 2
    if (packet[TIP_TWO] == CLOSED) {
      Serial.println("Bico 2 recebeu o comando para ser fechado. Fechando bico 2...");
      digitalWrite(tip2, HIGH); // Fecha bico
    } else {
      Serial.println("Bico 2 recebeu o comando para ser aberto. Abrindo bico 2...");
      digitalWrite(tip2, LOW); // Abre bico
    }

    // Controle bico 3
    if (packet[TIP_THREE] == CLOSED) {
      Serial.println("Bico 3 recebeu o comando para ser fechado. Fechando bico 3...");
      digitalWrite(tip3, HIGH); // Fecha bico
    } else {
      Serial.println("Bico 3 recebeu o comando para ser aberto. Abrindo bico 3...");
      digitalWrite(tip3, LOW); // Abre bico
    }
  }
}
