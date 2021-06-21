 /**************************************************************************************
 * 
 * Name         : simulation for ultrasonic sensor
 * 
 * Version      : 1.0
 * 
 * Author       : Raji
 * 
 * controller   : arduino nano 
 * 
 * project Name :swms regular
 * 
 * components   :ultrasonic sensor,solenoid,NRF,led's
 * 
 * Description : This is the one of the sensor node that works under
 *               Smart Water Management System and is placed at the top of the water tank
 *               attached with ultrasinic sensor and actuator inorder to check the water level
 *               in the tank and send to gateway device.
 *                             
 **************************************************************************************/
 /***** INCLUDES LIBRARIES*******/
#include <SPI.h>                                     
#include <nRF24L01.h>                               
#include <EEPROM.h>
#include <RF24.h>                                 
/************************************/
#define MOTOR 4
#define PULSE_PIN 3
#define WHICH_NODE 6  
#define ERROR_LED 5                    
/*******************************************/
RF24 radio(7, 8);     
/***************************************/
 int motor_status,itration2,add_constant = 0; 
 const int pingPin = 9; // Trigger Pin of Ultrasonic Sensor
 const int echoPin = 10; // Echo Pin of Ultrasonic Sensor
 int this_node,new_node,water_remaining;
 int state,eeprom_status=0;
 int interupt_state=0;
 int flow_status,count;
  float calibrationFactor;
  float flowRate,flow_rate;
  float totalLitres,maximum_flow;
  unsigned int flowMilliLitres;
  unsigned long totalMilliLitres,previous_time;
  volatile long pulseCount;
  unsigned long oldTime;
 char node_data[35],new_number[2];
 char incoming_data[40];
 char dest[40],char_lower_limit[2],char_upper_limit[2];
 const int interruptPin = 2;
 unsigned long previous_millis=0;
 unsigned long manual_millis=0;
 const uint64_t wAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL};
 char* rAddresss[] = {"0x7878787878LL", "0xB3B4B5B6F1LL", "0xB3B4B5B6CDLL", "0xB3B4B5B6A3LL", "0xB3B4B5B60FLL", "0xB3B4B5B605LL" };
 uint64_t PTXpipe = wAddress[ WHICH_NODE - 1 ];    
 String NrfAction1,NrfAction2,NrfAction3,NrfAction4,value_one,value_two,value_three,value_four;
/**************************************************************************************/
/**********************************void declaration******************************************/
void pulseCounter(){
  pulseCount++;
}
void setup(){
 Serial.begin(9600);  
/*******************pin mode******************************/  
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(MOTOR,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(ERROR_LED ,OUTPUT);
   pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
/*********************make pins low********************************/
digitalWrite(ERROR_LED,LOW);
digitalWrite(5,LOW);
digitalWrite(MOTOR,LOW);
/*********************attach interrupt**************************/  
attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE);
/******************************start nrt********************************/
  radio.begin();                                //Start the nRF24 module
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(108);       
/******************************read the node number from eeprom*********************************************/
  EEPROM.write(0,5);
  this_node=EEPROM.read(0);
  Serial.println(this_node); 
  PTXpipe = wAddress[this_node];
  radio.openReadingPipe(0,PTXpipe);        //open reading or receive pipe
  radio.startListening();                  //go into transmit mode
  state=0;
/************************************************************/
   motor_status=EEPROM.read(103);
   digitalWrite(MOTOR, motor_status);
  /**********************************************/

 
}
void loop(){
delay(899);
//===================interrupt detectecd===============//
 if((state==1)&&(this_node==5)&&(interupt_state==0))
   {
    
      Serial.println("Interrupt2 Detected"); 
      digitalWrite(5,HIGH);
      radio.stopListening();
      strcpy(node_data,"");
      strcpy(node_data,"{\"R\":\"8\",\"ID\":\"OTSS111\"}");
      radio.openWritingPipe(PTXpipe);    

     if (!radio.write(&node_data, sizeof(node_data))){  

         //Serial.println(" failed"); 
         digitalWrite(ERROR_LED,HIGH);     
        interupt_state=0;
     }

     else {  

         // Serial.print("Success :");
          digitalWrite(ERROR_LED,LOW);
          add_constant=11;
         
     }

     radio.startListening();
     interupt_state=1;
    
}
  
  
 else{
  while(1)
  {      
 /********************************ultrasonic sensor code***********************************/  
    long duration, inches, cm;
  
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
  
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   Serial.print(inches);
   Serial.print("in, ");
   Serial.print(cm);
   Serial.print("cm");
   Serial.println();
/*******************************send to gateway every 15 mins******************************************/
   unsigned long current_time=millis();
   if(current_time  - previous_time >= 900000){
      previous_time  =  current_time;
      int motor_status  =  EEPROM.read(103);
   /*******************************water should not flow******************************************/
    if(motor_status  !=  1){
       /*******************************rise in water level******************************************/
       
       /*******************************water level is same******************************************/
      
    }
    /*******************************water should flow******************************************/
    if(1  ==  motor_status){
      /*******************************rise in water level******************************************/
      
      /*******************************water level is same******************************************/
    }
    count++;
    if(count == 4){
      /*******************************send to gateway every 45 mins******************************************/
      radio.stopListening();
       String depth=String(cm);
      strcpy(node_data,"");
      strcpy(node_data,"{\"R\":\"13\",\"L\":\"");
      strcpy(node_data,depth.c_str());
      strcpy(node_data,"\"}");
      radio.openWritingPipe(PTXpipe);    

     if (!radio.write(&node_data, sizeof(node_data))){   
         digitalWrite(ERROR_LED,HIGH);     
      
     }

     else {  

          digitalWrite(ERROR_LED,LOW);
        
     }

     radio.startListening();
     count=0;
      
    }
    
   }
   
 /****************************check if the sensor goes 15cm then switch off and send to gateway*********************
 if(cm <=  15){
  delay(899);
  Serial.println("waterfilled"); 
      digitalWrite(MOTOR,LOW);
      radio.stopListening();
      strcpy(node_data,"");
      strcpy(node_data,"{\"G\":\"9\",\"T\":\"1\",\"ID\":\"5\"}");
      radio.openWritingPipe(PTXpipe);    

     if (!radio.write(&node_data, sizeof(node_data))){  

         //Serial.println(" failed"); 
         digitalWrite(ERROR_LED,HIGH);     
        interupt_state=0;
     }

     else {  

         // Serial.print("Success :");
          digitalWrite(ERROR_LED,LOW);
         // Serial.println(node_data);
     }

     radio.startListening();
  
 }
  /****************************check if the sensor reaches threshold then switch on and send to gateway*********************
 if(cm <=  105){
  
  Serial.println("waterLOW"); 
      digitalWrite(MOTOR,HIGH);
      radio.stopListening();
      strcpy(node_data,"");
      strcpy(node_data,"{\"G\":\"9\",\"T\":\"1\",\"ID\":\"5\"}");
      radio.openWritingPipe(PTXpipe);    

     if (!radio.write(&node_data, sizeof(node_data))){  

         //Serial.println(" failed"); 
         digitalWrite(ERROR_LED,HIGH);     
        interupt_state=0;
     }

     else {  

         // Serial.print("Success :");
          digitalWrite(ERROR_LED,LOW);
         // Serial.println(node_data);
     }

     radio.startListening();
  
 }
    int motor_flow_rate=EEPROM.read(104);
    manual_millis=motor_flow_rate*60000;
    int motor_state=EEPROM.read(103);
    /*************************************set internal timer to switch off************************************************
   if(((millis() - previous_millis) > manual_millis)&&(motor_state == 1)){
         
         EEPROM.write(103,0);
        digitalWrite(MOTOR,0);
        int MotorValue = digitalRead(MOTOR);
        String string_motor=String(MotorValue);
        /********************send acknowledgement to gateway*********************
        strcpy(node_data,"");
        strcpy(node_data,"{\"R\":\"6\",\"A\":\"0X6678888\",\"S\":");
        strcpy(node_data,string_motor.c_str());
        strcpy(node_data,"}");
  }**/
/*******************************************NRF RECIVIVER***********************************************/                                             
while ( radio.available() ) { //run while no receive data and not timed out
      Serial.print("Success :radio available");
      radio.read( &incoming_data, sizeof(incoming_data) );
      strncpy(dest,incoming_data, sizeof(incoming_data));
      Serial.println(incoming_data);
      delay(200);  
      NRF_check(dest);
        }
          delay(1000);        
/*******************************************NRF RECIVIVER***********************************************/   
         if(state==1)
          {
            
            break;
          }
    delay(100);
  
 }
  }
   delay(1100);

 } 
 

void NRF_check(char *commands){
/******************************parse json action********************************************/
  Serial.println(commands);

     for(itration2=0;itration2<=strlen(commands);itration2++)
   {
    if(commands[itration2]=='{')
    {
       NrfAction1="";
       for(itration2=itration2;itration2<(strlen(commands));itration2++)
       {
      if(commands[itration2+1]==':')
     {
      break;
     }
    NrfAction1=NrfAction1+commands[itration2+1];
       }
       break;
    }
   }
 for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==':')
    {
      value_one="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if(commands[itration2]==',')
     { 
      break;
     }
  
    value_one=value_one+commands[itration2];

      }
      break;
    }
   }
    for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==',')
    {
      NrfAction2="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if(commands[itration2]==':')
     { 
      break;
     }
  
    NrfAction2=NrfAction2+commands[itration2];

      }
      break;
    }
   }
       for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==':')
    {
      value_two="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if(commands[itration2]==',')
     { 
      break;
     }
  
     value_two= value_two+commands[itration2];

      }
      break;
    }
   }
    for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==',')
    {
      NrfAction3="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if(commands[itration2]==':')
     { 
      break;
     }
  
    NrfAction3=NrfAction3+commands[itration2];

      }
      break;
    }
   }
       for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==':')
    {
      value_three="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if((commands[itration2]==',')||(commands[itration2]=='}'))
     { 
      break;
     }
  
     value_three= value_three+commands[itration2];

      }
      break;
    }
   }
   for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==',')
    {
      NrfAction4="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if(commands[itration2]==':')
     { 
      break;
     }
  
    NrfAction4=NrfAction4+commands[itration2];

      }
      break;
    }
   }
       for(itration2=itration2;itration2<=strlen(commands);itration2++)
   {  
    if(commands[itration2]==':')
    {
      value_four="";
      for(itration2=itration2+1;itration2<(strlen(commands));itration2++)
      {
      if(commands[itration2]=='}')
     { 
      break;
     }
  
      value_four=value_four+commands[itration2];

      }
      break;
    }
   }
       Serial.println("NrfAction1");
    Serial.println(NrfAction1);
    Serial.println("value_one");
     Serial.println(value_one);
     Serial.println("NrfAction2");
     Serial.println(NrfAction2);
          Serial.println("value_two");
     Serial.println(value_two);
        Serial.println("NrfAction3");
         Serial.println(NrfAction3);
          Serial.println("value_three");
     Serial.println(value_three);
   delay(99);

     if((value_one=="900")&&(add_constant == 11))
      {
     
    /**************888********************asssign new node*******************8*********************/
     digitalWrite(5,LOW);
     int change_node= value_two.toInt();
     new_node=0;
     new_node=new_node+change_node;
    /********************write in eeprom*********************/
        EEPROM.write(0,new_node);
   /********************eeprom to indicate the node is changed*********************/
        EEPROM.write(5,1);
  /********************send acknowledgement to gateway*********************/
        radio.stopListening();
        PTXpipe = wAddress[new_node];
        radio.openReadingPipe(0,PTXpipe);
        radio.startListening();
       delay(13000);
       String node_string=String(new_node);
       strcpy(new_number,"");
       strcpy(new_number,node_string.c_str());
  /*******************************start sending to gateway*******************************************/      
       radio.stopListening();
       strcpy(node_data,"");
       strcat(node_data,"{\"R\":\"88\",\"ID\":\"OTSS111\"}");
       
       delay(100);
  
      radio.openWritingPipe(PTXpipe);        
     if (!radio.write(&node_data, sizeof(node_data))){  
         //Serial.println(" failed");      
         digitalWrite(ERROR_LED,HIGH);

     }

     else { 

          //Serial.print("Success :");
          digitalWrite(ERROR_LED,LOW);
        
     }

     radio.startListening();
   
      
   add_constant=12;
      }
       
 /***********************************delete the node address*****************************************/
   if(value_one=="111")
     {
      delay(30000);
     /************************************************************/
      this_node=EEPROM.read(0);
      radio.stopListening();
     strcpy(node_data,"");
     strcat(node_data,"{\"R\":\"2\",\"A\":\"");
     strcat(node_data,rAddresss[this_node]);
     strcat(node_data,"\"}");
     Serial.println(node_data);
      radio.openWritingPipe(PTXpipe);        
     if (!radio.write(&node_data, sizeof(node_data))){  

         //Serial.println(" failed"); 
         digitalWrite(ERROR_LED,HIGH);  
            
        

     }

     else {  

          //Serial.print("Success :");
          digitalWrite(ERROR_LED,LOW);
          Serial.println(node_data);
     }

     radio.startListening();
     delay(30000);
        radio.stopListening();
     /*************change the sttus in eeprom**********************************/
        EEPROM.write(5,0);
        EEPROM.write(0,5);
     /*************change pipenumber to 5**********************************/
        PTXpipe = wAddress[5];
        radio.openReadingPipe(0,PTXpipe);
        radio.startListening();
      /**********************************************/
      // send to gateway  solenoid NOT 5TH NODE ADDRESS
     // {"NA":"2","A":"0X6678888"};
     //send to gateway its sensor 
     //{"NA":"1","A":"0X6678888"};
     
      }
  
 /***********************************control solenoid*****************************************/
      else if(value_one=="3")
      {
       int motor_value= value_three.toInt();
        EEPROM.write(103,motor_value);
        digitalWrite(MOTOR, motor_value);
        int MotorValue = digitalRead(MOTOR);
        String string_motor=String(MotorValue);
        if(NrfAction1 =="T"){
         EEPROM.write(104,value_four.toInt()); 
        }
        /********************send acknowledgement to gateway*********************/
        strcpy(node_data,"");
        strcpy(node_data,"{\"R\":\"6\",\"A\":\"0X6678888\",\"S\":");
        strcpy(node_data,string_motor.c_str());
        strcpy(node_data,"}");
         send_to_node(node_data);
      }

      /***********************************total liters in tank update*****************************************/
      //{"G":"10","TL":"500",”F”:”0”}
      else if(value_one=="10")
      {
        int tank_level= value_two.toInt();
        EEPROM.write(20,tank_level);
        int flowsensor_bool= value_three.toInt();
        EEPROM.write(24,flowsensor_bool);
        
        /********************send acknowledgement to gateway*********************/
        strcpy(node_data,"");
        strcpy(node_data,"{\"R\":\"10\",\"A\":\"0X6678888\"");
        strcpy(node_data,"}");
         send_to_node(node_data);
      }
      /***********************************total liters in tank update*****************************************/
      //={"G":"11","T":"30",”L”:”1000”}
      else if(value_one=="11")
      {
        int full_tank= value_three.toInt();
        EEPROM.write(26,full_tank);
       
        EEPROM.write(24,0);
        /********************send acknowledgement to gateway*********************/
        strcpy(node_data,"");
        strcpy(node_data,"{\"R\":\"11\",\"A\":\"0X6678888\"");
        strcat(node_data,"}");
         send_to_node(node_data);
      }
     /***********************************setting threshold limit*****************************************/
      //={"G":"2","u":"30",”2”:”1000”}
      else if(value_one=="2")
      {   
         
      strcpy(char_lower_limit,"");
       strcpy(char_upper_limit,"");
    
     char ll[2]="";
       strcpy(ll, value_three.c_str());
      strcpy(char_upper_limit, value_two.c_str());
     
        /********************send acknowledgement to gateway*********************/
        Serial.println(char_upper_limit);
        Serial.println(char_lower_limit);
        
             delay(13000);
             /*******************************start sending to gateway*******************************************/      
       radio.stopListening();
       strcpy(node_data,"");
        strcpy(node_data,"{\"R\":\"1\",\"I\":[\"OTSS111\",");
        strcat(node_data,char_upper_limit);
        strcat(node_data,",");
         strcat(node_data,ll);
        strcat(node_data,"]}");
        
       delay(100);
  
      radio.openWritingPipe(PTXpipe);        
     if (!radio.write(&node_data, sizeof(node_data))){  
         //Serial.println(" failed");      
         digitalWrite(ERROR_LED,HIGH);

     }

     else { 

          //Serial.print("Success :");
          digitalWrite(ERROR_LED,LOW);
        
     }

     radio.startListening();
           int upper_limit= value_two.toInt();
        EEPROM.write(30,upper_limit);
        int lower_limit= value_three.toInt();
         EEPROM.write(32,lower_limit);
        
      }
      
          /**********************************************/
      // seTTING THRESHOLD TO NANO
     // {"NA":"2","A":"0X6678888",U:89,L:"00"};
     //FOR SETTING SUCESS IN EEPROM
      // {"NR":"2","A":"0X6678888",U:89,L:"00"};
     //if the threshold is reached 
       // {"NR":"21","A":"0X6678888","T":"89","S":1};
     /***********************************************/
     //send on/off to solenoid
     //{"NA":"1","NA":"0X6678888","S":1};
     //send on/off to solenoid
     //{"NR":"11","NA":"0X6678888","S":1}};
     
     
      }


 bool send_to_node(char *node_data) {
  delay(1000);
  int worked=0;
/*****************stop listening and send data to gateway************************************/
 radio.stopListening();

      radio.openWritingPipe(PTXpipe);        //open writing or transmit pipe
     if (!radio.write(&node_data, sizeof(node_data))){  //if the write fails let the user know over serial monitor
          worked=-1;
         Serial.println(" failed");   
         resend_message(node_data);   

     }

     else { //if the write was successful 

          Serial.print("Success :");

          Serial.println(node_data);
           digitalWrite(5,LOW);
           EEPROM.write(103,0);
           Serial.println(node_data);
          worked=1;
     }

     radio.startListening();   

delay(1000);
  
   return worked; 
}
/*******************************************
 *  FUNCTION:  handleInterrupt

     DISCRIPTION:thris function is called when interrupt is called

     PARAMETERS: void
 ******************************************/
void handleInterrupt() { 
   
   state=!state;

}
/*******************************************
 *  FUNCTION: resend_message

     DISCRIPTION:

     PARAMETERS: int

     RETURNS: succes or failed of sending to node
 * 
 * 
 ******************************************/
 int resend_message(char *node_data){
  Serial.println("resend message is");
  Serial.println(node_data);
  delay(20000);
  
  eeprom_status=EEPROM.read(103);
  Serial.println("eeprom_status");
  Serial.println(eeprom_status);

  if (eeprom_status < 4){
     Serial.println("eeprom_status");
     
      
      int counter =eeprom_status +1;
      EEPROM.write(103,counter);
       send_to_node(node_data);
      digitalWrite(5,HIGH);
      return 1;
  }
  else
    digitalWrite(5,LOW);
    return -1;
 }
/*******************************************
 *  FUNCTION: microsecondsToInches

    DISCRIPTION: convert microseconds To Inches

    PARAMETERS: long

    RETURNS: calculation output
 * 
 * 
 ******************************************/
long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}
/*******************************************
 *  FUNCTION: microsecondsToCentimeters

     DISCRIPTION:convert microseconds To Centimeters

     PARAMETERS: long

     RETURNS: calculation output for centimeters
 * 
 * 
 ******************************************/
long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
