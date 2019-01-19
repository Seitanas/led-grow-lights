/* LED grow light automation system
 Tadas Ustinavi?ius
 *http://www.neblogas.lt/2012/10/31/homemade-led-grow-light-with-autmation-system/
 2012.10.31
 */
#include <htc.h>


__CONFIG(MCLRE_OFF& WDTE_OFF & CP_OFF & LVP_OFF & BOREN_OFF & FOSC_XT);
#define _XTAL_FREQ 4000000
#define Cnf  RA4
#define hour10 RA3
#define hour1  RA2
#define minute10 RA1
#define minute1 RA0
int seconds, minutes, hours;
int half=0;
int number[10];
int configuration=0;
int on_hrs, on_mins;
int off_hrs, off_mins;
int leds_on=0b10000000;
int show_hours=1, show_mins=1; //display hour and minuters segments

void rst_ports(){
    PORTB=0b01111111;
    hour10=1;
    hour1=1;
    minute10=1;
    minute1=1;
}
void config_init();
void play_config();
void led_display();
void led_display_on();
void led_display_off();
void hr_display();
void min_display();
void on_display();
void off_display();
void done_display();
void dly();
void main() {
    CMCON = 0x7;
    TRISB = 0;
    PORTB =0b11111111;
    TRISA0=0;
    TRISA1=0;
    TRISA2=0;
    TRISA3=0;
    TRISA4=1;
    hour10=0;
    hour1=1;
    minute10=1;
    minute1=1;
    T1CKPS1 = 1;
    T1CKPS0 = 1;
    T1OSCEN = 0;
    TMR1CS = 0;
    GIE = 1;
    PEIE = 1;
    TMR1IE = 1;
    TMR1ON = 1;
    TMR1H = 11;
    TMR1L = 220;
              //Xgfedcba
    number[0]=0b00100000;
    number[1]=0b01111001;
    number[2]=0b01000100;
    number[3]=0b01010000;
    number[4]=0b00011001;
    number[5]=0b00010010;
    number[6]=0b00000010;
    number[7]=0b01111000;
    number[8]=0b00000000;
    number[9]=0b00010000;
    on_hrs=eeprom_read(0x00);
    on_mins=eeprom_read(0x10);
    off_hrs=eeprom_read(0x20);
    off_mins=eeprom_read(0x30);
    if (on_hrs>23)
        on_hrs=0;
    if (off_hrs>23)
        off_hrs=0;
    if (on_mins>59)
        on_mins=0;
    if (off_mins>59)
        off_mins=0;
    while(1){
        int curr_time=hours*100+minutes;
        int full_on_time=on_hrs*100+on_mins;
        int full_off_time=off_hrs*100+off_mins;
    if (full_on_time<=curr_time&&curr_time<full_off_time){
            leds_on=0b10000000;
        }
        else
            leds_on=0b00000000;
        led_display();
        if (!Cnf)
            config_init();
    }
}
void interrupt isr() {
    if (TMR1IF) {
        TMR1H = 11;
        TMR1L = 220;
        TMR1IF = 0;

    if (!configuration){
        if (half){
            ++seconds;
            half=0;
        }
        else
            ++half;
        if (seconds==60){
            ++minutes;
            seconds=0;
        }
        if (minutes==60){
            minutes=0;
            ++hours;
        }
        if (hours==24){
            hours=0;
        }
    }
  }
}
void config_init(){
    rst_ports();
    dly();
    dly();
    dly();
    dly();
    int passive=0;
    int delayT=400;
    if (!Cnf){//if buttons are still presed after 1000ms - start config
        configuration=1;
        play_config();//play animation to show, that config is enabled
        //lets config hours
        hr_display();
        show_mins=0;
        while (passive!=delayT){//while rst button is not pushed
            led_display();
            if (!Cnf){
                ++hours;
                passive=0;
                if (hours==24)
                    hours=0;
                while (!Cnf){//wait for button to be realeased. refresh led for all time.
                     led_display();
                }

            }
            if (Cnf)
                ++passive;
         }
        passive=0;
        //lets config minutes
        show_mins=1;
        show_hours=0;
        min_display();
        while (passive!=delayT){//while rst button is not pushed
            led_display();
            if (!Cnf){
                ++minutes;
                passive=0;
                if (minutes==60)
                    minutes=0;
                while (!Cnf){//wait for button to be realeased. refresh led for all time.
                     led_display();
                }

            }
            if (Cnf)
                ++passive;
         }
        //lets config on hours
        show_mins=0;
        show_hours=1;
        passive=0;
        on_display();
        hr_display();
        while (passive!=delayT){//while rst button is not pushed
            led_display_on();
            if (!Cnf){
                ++on_hrs;
                passive=0;
                if (on_hrs==24)
                    on_hrs=0;
                while (!Cnf){//wait for button to be realeased. refresh led for all time.
                     led_display_on();
                }

            }
            if (Cnf)
                ++passive;
         }
        //lets config on minutes
        show_mins=1;
        show_hours=0;
        passive=0;
        on_display();
        min_display();
        while (passive!=delayT){//while rst button is not pushed
            led_display_on();
            if (!Cnf){
                ++on_mins;
                passive=0;
                if (on_mins==60)
                    on_mins=0;
                while (!Cnf){//wait for button to be realeased. refresh led for all time.
                     led_display_on();
                }

            }
            if (Cnf)
                ++passive;
         }

        //lets config off hours
        show_mins=0;
        show_hours=1;
        passive=0;
        off_display();
        hr_display();
        while (passive!=delayT){//while rst button is not pushed
            led_display_off();
            if (!Cnf){
                ++off_hrs;
                passive=0;
                if (off_hrs==24)
                    off_hrs=0;
                while (!Cnf){//wait for button to be realeased. refresh led for all time.
                     led_display_off();
                }

            }
            if (Cnf)
                ++passive;
         }
        //lets config off minutes
        show_hours=0;
        show_mins=1;
        passive=0;
        off_display();
        min_display();
        while (passive!=delayT){//while rst button is not pushed
            led_display_off();
            if (!Cnf){
                ++off_mins;
                passive=0;
                if (off_mins==60)
                    off_mins=0;
                while (!Cnf){//wait for button to be realeased. refresh led for all time.
                     led_display_off();
                }

            }
            if (Cnf)
                ++passive;
         }




    }
    eeprom_write(0x00,on_hrs);
    eeprom_write(0x10,on_mins);
    eeprom_write(0x20,off_hrs);
    eeprom_write(0x30,off_mins);
configuration = 0 ;
show_hours=1;
show_mins=1;
done_display();
}
void dly(){
   __delay_ms(300);
}
void dly_2(){
   __delay_ms(2);
}
void play_config(){
        rst_ports();
        hour10=0;
        PORTB=number[0]+1;
        dly();
        PORTB=number[0]+2;
        dly();
        PORTB=number[0]+4;
        dly();
        PORTB=number[0]+8;
        dly();
        PORTB=number[0]+16;
        dly();
        PORTB=number[0]+32;
        dly();
        rst_ports();
        hour10=1;
}
void led_display(){
        rst_ports();
        if (show_hours){
        PORTB=number[hours/10]+leds_on;
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=number[hours%10]+leds_on;
        hour1=0;
        dly_2();
        rst_ports();
        }
        if (show_mins){
        PORTB=number[minutes/10]+leds_on;
        minute10=0;
        dly_2();
        rst_ports();
        PORTB=number[minutes%10]+leds_on;
        minute1=0;
        dly_2();
        }
}
void led_display_on(){
        rst_ports();
        if(show_hours){
        PORTB=number[on_hrs/10];
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=number[on_hrs%10];
         hour1=0;
        dly_2();
        rst_ports();
        }
        if (show_mins){
        PORTB=number[on_mins/10];
         minute10=0;
        dly_2();
        rst_ports();
        PORTB=number[on_mins%10];
        minute1=0;
        dly_2();
        }
}
void led_display_off(){
        rst_ports();
        if (show_hours){
        PORTB=number[off_hrs/10];
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=number[off_hrs%10];
        hour1=0;
        dly_2();
        }
        if (show_mins){
        rst_ports();
        PORTB=number[off_mins/10];
        minute10=0;
        dly_2();
        rst_ports();
        PORTB=number[off_mins%10];
        minute1=0;
        dly_2();
        }
}
void min_display(){
    for(int i = 0; i != 200; i++){
        rst_ports();
              //Xgfedcba
        PORTB=0b01001010;
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=number[1];
         hour1=0;
        dly_2();
        rst_ports();
        PORTB=0b01001011;
         minute10=0;
        dly_2();
        rst_ports();
    }
}
void hr_display(){
    for(int i = 0; i != 200; i++){
        rst_ports();
              //Xgfedcba
        PORTB=0b00001011;
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=0b01001111;
         hour1=0;
        dly_2();
        rst_ports();
    }
}
void on_display(){
    for(int i = 0; i != 200; i++){
        rst_ports();
              //Xgfedcba
        PORTB=0b01000011;
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=0b01001011;
         hour1=0;
        dly_2();
        rst_ports();
    }
}
void off_display(){
    for(int i = 0; i != 200; i++){
        rst_ports();
              //Xgfedcba
        PORTB=0b01000011;
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=0b00001110;
         hour1=0;
        dly_2();
        rst_ports();
        PORTB=0b00001110;
         minute10=0;
        dly_2();
        rst_ports();
    }
}
void done_display(){
    for(int i = 0; i != 200; i++){
        rst_ports();
              //Xgfedcba
        PORTB=0b01000001;
        hour10=0;
        dly_2();
        rst_ports();
        PORTB=0b01000011;
         hour1=0;
        dly_2();
        rst_ports();
        PORTB=0b01001011;
        minute10=0;
        dly_2();
       rst_ports();
        PORTB=0b00000110;
         minute1=0;
        dly_2();
        rst_ports();
    }
}