//dht11.c 
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define MAX_TIME 85
#define DHT11PIN 1
#define ATTEMPTS 5                 //retry 5 times when no response
int dht11_val[5]={0,0,0,0,0};

char *r_str;
/*方法三，调用C库函数,*/  
char* join3(char *s1, char *s2)  
{  
	char *fc = ",";
    char *result = malloc(strlen(s1)+strlen(fc)+strlen(s2)+1);//+1 for the zero-terminator  
    //in real code you would check for errors in malloc here  
    if (result == NULL) exit (1);  
  
    strcpy(result, s1);  
    strcat(result, fc);  
    strcat(result, s2);  
  
    return result;  
}  
int dht11_read_val(){
    uint8_t lststate=HIGH;         //last state
    uint8_t counter=0;
    uint8_t j=0,i;
    for(i=0;i<5;i++)
        dht11_val[i]=0;
         
    //host send start signal    
    pinMode(DHT11PIN,OUTPUT);      //set pin to output 
    digitalWrite(DHT11PIN,LOW);    //set to low at least 18ms 
    delay(18);
    digitalWrite(DHT11PIN,HIGH);   //set to high 20-40us
    delayMicroseconds(40);
     
    //start recieve dht response
    pinMode(DHT11PIN,INPUT);       //set pin to input
    for(i=0;i<MAX_TIME;i++)         
    {
        counter=0;
        while(digitalRead(DHT11PIN)==lststate){     //read pin state to see if dht responsed. if dht always high for 255 + 1 times, break this while circle
            counter++;
            delayMicroseconds(1);
            if(counter==255)
                break;
        }
        lststate=digitalRead(DHT11PIN);             //read current state and store as last state. 
        if(counter==255)                            //if dht always high for 255 + 1 times, break this for circle
            break;
        // top 3 transistions are ignored, maybe aim to wait for dht finish response signal
        if((i>=4)&&(i%2==0)){
            dht11_val[j/8]<<=1;                     //write 1 bit to 0 by moving left (auto add 0)
            if(counter>16)                          //long mean 1
                dht11_val[j/8]|=1;                  //write 1 bit to 1 
            j++;
        }
    }
    // verify checksum and print the verified data
    if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))){
        char rh_str[25];
        char temp_str[25];
        sprintf(rh_str, "%d",dht11_val[0]);
        sprintf(temp_str,"%d", dht11_val[2]);
        //printf("RH1:%s,TEMP2:%s\n",rh_str,temp_str);
        r_str = join3(rh_str, temp_str);
        return 1;
    }
    else
        return 0;
}

char* get_rh_temp(){
	int attempts=ATTEMPTS;
    if(wiringPiSetup()==-1)
        exit(1);
    while(attempts){                        //you have 5 times to retry
        int success = dht11_read_val();     //get result including printing out
        if (success) {                      //if get result, quit program; if not, retry 5 times then quit
            return r_str;
        }
        attempts--;
        delay(2500);
    }
    return r_str;
}

int main(void){
    char* s = get_rh_temp();
    printf("%s",s);
    return 0;
}
