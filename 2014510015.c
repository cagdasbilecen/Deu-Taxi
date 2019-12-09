#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
// The maximum number of two threads.
#define MAX_STUDENT 100 
#define MAX_TAXI 10
 
void *student(void *num);
void *taxi(void *);
void randwait(int secs);
///////// currently, there is 1 taxi; many taxi did not be synchronized.(it could be done easly) (not enoughtime to do this.) 
///////// but one taxi work for many students correctly and mutex, semaphores other works has been synchronized.
///////// student number getting with input, 100 can be entered. Or you can enter another small number to look easier.

//Define the semaphores.
// this is busstop.
sem_t waitingRoom;
// taksichair ensures mutually exclusive access to the taxi chair.
sem_t taksiChair;
// taxiDriverPillow is used to allow the taxi driver to sleep until a student arrives.
sem_t taxiDriverPillow;
// seatBelt is used to make the student to wait until the driver done the taking process.
sem_t seatBelt;
// Flag to stop the taxi thread when all students have been taked / carried.
sem_t mutex;     
// MUTEX controlling : students leave the taxi after taxi arrived on faculty               

int allDone = 0;
int numberOfStudentInTaxi=0;
int main(int argc, char *argv[])
{
    pthread_t btid;
    pthread_t tid[MAX_STUDENT];
    pthread_t xtid[MAX_TAXI]; //there is 10 taxi - but currently, it is 1.
    int i, x, numStudents, numChairs, numTaxis; int Number[MAX_STUDENT];
    printf("Maximum number of students can only be 100. Enter number of students.\n");
    scanf("%d",&x);
    numStudents = x;
    numTaxis=1;
   // scanf("%d",&x);
    numChairs = 4;  //all taxis has a 4 chair!
    if (numStudents > MAX_STUDENT) {
       printf("The maximum number of Students is %d.\n", MAX_STUDENT);
       system("PAUSE");   
       return 0;
    }
    printf("-------A solution to the DEU Transport problem using semaphores.------\n");
    for (i = 0; i < MAX_STUDENT; i++) {
        Number[i] = i;
    }
    // Initialize the semaphores with initial values...
    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&taksiChair, 0, 1);
    sem_init(&taxiDriverPillow, 0, 0);
    sem_init(&seatBelt, 0, 0);
    sem_init(&mutex,0,1);   
   
    // Create the taxis.
	for (i = 0; i < numTaxis; i++) {
        pthread_create(&xtid[i], NULL, taxi, (void *)&Number[i]);
    }
    //pthread_create(&btid, NULL, taxi, NULL);
   
    // Create the students.
    for (i = 0; i < numStudents; i++) {
        pthread_create(&tid[i], NULL, student, (void *)&Number[i]);
    }
    // Join each of the threads to wait for them to finish.
    for (i = 0; i < numStudents; i++) {
        pthread_join(tid[i],NULL);
    }
    
    // When all of the students are finished, kill the taxi thread.
    allDone = 1; //work is done.
    sem_post(&taxiDriverPillow); // Wake the taxi driver so he will exit.
    //pthread_join(btid,NULL);
    for (i = 0; i < numTaxis; i++) {  //join taxis as a students
        pthread_join(xtid[i],NULL);
    }
    system("PAUSE");   
    return 0;
}
 
void *student(void *number) {
     
     
     int num = *(int *)number; // Leave for the stop and take some random amount of  time to arrive.
     printf("Student %d leaving for taxi stop.\n", num);
     randwait(5);
     printf("Student %d arrived at taxi stop.\n", num); // Wait for space to open up in the waiting room...
     sem_wait(&waitingRoom);
     printf("Student %d entering taxi.\n", num); // Wait for the taxi chair to become free.
    
     sem_wait(&taksiChair); // The chair is free so give up your spot in the  stop
     sem_post(&waitingRoom); // Wake up the taxi driver!...
     if(numberOfStudentInTaxi==0) {  //student who first entered in taxi, waking up the driver!
     	printf("Student %d waking the taxi driver.\n", num);}
     numberOfStudentInTaxi++;
     sem_post(&taxiDriverPillow); // Wait for the taxi driver to take students
     sem_wait(&seatBelt); // Give up the chair.
     sem_post(&taksiChair);
     sleep(2); //its not first choose, so wait other thread get the source! (mutex) 
     sem_wait(&mutex); //wait for the taxi to arrive to faculty
     printf("Student %d leaving taxi.\n", num);
     sem_post(&mutex); //release mutex
}
 
void *taxi(void *number)
{
  int num = *(int *)number; // Leave for the taxi and take some random amount of  time to arrive.
  while (!allDone) { // Sleep until someone arrives and wakes taxi driver!
    printf("The taxi %d is sleeping\n",num);
    sem_wait(&taxiDriverPillow); // Skip this stuff at the end.
    
    if (!allDone)
     { // Take a random amount of time to take the students
	sem_wait(&mutex); //first arrive, and after students can leave the taxi !

///////// THERE IS SOME SIMULATION ABOUT INFORMATIONS ABOUT CURRENTLY STATES ////////
     if(numberOfStudentInTaxi==1) {
     printf("The taxi %d ANNOUNCE: LAST 3 STUDENTS, LETS GET UP!\n",num);
     printf("Taxi %d seats : [S] - 0 - 0 - 0 \n",num);    //simulate basicly the taxi
     }
     else if(numberOfStudentInTaxi==2) {
     printf("The taxi %d ANNOUNCE: LAST 2 STUDENTS, LETS GET UP!\n",num);
     printf("Taxi %d seats : [S] - [S] - 0 - 0 \n",num);    //simulate basicly the taxi
     }
     else if(numberOfStudentInTaxi==3) {
     printf("The taxi %d ANNOUNCE: LAST 1 STUDENT, LETS GET UP!\n",num);
     printf("Taxi %d seats : [S] - [S] - [S] - 0 \n",num);    //simulate basicly the taxi
     }
     
     else if(numberOfStudentInTaxi==4) { //start transport if student number in taxi is 4
     printf("Taxi %d seats is full now: [S] - [S] - [S] - [S]  \n",num);    //simulate basicly the taxi
     printf("The taxi %d is going to faculty \n",num);
     randwait(3);

     printf("The taxi %d finished the road and come again the taxi stop\n",num);} // Release the student when done number...
     
     sem_post(&seatBelt);
sem_post(&mutex);
    }
    else {
         printf("WORK TIME HAS FINISHED... The taxi %d driver is going home for the day.\n",num);
    }
   }
}
   
void randwait(int secs) {
     int len = 1; // Generate an arbit number...
     sleep(len);
}
