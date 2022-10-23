
// INCLUSÃO DAS BIBLIOTECAS
#include <Adafruit_Fingerprint.h>

// DEFINIÇÃO DO PINO
#define botao 13
#define rele_int1 4
#define rele_int2 5
#define led_green 10
#define led_red 9


// INSTANCIANDO OBJETOS
SoftwareSerial mySerial(3, 2);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// DECLARAÇÃO DAS VARIÁVEIS E FUNCOES
uint8_t numID = 1;
bool gravar=false;
bool estadoBotao = 0;
int idValue = 0;
uint8_t modoGravacaoID(uint8_t IDgravar);

void setup() {
 
  Serial.begin(9600);
  finger.begin(57600);
  
  pinMode(botao,INPUT);
  pinMode(rele_int1, OUTPUT); //configura o pino digital 3 como saída
  pinMode(rele_int2, OUTPUT); //configura o pino digital 5 como saída
  pinMode(led_green, OUTPUT); // configura o pino digital 9 como saída
  pinMode(led_red, OUTPUT); // configura o pino digital 8 como saída
  
  digitalWrite(rele_int1, HIGH); //rele inicia desligado
  digitalWrite(rele_int2, HIGH); //rele inicia desligado
  digitalWrite(led_green, LOW); //led inicia desligada
  digitalWrite(led_red, LOW); //led inicia desligada


  
 startSensor(); //varifica o estado do sensor
}

void loop() {


    idValue = getFingerprintIDez();
    estadoBotao = digitalRead(botao);
    if(estadoBotao == HIGH){
      gravar = true;
    }
    if(gravar){
      Serial.println("Modo Administrador");
      numID = finger.templateCount + 1;
      Serial.println(numID); 
      modoGravacaoID(numID);//chama função para gravar nova digital
      gravar= false;
    }

    if (idValue >= 0) {
      startRele();
    }
    idValue = -1; 
}
// #################### Função de inicializar o sensor ########################## ###
void startSensor(){
 //COMUNICAÇÃO COM O MODULO SENSOR
   finger.begin(57600); // O sensor normalmente trabalha neste valor

  if (finger.verifyPassword()) {
    Serial.println("Sensor de impressao digital detectado!");
    digitalWrite(led_green, HIGH);
    delay(1000);
    digitalWrite(led_green, LOW);
    printStoredFingerprintsCount();
  
  } else {
    Serial.println("Sensor de impressao digital nao encontrado:(");
    digitalWrite(led_red, HIGH);
    delay(1000);
    digitalWrite(led_red, LOW);

    while (true) {
      delay(1);
    }
  }
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);

  int p = -1;
  Serial.print("Esperando uma leitura válida para gravar #"); 
  Serial.println(IDgravar);  
  delay(2000);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      digitalWrite(led_red, LOW);      
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro comunicação");
      digitalWrite(led_green, LOW);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de leitura");
      digitalWrite(led_green, LOW);
      break;
    default:
      Serial.println("Erro desconhecido");
      digitalWrite(led_green, LOW);
      break;
    }
  }

  // OK successo!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
       digitalWrite(led_green, HIGH);
      digitalWrite(led_red, LOW);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
       digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      return p;
    default:
      Serial.println("Erro desconhecido");
       digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      return p;
  }
  
  delay(900);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);

  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); 
  Serial.println(IDgravar);
  p = -1;
  Serial.println("Coloque o Mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      digitalWrite(led_red, LOW);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      digitalWrite(led_green, LOW);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Leitura");
      digitalWrite(led_green, LOW);
      break;
    default:
      Serial.println("Erro desconhecido");
      digitalWrite(led_green, LOW);
      break;
    }
  }

  // OK successo!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      return p;
    default:
      Serial.println("Erro desconhecido");
      digitalWrite(led_red, HIGH);
      digitalWrite(led_green, LOW);
      return p;
  }
  

  delay(900);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);

  // OK convertido!
  Serial.print("Criando modelo para #");  
  Serial.println(IDgravar);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("As digitais batem!");
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, HIGH);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, LOW);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("As digitais não batem");
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, LOW);
    return p;
  } else {
    Serial.println("Erro desconhecido");
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, LOW);
    return p;
  }   
  
 

  
  Serial.print("ID "); Serial.println(IDgravar);
  p = finger.storeModel(IDgravar);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
    digitalWrite(led_green, HIGH);

  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    digitalWrite(led_red, HIGH);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não foi possível gravar neste local da memória");
    digitalWrite(led_red, HIGH); 
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro durante escrita na memória flash");
    digitalWrite(led_red, HIGH);
    return p;
  } else {
    Serial.println("Erro desconhecido");
    digitalWrite(led_red, HIGH);
    return p;
  }   


  delay(1000);
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, LOW);  
}

int getFingerprintIDez() {

  uint8_t p = finger.getImage();
  
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK){
    //Se chegou aqui deu erro, então abortamos os próximos passos
    Serial.println(F("Erro imagem"));
    digitalWrite(led_red, HIGH);
    delay(1000);
    digitalWrite(led_red, LOW);
    return -1;
  }


  p = finger.fingerFastSearch();
  //Procura por este padrão no banco de digitais
  if (p != FINGERPRINT_OK)  {
    //Se chegou aqui significa que a digital não foi encontrada
    Serial.println(F("Digital não encontrada"));
     digitalWrite(led_red, HIGH);
    delay(1000);
    digitalWrite(led_red, LOW);
    return -1;
  }

    Serial.print("ID encontrado #"); 
    Serial.print(finger.fingerID); 
    Serial.print(" com confiança de "); 
    Serial.println(finger.confidence);
    delay(500);
    return finger.fingerID;  
}

// #################### Função de acionar rele ########################## ###
void startRele(){
     //Aciona o led e o rele
    digitalWrite(led_green,HIGH);
    digitalWrite(rele_int1, LOW);
    digitalWrite(rele_int2, LOW);
    delay(10000);
    digitalWrite(led_green, LOW);
    digitalWrite(rele_int1, HIGH);
    digitalWrite(rele_int2, HIGH);
}


void printStoredFingerprintsCount(){
  //Manda o sensor colocar em "templateCount" a quantidade de digitais salvas
  finger.getTemplateCount();

  //Exibe a quantidade salva
  Serial.print(F("Digitais cadastradas: "));
  Serial.println(finger.templateCount);
}
