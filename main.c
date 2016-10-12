#include <msp430.h>
#include <intrinsics.h>
#include <HAL_Dogs102x6.h>
#include "Scheduler.h"
#include "ta_32768.h"
#include "SMC_hal.h"


#define H_X_POS 4
#define H_Y_POS 26

#define HEIGHT 7
#define LENGHT 102-H_X_POS

unsigned int a;

unsigned int s1_pushed=0;
unsigned int s2_pushed=0;
unsigned int s1_tick_pressed=0;
unsigned int s2_tick_pressed=0;
unsigned int s1_long=0;
unsigned int s2_long=0;

unsigned int long_press;
unsigned int change_mode;

unsigned int chrono_start = 0;

unsigned int blink_state=0;

unsigned int set_clock=0;
unsigned int set_pos=0;

char mode=0; // Mode: 0-> Clock, 1->Chrono, 2->set_clock
char blink=0;

//For centi and mili seconds
unsigned int ms=0;
unsigned int cs=0;
// Clock vars
unsigned int var_hhd=0; //Ten hour
unsigned int var_hhu=0; //Unit hour
unsigned int var_hmd=0; //Ten minutes
unsigned int var_hmu=0; //Unit minutes
unsigned int var_hsd=0; //Ten seconds
unsigned int var_hsu=0; //Unit seconds

// Chrono vars
unsigned int var_cmd=0; //Ten minutes
unsigned int var_cmu=0; //Unit minutes
unsigned int var_csd=0; //Ten seconds
unsigned int var_csu=0; //Unit seconds
unsigned int var_ccd=0; //Ten cents
unsigned int var_ccu=0; //Unit cents

// Display vars
unsigned int d1=0; //decimal 1
unsigned int d2=0; //decimal 3
unsigned int d3=0; //decimal 2
unsigned int u1=0; //Unit 1
unsigned int u2=0; //Unit 2
unsigned int u3=0; //Unit 3

//Strings for display
static char *horloge  = "Horloge        ";
static char *chrono   = "Chronometre    ";
static char *clockset = "Reglage Horloge";


// To setup clock to freq indicated at the end of the func.
void setup_clk(void) {
	P1OUT |= BIT2;
	// Target Freq for SMCLK, MCLK 3.2Mhz -> 32kHz/8*25*32
	__disable_interrupt();

	// Choosing XT1 and FLLREFDIV as 8
	UCSCTL3 = SELREF__XT1CLK + FLLREFDIV__8;
	// DCORSEL bits to 0
	UCSCTL1 &= ~(BIT4 | BIT5 | BIT6);
	// DCORSEL to 4 for 3.2Mhz Target
	UCSCTL1 |= DCORSEL_4;
	// FLLN set to 25 and FLLD to 32.
	UCSCTL2 = FLLD__32 + 24;

	// Set all the cloks to DCO ref
	UCSCTL4 = SELA__DCOCLK + SELM__DCOCLK + SELS__DCOCLK;

	// All clks dividers to 1
	UCSCTL5 = DIVA__1 + DIVM__1 + DIVS__1;

	// freq ACKL 32'768
	// freq MCLK 3'276'800
	// freq SMCLK 3'276'800


	//__enable_interrupt();
	P1OUT &= ~BIT2;
}

void screen_init(void){
    Dogs102x6_init();
    Dogs102x6_backlightInit();
    Dogs102x6_setBacklight(3);
    Dogs102x6_setContrast(16);
    Dogs102x6_clearScreen();
    Dogs102x6_stringDraw(0, 0, horloge, 0);
    Dogs102x6_stringDraw(H_X_POS, H_Y_POS, "00:00:00", 0);
}


void init(void){

    P1DIR &= ~BIT7;          // Set P1.7 as input

    P1REN |= BIT7;  // Enable P1.7 internal resistance
    P1OUT |= BIT7;  // Set P1.7 as pull-Up resistance
    P1IES |= BIT7;  // P1.7 Hi/Lo edge

    P2DIR &= ~BIT2;          // Set P2.2 as input

    P2REN |= BIT2;  // Enable P2.2 internal resistance
    P2OUT |= BIT2;  // Set P2.2 as pull-Up resistance
    P2IES |= BIT2;  // P2.2 Hi/Lo edge

    // Screen and clock setup
	screen_init();
	setup_clk();
}

void TaskTogglePAD1(void)
{
  TGL_PAD1();
  ms = 0;
  cs= 0;
}

void update_clock(void){
	switch(set_pos){
		case 0:{
			var_hhd+=1;
			break;
		}
		case 1:{
			var_hhu+=1;
			break;
		}
		case 2:{
			var_hmd+=1;
			break;
		}
		case 3:{
			var_hmu+=1;
			break;
		}
		case 4:{
			var_hsd+=1;
			break;
		}
		case 5:{
			var_hsu+=1;
			break;
		}
	}
}

void incr_clock(void){
	if(mode != 2){ //Not when setting clock
	//Every sec
	var_hsu+=1;
	ms=0;
	}
	P1OUT ^= BIT1;
	if(var_hsu==10){
		//Every Ten secs
		var_hsd+=1;
		var_hsu=0;
		if(var_hsd==6){
			//Every Minute
			var_hmu+=1;
			var_hsd=0;
			if(var_hmu==10){
				var_hmd+=1;
				var_hmu=0;
				if(var_hmd==6){
					var_hhu+=1;
					var_hmd=0;
					if(var_hhu==10){
						var_hhd+=1;
						var_hhu=0;
						if(var_hhd>=2){
							var_hhd=0;
						}
					}
				}
			}
		}
	}
}

void incr_chrono(void){
	if(chrono_start){
		//Every cent
		var_ccu+=1;
		cs=0;
		if(var_ccu==10){
			var_ccd+=1;
			var_ccu=0;
			if(var_ccd==10){
				//Every second
				var_csu+=1;
				var_ccd=0;
				P1OUT ^= BIT2;
				if(var_csu==10){
					var_csd+=1;
					var_csu=0;
					if(var_csd==6){
						var_cmu+=1;
						var_csd=0;
						if(var_cmu==10){
							var_cmd+=1;
							var_cmu=0;
							if(var_cmd==2){
								var_cmd=0;
							}
						}
					}
				}
			}
		}
	}
}

void reset_chrono(void){
	var_cmd = 0;
	var_cmd = 0;
	var_csd = 0;
	var_csu = 0;
	var_ccd = 0;
	var_ccu = 0;
}

void update_display_val(void){
	if(mode == 0){ //Clock case
		d1 = var_hhd;
		u1 = var_hhu;
		d2 = var_hmd;
		u2 = var_hmu;
		d3 = var_hsd;
		u3 = var_hsu;
	}
	if(mode == 1){ //Chrono mode
		d1 = var_cmd;
		u1 = var_cmd;
		d2 = var_csd;
		u2 = var_csu;
		d3 = var_ccd;
		u3 = var_ccu;
	}
}

void display_clock(void){
	//Dogs102x6_charDrawXY(HD_X_POS, H_Y_POS,0)
	char display_string[9];
	display_string[0] = d1+'0';
	display_string[1] = u1+'0';
	display_string[3] = d2+'0';
	display_string[4] = u2+'0';
	display_string[6] = d3+'0';
	display_string[7] = u3+'0';

	display_string[2] = ':';
	display_string[5] = ':';
	display_string[8] = '\0';
	if(blink && blink_state){
		display_string[set_pos]==' ';
	}
	else if(blink){
		blink_state=1;
	}

	Dogs102x6_clearRow(H_X_POS);
    Dogs102x6_stringDraw(H_X_POS, H_Y_POS, display_string, 0);
}

//To check if S1 button is pressed
void check_S1(void){
	if((P1IN & BIT7) == 0){
		s1_pushed = 1;
		s1_tick_pressed ++;
		if(s1_tick_pressed > 5){
			s1_long = 1;
		}
	}
	else{
		if(s1_pushed == 0){
			s1_tick_pressed = 0;
		}
		s1_pushed = 0;
		s1_long = 0;
	}
}

//To check if S2 button is pressed
void check_S2(void){
	if((P2IN & BIT2) == 0){
		s2_pushed = 1;
		s2_tick_pressed ++;
		if(s2_tick_pressed > 5){
			s2_long = 1;
		}
	}
	else{
		if(s2_pushed == 0){
			s2_tick_pressed = 0;
		}
		s2_pushed = 0;
		s2_long = 0;
	}
}

void mode_changer(void){
	if(mode == 0){ //Mode horloge
		if(s1_pushed == 0 && s1_tick_pressed <= 5 && s1_tick_pressed > 0){ //Only look when released
			mode = 1; //Change to chrono
			Dogs102x6_clearScreen();
		    Dogs102x6_stringDrawXY(0, 0, chrono, 0);
		}
		else if(s1_long){
			mode = 2;
		}
	}
	else if(mode == 1){ //Mode chrono
		if(s1_pushed == 0 && s1_tick_pressed <= 5 && s1_tick_pressed > 0){
			mode = 0; //Change to clock
			Dogs102x6_clearScreen();
		    Dogs102x6_stringDrawXY(0, 0, horloge, 0);
		}
		if(s2_pushed == 0 && s2_tick_pressed <= 5 && s2_tick_pressed > 0){

			//Stop/Start chrono
			if(chrono_start == 0){ //S´il est arreté -> lancement
				chrono_start = 1;
			}
			else if(chrono_start == 1){ //Si le chrono tourne -> stop
				chrono_start = 0;
			}
		}
		else if(s2_long){
			reset_chrono(); //Reset chrono
		}
	}
	else if(mode == 2){//Mode set horloge
		if(s1_pushed){
			update_clock();
		}
		if(s2_pushed){
			set_pos++;
			if(set_pos >= 6){
				set_pos = 0;
			}
		}
		if(s1_long){
			mode=0;
		}
	}
}


void main(void)
{

   // Stop watchdog timer to prevent time out reset
   WDTCTL = WDTPW + WDTHOLD;

   //Call init function for some basic setup
   init();

   // Initialisation de l'ordonnanceur
   SCH_Init_T0();

   // Enregistrement de la tache en mode horloge de base.
   SCH_Add_Task(incr_clock, 0, 100);
   SCH_Add_Task(incr_chrono, 0, 1);
   SCH_Add_Task(update_display_val, 0, 10);
   SCH_Add_Task(display_clock, 0, 20);
   SCH_Add_Task(check_S1, 0, 10);
   SCH_Add_Task(check_S2, 0, 10);
   SCH_Add_Task(mode_changer, 0, 10);



   // To use clock to drive Led P1.1 through interruptions
   P1DIR |= BIT1;
   TA0CTL |= TASSEL_2 + MC_1; // On MCLK up to Max Register Val
   TA0CCR0 = 32767; // Blink period -> 10ms


   // Demarrage de l'ordonnanceur
   SCH_Start();

   // Boucle infinie. La mise eventuelle en basse consommation
   // est faite dans la fonction SCH_Dispatch_Tasks
   while(1)
   {
	   SCH_Dispatch_Tasks();
   }

}
