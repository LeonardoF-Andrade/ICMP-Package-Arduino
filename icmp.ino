#define CS 10
#define mosi 11
#define miso 12
#define SCK 13

#define MIREGADR 0x14
#define MIWRL 0x16
#define MIWRH 0x17
#define Bank3 0x03
#define Bank2 0x02
#define Bank1 0x01
#define Bank0 0x00
#define MACON1 0x00  //banco 2
#define MACON3 0x02  //banco 2
#define MACON4 0x03  //banco 2
#define econ1 0x1F
#define MAMXFLL 0x0A  //banco 2
#define MAMXFLH 0x0B  //banco2
#define MABBIPG 0x04  //bacon 2
#define MAIPGL 0x06   //banco 2
#define MAADR1 0x04   //banco 3
#define MAADR2 0x05   //banco 3
#define MAADR3 0x02   //banco 3
#define MAADR4 0x03   //banco 3
#define MAADR5 0x00   //banco 3
#define MAADR6 0x01   //banco 3g
#define ETXSTL 0x04  //banco 0
#define ETXSTH 0x05  //banco 0
#define ETXNDL 0x06  //banco 0
#define ETXNDH 0x07  //banco 0
#define PHCON1 0X00
#define EIR 0x1C
#define EIE 0x1B
#define ESTAT 0x1D
#define econ2 0x1E
#define EWRPTL 0x02
#define EWRPTH 0x03
#define ERXSTL 0x08    // banco 0
#define ERXSTH 0x09    //bacon 0
#define ERXNDL 0x0A    //bacon 0
#define ERXNDH 0x0B    //bacon 0
#define ERXRDPTL 0x0C  //banco 0
#define ERXRDPTH 0x0D  //banco 0
#define ERXFCON 0x18   //banco 1
#define RBM 0x3A
#define ERDPTL 0x00    //bacon 0
#define ERDPTH 0x01    //bacon 0
#define EPKTCNT 0x19   //banco 1
#define ERXWRPTL 0x0E  // bacno 0
#define ERXWRPTH 0x0F  // bacno 0
#define PHCON2 0X10 //banco 2

unsigned int LugarPac = 0x0000;
int tamanho;
unsigned char *respostaF;


struct arp {
  unsigned char ethernetDest[6];
  unsigned char ethernetSrc[6];
  unsigned char FrameT[2];

  unsigned char HardT[2];
  unsigned char ProtT[2];
  unsigned char HardS[1];
  unsigned char ProtS[1];
  unsigned char Op[2];
  unsigned char SenderMac[6];
  unsigned char SenderIp[4];
  unsigned char TargetMac[6];
  unsigned char TargetIp[4];
} ;
 struct arp *datagram;

struct icmp {
  unsigned char MacDest[6];
  unsigned char MacSrc[6];
  unsigned char EthType[2];
  unsigned char IHL[1];
  unsigned char TypeS[1];
  unsigned char Tamn[2];
  unsigned char id[2];
  unsigned char Flag[2];
  unsigned char TLL[1];
  unsigned char ICMPT[1];
  unsigned char Check[2];
  unsigned char IpSr[4];
  unsigned char IpDe[4];
  unsigned char Typem[1];
  unsigned char cod[1];
  unsigned char checksum[2];
  unsigned char msg[36];

};
struct icmp *Resol;


unsigned char ler(unsigned char argument) {
  unsigned char opcode = 0x00;

  digitalWrite(CS, LOW);

  SPDR = opcode | argument;
  while (!(SPSR & (1 << SPIF)));

  SPDR = 0x00;
  while (!(SPSR & (1 << SPIF)));

  digitalWrite(CS, HIGH);

  return SPDR;
}

unsigned char lerB() {
  unsigned char opcode = 0x3A;
  digitalWrite(CS, LOW);
  SPDR = opcode;
  while (!(SPSR & (1 << SPIF)))
    ;

  SPDR = 0x00;
  while (!(SPSR & (1 << SPIF)))
    ;
  digitalWrite(CS, HIGH);
  return SPDR;
}

void escrever(unsigned char data, unsigned char argument) {
  //datasheet pag 26
  unsigned char opcode = 0x40;
  digitalWrite(CS, LOW);
  SPDR = opcode | argument;
  for(int i = 1; i < 3; i++){
    if (i == 2){
      SPDR = data;
    }
    while (!(SPSR & (1 << SPIF)));
  }
  digitalWrite(CS, HIGH);
}

void banco(unsigned char bank, unsigned char argument) {

  digitalWrite(CS, LOW);
  SPDR = 0XA0 | argument;
  while (!(SPSR & (1 << SPIF)));
  SPDR = 0x03;
  while (!(SPSR & (1 << SPIF)));
  digitalWrite(CS, HIGH);

  //datasheet pag 26
  unsigned char opcode = 0x80;
  digitalWrite(CS, LOW);
  SPDR = opcode | argument;
  for(int i = 1; i < 3; i++){
    if (i == 2){
      SPDR = bank;
    }
    while (!(SPSR & (1 << SPIF)));

  }
  digitalWrite(CS, HIGH);
}

void escreverBit(unsigned char data, unsigned char argument) {
  //datasheet pag 26
  unsigned char opcode = 0x80;
  digitalWrite(CS, LOW);
  SPDR = opcode | argument;
  for(int i = 1; i < 3; i++){
    if (i == 2){
      SPDR = data;
    }
    while (!(SPSR & (1 << SPIF)));

  }

  digitalWrite(CS, HIGH);
}

////////////////////////////////////////////PHCON1 PDPXMD, precisa setar para ativar full duplex/////////////////////////////////////////////////////////
void confiPH() {
  banco(Bank2, econ1);
  escrever(PHCON1, MIREGADR);
  char dadoA = 0x01;   //0b0000001
  char dadodB = 0x00;  //0b0000000
  escrever(dadodB, MIWRL);
  escrever(dadoA, MIWRH);
  //Serial.println("OI");

  escrever(PHCON2, MIREGADR);
  char dadoC = 0x01;   //0b00000001
  char dadodD = 0x00;  //0b00000000
  escrever(dadodD, MIWRL);
  escrever(dadoC, MIWRH);

}

void configMac() {
  banco(Bank2, econ1);
  char datamac = 0x0D;  //0b00001111;
  escrever(datamac, MACON1);

  datamac = 0x11;  //0b00010001
  escrever(datamac, MACON3);

  datamac = 0x40;  //0b01000000
  escrever(datamac, MACON4);

  //Para o tamanho maximo do pacote de 1518bytes - 0x05EE
  char MSB = 0x05;
  char LSB = 0xEE;
  escrever(LSB, MAMXFLL);
  escrever(MSB, MAMXFLH);

  datamac = 0x15;  //padrao para Full duplex
  escrever(datamac, MABBIPG);
  datamac = 0x12;  //padrao
  escrever(datamac, MAIPGL);

  /////////////Endereço MAC - 02:00:00:00:00:01/////////////
  banco(Bank3, econ1);
  datamac = 0x02;
  escrever(datamac, MAADR1);
  datamac = 0x00;
  escrever(datamac, MAADR2);
  datamac = 0x00;
  escrever(datamac, MAADR3);
  datamac = 0x00;
  escrever(datamac, MAADR4);
  datamac = 0x00;
  escrever(datamac, MAADR5);
  datamac = 0x01;
  escrever(datamac, MAADR6);
}

void buffer() {

  banco(Bank0, econ1);
  char data = 0x00;
  escrever(data, ERXSTL);
  escrever(data, ERXSTH);
  escrever(data, ERXRDPTL);
  escrever(data, ERXRDPTH);

  data = 0x00;
  escrever(data, ERXNDL);
  data = 0x1C;
  escrever(data, ERXNDH);

  data = 0x01;
  escrever(data, ETXSTL);
  data = 0x1C;
  escrever(data, ETXSTH);
  data = 0x2B;
  escrever(data, ETXNDL);
  data = 0X1C;
  escrever(data, ETXNDH);

  banco(Bank1, econ1);
  escrever(0x81, ERXFCON);

}


void icmpp(int tamnh){
  Resol = (struct icmp *)respostaF;
  //Serial.println("oi");
  unsigned char aux1[6];
  unsigned char aux2[6];
  int i;
 // Serial.println("Aux");
  for (i = 0; i < 6; i++){
    aux1[i] = Resol->MacDest[i];
    aux2[i] = Resol->MacSrc[i];
  }
 // Serial.println("");
  for (i = 0; i < 6; i++){
    Resol->MacDest[i] = aux2[i];
    Resol->MacSrc[i] = aux1[i];
  }
  unsigned char aux3[4] ;
  unsigned char aux4[4] ;
  for (i = 0; i < 4; i++){
    aux3[i] = Resol->IpDe[i];
    aux4[i] = Resol->IpSr[i];
  }
    for (i = 0; i < 4; i++){
    Resol->IpDe[i] = aux4[i];
    Resol->IpSr[i] = aux3[i];
  }
  Resol->Typem[0] = 0x00;

  unsigned char *aux = Resol->checksum;

  if (aux[0] > (0xff - 0x08)) {
    aux[1]++;
  }
  aux[0] += 0x08 ;

  transmit(respostaF, 1, tamnh);
}


void criaA(int taman){
 // Serial.println("Oi");
  datagram = (struct arp *)respostaF;
  unsigned char aux1[6];
  unsigned char aux2[6];
  int i;
  for (i = 0; i < 6; i++){
    aux2[i] = datagram->ethernetSrc[i];
  }
  for (i = 0; i < 6; i++){
    datagram->ethernetDest[i] = aux2[i];
    datagram->TargetMac[i] = aux2[i];
  }
  datagram->ethernetSrc[0] = datagram->SenderMac[0] = 0x02;
  datagram->ethernetSrc[1] = datagram->SenderMac[1]= 0x00;
  datagram->ethernetSrc[2] = datagram->SenderMac[2]= 0x00;
  datagram->ethernetSrc[3] = datagram->SenderMac[3]= 0x00;
  datagram->ethernetSrc[4] = datagram->SenderMac[4]= 0x00;
  datagram->ethernetSrc[5] = datagram->SenderMac[5]= 0x01;

  datagram->SenderIp[0] = 0xC0;
  datagram->SenderIp[1] = 0xA8;
  datagram->SenderIp[2] = 0x64;
  datagram->SenderIp[3] = 0x02;

  datagram->TargetIp[0] = 0xC0;
  datagram->TargetIp[1] = 0xA8;
  datagram->TargetIp[2] = 0x64;
  datagram->TargetIp[3] = 0xC8;

  datagram->Op[1] = 0x02;

  // for(i = 0; i<42; i++){
  //   Serial.print(" ");
  //   Serial.print(respostaF[i], HEX);
  // }

  transmit(respostaF, 0 , taman);
}


void transmit(unsigned char *pac, int teste, int tam){
  banco(Bank0, econ1);
  if(teste == 1){
  unsigned char data = 0x4B;
  escrever(data, ETXNDL);
  data = 0X1C;
  escrever(data, ETXNDH);

  }
  if (teste == 0){
      unsigned char data = 0x2B;
  escrever(data, ETXNDL);
  data = 0X1C;
  escrever(data, ETXNDH);
  }


  char data = 0x01;
  escrever(data, EWRPTL);
  data = 0x1C;
  escrever(data, EWRPTH);

  digitalWrite(CS, LOW);
  char WBW = 0x7A;
  SPDR = WBW;
  while(!(SPSR & (1<<SPIF)));
  SPDR = 0x00;//datasheet pag 31
  while(!(SPSR & (1<<SPIF)));

 //Serial.println(tam);
  for (int i = 0; i < tam; i++){
    digitalWrite(SCK, LOW);
    SPDR = pac[i];
    while(!(SPSR & (1<<SPIF)));
    digitalWrite(SCK, HIGH);
  }

  digitalWrite(CS, HIGH);
  char txrts = 0x08; //Inicar transmissão
  escreverBit(txrts, econ1);

}


unsigned char* recebe(unsigned char *resposta) {
  //struct arp resposta;
  banco(Bank0, econ1);
  escrever(LugarPac, ERDPTL);
  escrever((LugarPac >> 8), ERDPTH);

  LugarPac = lerB();
  LugarPac |= (lerB() << 8);

  tamanho = lerB();
  tamanho |= lerB() << 8;
  tamanho -= 4;

  int verifica = lerB();
  verifica |= lerB() << 8;

  free(resposta);
  resposta = calloc(tamanho + 1, sizeof(unsigned char));
  digitalWrite(CS, LOW);
  SPDR = 0x3A;
  while (!(SPSR & (1 << SPIF)));

  for (int i = 0; i < tamanho; i++) {
    digitalWrite(SCK, LOW);
    SPDR = 0x00;
    while (!(SPSR & (1 << SPIF)));
    resposta[i] = SPDR;
    //Serial.print(" ");
   // Serial.print(resposta[i], HEX);
    digitalWrite(SCK, HIGH);
  }
  digitalWrite(CS, HIGH);
  banco(Bank0, econ1);
  escrever(LugarPac, ERXRDPTL);
  escrever((LugarPac >> 8), ERXRDPTH);
  escreverBit(0x40, econ2);
  respostaF = resposta;
  if (respostaF[12] == 0x08 & respostaF[13] == 0x00 & respostaF[23] == 0x01) {
    icmpp(tamanho);
    return respostaF;
  }
  else if (respostaF[12] == 0x08 & respostaF[13] == 0x06) {
    criaA(tamanho);
    return respostaF;
  }

}


void setup() {
  //Datasheet, para ser o master, se fosse escravo seria o contrario
  delay(10);

  Serial.begin(9600);
  pinMode(CS, OUTPUT);
  pinMode(mosi, OUTPUT);
  pinMode(miso, INPUT);
  pinMode(SCK, OUTPUT);


  //datasheet pag.140 - SPCR SPI control register
  SPCR = 0x50;               //bit de interupção - 0, bit do spi ativado, bit 5 em 0 para envia o MSB primeiro, bit 4 em 1 para definir como mestre
  digitalWrite(CS, HIGH);  //Para deixar a comunicação fechada
  escreverBit(0x80, econ2);  //0b10000000

  buffer();
  configMac();
  confiPH();

  banco(Bank0, econ1);
  char data = 0x04;
  escreverBit(data, econ1);  // inicia recepção
  // transmit();
  // delay(10);

}

void loop() {
  banco(Bank1, econ1);
  if (ler(EPKTCNT) > 0) {
    recebe(respostaF);
  }
}
