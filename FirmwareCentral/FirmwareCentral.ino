// ----- BIBLIOTECAS BLE -----
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <CAN.h>
// ---------------------------

// ----- CONFIGURAÇÕES DO DISPOSITIVO -----
#define DEVICE_NAME "MVP"
#define TIPS_NUMBER 20
// ----------------------------------------

// ----- UUIDs BLE -----
#define TIPS_CONTROL_SERVICE_UUID "ab0828b1-198e-4351-b779-901fa0e0371e" // Identificador do serviço BLE referente ao controle de bicos
#define TIPS_CONTROL_CHARACTERISTIC_UUID_RX "4ac8a682-9736-4e5d-932b-e9b31405049c"  // Identificador da característica referente ao recebimento de dados referente ao controle de bicos
#define DATA_SERVICE_UUID "aa1bc592-eae6-11eb-9a03-0242ac130003"  // Identificador do serviço BLE referente aos dados dos sensores
#define DATA_CHARACTERISTIC_UUID_RX "b29980ba-eae6-11eb-9a03-0242ac130003"  // Identificador da característica referente ao recebimento de solicitações de dados dos sensores
#define DATA_CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91"  // Identificador da característica referente ao envio de dados dos sensores
// ---------------------

// ----- ENUMERADORES -----
enum sideEnum {
  LEFT = 1,
  RIGHT = 2
};

enum tipsStatusEnum {
  OPEN = 0,
  CLOSED = 1
};
// ------------------------


// ----- VARIÁVEIS DE CONTROLE -----
bool deviceConnected = false;

BLECharacteristic *dataCharacteristicTX;
// ---------------------------------


// ----- FUNÇÕES -----
void sendCANmessage(uint8_t section, uint8_t module, uint8_t *tipsStatus);
uint8_t char2UInt8(char charToConvert);
// -------------------


// ----- CLASSE DO CALLBACK DO SERVIDOR -----
class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("--> Um novo dispositivo foi conectado!");
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      pServer->getAdvertising()->start();
      Serial.println("--> O dispositivo foi desconectado!");
    }
};
// ------------------------------------------



// ----- CLASSE DO CALLBACK DA RECEBIMENTO DE DADOS DO BLUETOOTH REFERENTES AO CONTROLE DE BICOS -----
class TipsControlCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      String rxValue = characteristic->getValue().c_str();

      if (rxValue.length() > 0) {
        char sectionChar = rxValue[0];
        uint8_t sectionUInt = char2UInt8(sectionChar);
        //        Serial.println(sectionUInt);

        if (sectionUInt < 10) {
          uint8_t module = 0;
          for (int i = 1; i < rxValue.length(); i = i + 3) {
            String tipsStatus = rxValue.substring(i, i + 3);
            uint8_t statusToSend[3];

            for (int k = 0; k < 3; k++) {
              if (tipsStatus.substring(k, k + 1) == "0") statusToSend[k] = OPEN;
              else statusToSend[k] = CLOSED;
            }
            sendCANmessage(sectionUInt, module, statusToSend);
            module++;
          }
        } else {
          Serial.println("Os dados recebidos via Bluetooth são inválidos!");
        }
      }
    }
};
// -------------------------------------------------------------------------------------------------



// ----- INICIALIZAÇÃO DOS DISPOSITIVOS -----
void setup() {
  Serial.begin(115200); // Inicializa o monitor Serial

  //  // -- Inicialização do Servidor BLE --
  BLEDevice::init(DEVICE_NAME); // Define o nome do dispositivo
  BLEServer *server = BLEDevice::createServer(); // Instancia o servidor BLE
  server->setCallbacks(new ServerCallbacks()); // Configura o callback do servidor
  // -----------------------------------

  // -- Criação do serviço e característica para o controle dos bicos --
  BLEService *tipsControlService = server->createService(TIPS_CONTROL_SERVICE_UUID); // Cria o serviço do dispositivo BLE para controle dos bicos
  BLECharacteristic *tipsControlCharacteristic = tipsControlService->createCharacteristic( // Cria a característica para o recebimento dos dados
        TIPS_CONTROL_CHARACTERISTIC_UUID_RX, //                 referentes ao controle do bicos
        BLECharacteristic::PROPERTY_WRITE
      );
  tipsControlCharacteristic->setCallbacks(new TipsControlCharacteristicCallbacks()); // Configura o callback para a característica de recebimento de dados de controle de bicos
  tipsControlService->start(); // Inicializa o serviço do BLE do controle dos bicos
  // -------------------------------------------------------------------

  server->getAdvertising()->start(); // Ativa a descoberta do dispositivo BLE

  // -- Iniciando o Serial --
  Serial.print(" ----- EXECUÇÃO INICIADA NO DISPOSITIVO ");
  Serial.print(DEVICE_NAME);
  Serial.println(" ----- ");
  // ------------------------

  Wire.begin(); //Inicia comunicação I2C

  // -- Inicia o módulo CAN --
  if (!CAN.begin(125E3)) {
    Serial.println("Falha ao iniciar o controlador CAN"); //Se não for possível iniciar o controlador
    while (1);
  }
}
// ------------------------------------------



void loop() {}



void sendCANmessage(uint8_t section, uint8_t module, uint8_t *tipsStatus) {
  // Usando o CAN 2.0 padrão
  // Envia um pacote: o id tem 11 bits e identifica a mensagem (prioridade, evento)
  // O bloco de dados deve possuir até 8 bytes

  // Print
  Serial.print("Enviando pacote via CAN para o ");
  Serial.print(module + 1);
  Serial.print("º módulo da ");
  Serial.print(section + 1);
  Serial.print("ª seção da barra: ");
  Serial.print(tipsStatus[0]);
  Serial.print(tipsStatus[1]);
  Serial.println(tipsStatus[2]);

  CAN.beginPacket(0x12); // id 18 em hexadecimal
  CAN.write(section); // identificador da seção
  CAN.write(module); // identificador do módulo
  CAN.write(tipsStatus[0]); // estado do bico da esquerda
  CAN.write(tipsStatus[1]); // estado do bico central
  CAN.write(tipsStatus[2]); // estado do bico da direita
  CAN.endPacket(); // encerra o pacote para envio

  Serial.println("Pacote enviado");
}



uint8_t char2UInt8(char charToConvert) {
  switch (charToConvert) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    default: return 10;
  }
}
