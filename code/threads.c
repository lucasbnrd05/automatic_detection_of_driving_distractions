#include <stdio.h>    // Used for printf() statements
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <wiringPi.h> // Include WiringPi library!

#include <pthread.h>

#include <math.h>
#include <time.h>

#include "devices.h"

#include <semaphore.h>

// semaphore for critical region 
sem_t sem_S1;
sem_t sem_S2;
sem_t sem_D;

#define NUM_THREADS 4


int s1_status = 0;
int s2_status = 0;
int d0_status = 0;
int d1_status = 0;
int d2_status = 0;
int d3_status = 0;

int wheel_direction_initial = 0; 

void *head_tilt (){
	int x_default = Read_Giroscope_X() ;
	int y_default= Read_Giroscope_Y();

	int x_previous = x_default;
	int y_previous= y_default;

	while(1)
	{
		int x_actual = Read_Giroscope_X();
		int y_actual = Read_Giroscope_Y();

		int wheel_direction = read_single_ADC_sensor(2); 
			
		int dif_x_a= x_actual - x_default;
		int dif_y_a= y_actual - y_default;
		
		int dif_x_p = x_previous - x_default;
		int dif_y_p = y_previous - y_default;
		
		int head_tilt = (dif_x_a >= 30 && dif_x_p >= 30) || (dif_x_a <= -30 && dif_x_p <= -30);
		int head_turning = (dif_y_a >= 30 && dif_y_p >= 30) || (dif_y_a <= -30 && dif_y_p <= -30);
		int turn_right = (wheel_direction > wheel_direction_initial+20 && dif_y_p >= 30 && dif_y_a >= 30);
		int turn_left = (wheel_direction > wheel_direction_initial-20 && dif_y_p <= -30 && dif_y_a <= -30);
		
		sem_wait(&sem_S1);
		s1_status = head_tilt || (head_turning && !(turn_right || turn_left));
		sem_post(&sem_S1);
		
		x_previous = x_actual;
		y_previous= y_actual;

		delay(400);
	}
	pthread_exit (NULL);
}


void *safety_distance() {
    while (1) {
        int Speed = read_single_ADC_sensor(3) / 10;
        int dist = getDistance();
        
        int safetydist = (Speed / 10) * (Speed / 10);
		//printf("Speed : %d,Distance  : %d, Safetydistance :  %d, Safety distance / 3 :  %d \n", Speed, dist, safetydist, safetydist/3);

        sem_wait(&sem_D);  
        
        if (dist < safetydist / 3) {
            d3_status = 1;
            d2_status = 0;
            d1_status = 0;
            d0_status = 0;
        } 
        else if (dist < safetydist / 2) {
            d3_status = 0;
            d2_status = 1;
            d1_status = 0;
            d0_status = 0;
        } 
        else if (dist < safetydist) {
            d3_status = 0;
            d2_status = 0;
            d1_status = 1;
            d0_status = 0;
        } 
        else {
            d3_status = 0;
            d2_status = 0;
            d1_status = 0;
            d0_status = 1;
        }

        sem_post(&sem_D);  
        
        delay(300);
    }
    pthread_exit(NULL);
}

void *steering_wheel_turns (){

	int wheel_direction_previous = read_single_ADC_sensor(2); 

	while(1)
	{
		sem_wait(&sem_S2);

		int wheel_direction = read_single_ADC_sensor(2);  
		int Speed = read_single_ADC_sensor(3) /10 ;
		int diff = wheel_direction_previous - wheel_direction;
		if( (diff >= 20 || diff <=-20) && Speed >=40)
			s2_status = 1;
		else 
		{
			s2_status = 0;
			wheel_direction_previous = wheel_direction;
		}
		sem_post(&sem_S2);
		delay(350);
	}
	pthread_exit (NULL);
}

void *symptom_function (){
	while(1)
	{
		sem_wait(&sem_S1);
		int s1 = s1_status;
		sem_post(&sem_S1);
		sem_wait(&sem_S2);
		int s2 = s2_status;
		sem_post(&sem_S2);
		sem_wait(&sem_D);

		int d0 = d0_status;
		int d1 = d1_status;
		int d2 = d2_status;
		int d3 = d3_status;
		sem_post(&sem_D);

		
		//printf("s1 : %d; s2 : %d; d0 : %d; d1 : %d; d2 : %d; d3 : %d \n",s1,s2,d0, d1,d2,d3);	
		if(d3)
		{			
			set_led_1(1);
			set_led_2(1);
			moveServo(180);
			printf("Danger Collision \n");
		}
		else
		{	
			if((s1 || s2 ) && d2)
			{			
				set_led_1(1);
				set_led_2(1);
				moveServo(120);
				printf("Imprudent Distance \n");
			}	
			else
			{
				if((s1 || s2 ) && d1)
				{
					set_led_1(1);
					set_led_2(1);
					moveServo(60);
					printf("Insecure Distance \n");
				}
				else
				{
					moveServo(0);
					if((s1 && s2 ) && d0)
					{
						printf("Correct distance and  Head Tilt and  Sharp Turn \n");

						set_led_2(1);
					}
					else
						set_led_2(0);
					if((s1 || s2 ) && d0)
					{
						set_led_1(1);
						printf("Correct distance and  Head Tilt or  Sharp Turn\n");
					}
					else
					{
						set_led_1(0);
						printf("Nothings happened \n");
					}
				}
			}	

		}
		delay(200);
	}
	pthread_exit (NULL);
}




int main(void)
{
    int n;
    n = init_devices ();
    wheel_direction_initial = read_single_ADC_sensor(2); 

    sem_init(&sem_S1, 0, 1); // init semaphore
    sem_init(&sem_S2, 0, 1); // init semaphore
    sem_init(&sem_D, 0, 1); // init semaphore

    pthread_t thread [NUM_THREADS]; 

    pthread_create(&thread[0], NULL, head_tilt, NULL);
    pthread_create(&thread[1], NULL, safety_distance, NULL);
    pthread_create(&thread[2], NULL, steering_wheel_turns, NULL);
    pthread_create(&thread[3], NULL, symptom_function, NULL);

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pthread_join(thread[3], NULL);

    close_devices ();

    return (0);
}
