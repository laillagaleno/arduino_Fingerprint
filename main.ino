//Baseado no exemplo original da biblioteca Adafruit Fingerprint

#include <Adafruit_Fingerprint.h> //biblioteca
#include <PushButton.h>

//PINAGEM DO PROJETO
#define push_button 13
#define button_level1 12
#define button_level2 11
#define led_green 10
#define led_red 9
#define rele_int1 5
#define rele_int2 3

//INSTANCIANDO OBJETO 
SoftwareSerial mySerial(3, 2); //pinos(3,2) do sensor no projeto
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); //criando objeto sensor com a biblioteca 

//DECLARAÇÃO DAS VARIAVEIS E FUNÇÕES
uint8_t id;  //Variável responsável pelo armazenamento da digital(entre 1 e 127)
bool admin_Gravar=false; // Indicador booleano para permitir que o sensor verifique se é administrador e gravar uma digital 
int status_blevel1 = 0;
int status_blevel2 = 0;
int idValue = 0;

uint8_t getFingerprintEnroll(uint8_t IDgravar);

void setup()
{
  Serial.begin(9600);
  Serial.println("\n\nModulo Leitor de Impressao Digital - Leitura");

  pinMode(rele_int1, OUTPUT); //configura o pino digital 3 como saída
  pinMode(rele_int2, OUTPUT); //configura o pino digital 5 como saída
  pinMode(led_green, OUTPUT); // configura o pino digital 9 como saída
  pinMode(led_red, OUTPUT); // configura o pino digital 8 como saída
  pinMode(push_button, INPUT_PULLUP); // define o pino do botao como entrada
  pinMode(button_level1, INPUT_PULLUP); // define o pino do botao como entrada
  pinMode(button_level2, INPUT_PULLUP); // define o pino do botao como entrada

  digitalWrite(rele_int1, LOW); //rele inicia desligado
  digitalWrite(rele_int2, LOW); //rele inicia desligado
  digitalWrite(led_green, LOW); //led inicia desligada
  digitalWrite(led_red, LOW); //led inicia desligada

  startSensor(); //varifica o estado do sensor
}

void loop(){
  if (digitalRead(push_button) == LOW){ // Se o botão for pressionado
    admin_Gravar = true; // troca o estado da variavel
  }
  
  if(admin_Gravar){
    id = finger.templateCount + 1;
    Serial.println(id); 
    getFingerprintEnroll(id);//chama função para gravar nova digital
    admin_Gravar = false; //ao finalizar a gravação, colocar admin como false
  }

  
  idValue = getFingerprintIDez(); //chama a função de ler digital
  if (idValue > 0) {
    startRele();
  }
  idValue = 0;


  status_blevel1 = digitalRead(button_level1);
  status_blevel2 = digitalRead(button_level1);
   
   if(status_blevel1 != 0){
      digitalWrite(led_green, LOW);
      digitalWrite(rele_int1, LOW);
      digitalWrite(rele_int2, LOW);
   }
   
   if(status_blevel2 != 0){
      digitalWrite(led_green, LOW);
      digitalWrite(rele_int1, LOW);
      digitalWrite(rele_int2, LOW);
   }
   
 
}

// #################### Função de ler impressão digital ########################## ###
//Retorna -1 em caso de falha, senao retorna o numero do ID
int getFingerprintIDez(){
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  Serial.print("ID encontrado #"); Serial.print(finger.fingerID);
  Serial.print(" com confianca de "); Serial.println(finger.confidence);
  
  return finger.fingerID;
}


void startRele(){
     //Aciona o led e o servo motor
    digitalWrite(led_green, HIGH);
    digitalWrite(rele_int1, HIGH);
    digitalWrite(rele_int2, HIGH);
}


void startSensor(){
 //COMUNICAÇÃO COM O MODULO SENSOR
  finger.begin(57600); // O sensor normalmente trabalha neste valor

  if (finger.verifyPassword()) {
    Serial.println("Sensor de impressao digital detectado!");
    digitalWrite(led_green, HIGH);
    delay(1000);
    digitalWrite(led_green, LOW);
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




// #################### Função de registro de impressão digital ##########################
uint8_t getFingerprintEnroll(uint8_t IDgravar) {
 
  int p = -1;
  Serial.print("Aguardando o dedo válido se inscrever como #");
  Serial.println(IDgravar);
  delay(2000);
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.clear();
      Serial.print(" concluído");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.clear();
      Serial.print("Erro");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.print("Erro leitura");
      break;
    default:
      Serial.print("ERROR");
      break;
    }
  }
 
  // Se não entrou em algum dos erros, todos os processos estão OK ! 
 
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.print("tente novamente ");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.print("ERROR");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.print("Endereço invalido");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.print("Imagem invalida");
      return p;
    default:
      Serial.print("ERRO");
      return p;
  }
 
  Serial.print("Retire o Dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.print(IDgravar);
  p = -1;
  Serial.print("  Insira o dedo ");
  Serial.print("  novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Concluido ! ");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.print("Erro no pacote");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.print("Erro na imagem");
      break;
    default:
      Serial.print("Erro ");
      break;
    }
  }
 
  // Se não entrou em algum dos erros, o primeiro processo está OK ! 
 
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
    Serial.print("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
     Serial.print("Má leitura");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
    Serial.print("Erro- comunicacao");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.print("Endereco invalido");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.print("imagem invalida");
      return p;
    default:
   Serial.print("Erro ");
      return p;
  }
  
  // Se não entrou em algum dos erros, todos os processos estão OK ! 
  Serial.print("Novo ID para #"); Serial.print(IDgravar);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
  Serial.print("As digitais batem!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.print("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.print("As digitais não batem");
    return p;
  } else {
    Serial.print("Erro desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.print(IDgravar);
  p = finger.storeModel(IDgravar);
  if (p == FINGERPRINT_OK) {
   Serial.print("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.print("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.print("Erro na memória");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.print("Erro-memória flash");
    return p;
  } else {
    Serial.print("Erro desconhecido");
    return p;
  }   
}

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Encontrada digital correspondente!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID encontrado #"); Serial.print(finger.fingerID);
  Serial.print(" com confianca de "); Serial.println(finger.confidence);
  return finger.fingerID;
}
