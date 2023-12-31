#include <CAN.h> //Inclui a biblioteca CAN
#include <driver/ledc.h> //Inclui a biblioteca ledc para PWM

// ----- DEFINE OS IDENTIFICADOS DO módulo -----
#define SECTION 0 // Seção ao qual o módulo diz respeito
#define NODE 0 // Identificador do módulo na seção
// -----------------------------------------

// ----- DEFINE PINOS DOS BICOS -----
#define tip2 25
#define tip3 32
// ----------------------------------
//-------Duty Cycle-------
int valor_desejado = 50; // Valor desejado para o duty cycle

int dutyCycle = valor_desejado*(0.80); // Valor desejado para o duty cycle

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

  // Configuração dos pinos para gerar PWM
  ledcSetup(0, 8, 8); // Configura o canal 0 com resolução de 8 bits
  //ledcAttachPin(tip1, 0); // Atribui o canal 0 ao pino tip1
  ledcAttachPin(tip2, 1); // Atribui o canal 1 ao pino tip2
  ledcAttachPin(tip3, 2); // Atribui o canal 2 ao pino tip3

  ledcWrite(1, 1);
  ledcWrite(2, 1);

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
    //if (packet[TIP_ONE] == CLOSED) {
    //  Serial.println("Bico 1 recebeu o comando para ser fechado. Fechando bico 1...");
    //  ledcWrite(0, dutyCycle); // Define o duty cycle como 100% (totalmente fechado)
   // } else {
   //   Serial.println("Bico 1 recebeu o comando para ser aberto. Abrindo bico 1...");
   //   ledcWrite(0, 0); // Define o duty cycle como 0% (totalmente aberto)
   // }

    // Controle bico 2
    if (packet[TIP_TWO] == CLOSED) {
      Serial.println("Bico 2 recebeu o comando para ser fechado. Fechando bico 2...");
      ledcWrite(1, dutyCycle); // Define o duty cycle como 100% (totalmente fechado)
    } else {
      Serial.println("Bico 2 recebeu o comando para ser aberto. Abrindo bico 2...");
      ledcWrite(1, 1); // Define o duty cycle como 0% (totalmente aberto)
    }

    // Controle bico 3
    if (packet[TIP_THREE] == CLOSED) {
      Serial.println("Bico 3 recebeu o comando para ser fechado. Fechando bico 3...");
      ledcWrite(2, 1); // Define o duty cycle como 100% (totalmente fechado)
    } else {
      Serial.println("Bico 3 recebeu o comando para ser aberto. Abrindo bico 3...");
      ledcWrite(2, dutyCycle); // Define o duty cycle como 0% (totalmente aberto)
    }
  }
}