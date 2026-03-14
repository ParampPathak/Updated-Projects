#define _GNU_SOURCE

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

/*** Constants that define parameters of the simulation ***/

#define MAX_RUNWAY_CAPACITY 2    /* Number of aircraft that can use runway simultaneously */
#define CONTROLLER_LIMIT 8       /* Number of aircraft the controller can manage before break */
#define MAX_AIRCRAFT 1000        /* Maximum number of aircraft in the simulation */
#define FUEL_MIN 20              /* Minimum fuel reserve in seconds */
#define FUEL_MAX 60              /* Maximum fuel reserve in seconds */
#define EMERGENCY_TIMEOUT 30     /* Max wait time for emergency aircraft in seconds */
#define DIRECTION_SWITCH_TIME 5  /* Time required to switch runway direction */
#define DIRECTION_LIMIT 3        /* Max consecutive aircraft in same direction */

#define COMMERCIAL 0
#define CARGO 1
#define EMERGENCY 2

#define NORTH 0
#define SOUTH 1
#define EAST  2
#define WEST  4

/* TODO */
/* Add your synchronization variables here */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_commercial = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cond_cargo      = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cond_emergency  = PTHREAD_COND_INITIALIZER;



/* basic information about simulation.  they are printed/checked at the end 
 * and in assert statements during execution.
 *
 * you are responsible for maintaining the integrity of these variables in the 
 * code that you develop. 
 */

static int aircraft_on_runway = 0;       /* Total number of aircraft currently on runway */
static int commercial_on_runway = 0;     /* Total number of commercial aircraft on runway */
static int cargo_on_runway = 0;          /* Total number of cargo aircraft on runway */
static int emergency_on_runway = 0;      /* Total number of emergency aircraft on runway */
static int aircraft_since_break = 0;     /* Aircraft processed since last controller break */
static int current_direction = NORTH;    /* Current runway direction (NORTH or SOUTH) */
static int consecutive_direction = 0;    /* Consecutive aircraft in current direction */
static int controller_on_break = 0;
static int waiting_emergencies = 0;
static int waiting_commercial = 0;
static int waiting_cargo = 0;
static int consecutive_commercial = 0;
static int consecutive_cargo = 0;


typedef struct 
{
  int arrival_time;         // time between the arrival of this aircraft and the previous aircraft
  int runway_time;          // time the aircraft needs to spend on the runway
  int aircraft_id;
  int aircraft_type;        // COMMERCIAL, CARGO, or EMERGENCY
  int fuel_reserve;         // Randomly assigned fuel reserve (FUEL_MIN to FUEL_MAX seconds)
  time_t arrival_timestamp; // timestamp when aircraft thread was created
} aircraft_info;
void emergency_enter(aircraft_info *ai);
void commercial_enter(aircraft_info *ai);
void cargo_enter(aircraft_info *ai);

/* Called at beginning of simulation.  
 * TODO: Create/initialize all synchronization
 * variables and other global variables that you add.
 */

 /*
 * Function: initialize
 * Parameters: aircraft_info *ai, char *filename
 * Returns: int
 * Description:
 * Reads the input file and initializes aircraft data
 * pthread.c example was used
 */

static int initialize(aircraft_info *ai, char *filename) 
{
  aircraft_on_runway    = 0;
  commercial_on_runway  = 0;
  cargo_on_runway       = 0;
  emergency_on_runway   = 0;
  aircraft_since_break  = 0;
  current_direction     = NORTH;
  consecutive_direction = 0;
  


  /* Initialize your synchronization variables (and 
   * other variables you might use) here
   */

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_commercial, NULL);
  pthread_cond_init(&cond_cargo, NULL);
  pthread_cond_init(&cond_emergency, NULL);

  /* seed random number generator for fuel reserves */
  srand(time(NULL));

  /* Read in the data file and initialize the aircraft array */
  FILE *fp;

  if((fp=fopen(filename, "r")) == NULL) 
  {
    printf("Cannot open input file %s for reading.\n", filename);
    exit(1);
  }

  int i = 0;
  char line[256];
  while (fgets(line, sizeof(line), fp) && i < MAX_AIRCRAFT) 
  {
    /* Skip comment lines and empty lines */
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
      continue;
    }
    
    /* Parse the line */
    if (sscanf(line, "%d%d%d", &(ai[i].aircraft_type), &(ai[i].arrival_time), 
               &(ai[i].runway_time)) == 3) {
      /* Assign random fuel reserve between FUEL_MIN and FUEL_MAX */
      ai[i].fuel_reserve = FUEL_MIN + (rand() % (FUEL_MAX - FUEL_MIN + 1));
      i = i + 1;
    }
  }

  fclose(fp);
  return i;
}

/* Code executed by controller to simulate taking a break 
 * You do not need to add anything here.  
 */
__attribute__((unused)) static void take_break() 
{
  printf("The air traffic controller is taking a break now.\n");
  sleep(5);
  assert( aircraft_on_runway == 0 );
  aircraft_since_break = 0;
}

/* Code executed to switch runway direction
 * You do not need to add anything here.
 */
__attribute__((unused)) static void switch_direction()
{
  printf("Switching runway direction from %s to %s\n",
         current_direction == NORTH ? "NORTH" : "SOUTH",
         current_direction == NORTH ? "SOUTH" : "NORTH");
  
  assert( aircraft_on_runway == 0 );  // Runway must be empty to switch
  
  sleep(DIRECTION_SWITCH_TIME);
  
  current_direction = (current_direction == NORTH) ? SOUTH : NORTH;
  consecutive_direction = 0;
  
  printf("Runway direction switched to %s\n",
         current_direction == NORTH ? "NORTH" : "SOUTH");
}

/* Code for the air traffic controller thread. This is fully implemented except for 
 * synchronization with the aircraft. See the comments within the function for details.
 */

 /*
 * Function: controller_thread
 * Parameters: void *arg
 * Returns: void 
 * Description:
 * Manages runway scheduling aircraft priority direction changes and controller breaks
 * Coordinates all aircraft threads
 */

void *controller_thread(void *arg) 
{
  // Suppress the warning for now
  (void)arg;

  printf("The air traffic controller arrived and is beginning operations\n");

  /* Loop while waiting for aircraft to arrive. */
  
  while (1) 
  {
    // From Mutex.c
    pthread_mutex_lock(&mutex);

    // Only wakes up if there is runway capacity open
    if (aircraft_on_runway < MAX_RUNWAY_CAPACITY)
    {
      // Enforces priority for emergencies
      // Fulfills Requirement 6
      if (waiting_emergencies > 0)
      {
        pthread_cond_broadcast(&cond_emergency);
      }
      else
      {
        // Fulfills Requirement 4
        // After 4 commercial prefer cargo
        if (consecutive_commercial >= 4)
        {
          pthread_cond_broadcast(&cond_cargo);
          consecutive_commercial = 0;
          consecutive_cargo = 0;
        }
        // Fulfills Requirement 4
        // After 4 cargo prefer commercial
        else if (consecutive_cargo >= 4)
        {
          pthread_cond_broadcast(&cond_commercial);
          consecutive_commercial = 0;
          consecutive_cargo = 0;
        }
        else
        {
          // See if it matches runway direction
          // Fulfills Requirement 5
          if (current_direction == NORTH)
          { 
            pthread_cond_broadcast(&cond_commercial);
          }
          else
          {
            pthread_cond_broadcast(&cond_cargo);
          }      
        }     
      }
    }
   

    // From Mutex.c and Jazz.c
    pthread_mutex_unlock(&mutex);
    if (aircraft_on_runway < MAX_RUNWAY_CAPACITY)
    {
      pthread_cond_broadcast(&cond_commercial);
      pthread_cond_broadcast(&cond_cargo);
      pthread_cond_broadcast(&cond_emergency);
    }

    // Take a break once limit is reached and runway is clear 
    if (aircraft_since_break >= CONTROLLER_LIMIT && aircraft_on_runway == 0)
    {
      pthread_cond_broadcast(&cond_commercial);
      pthread_cond_broadcast(&cond_cargo);
      pthread_cond_broadcast(&cond_emergency);
      controller_on_break = 1;
      take_break();
      controller_on_break = 0;
    }

    // Force direction switch if limit is hit and runway is empty
    // Fulfills Req 7
    if (consecutive_direction >= DIRECTION_LIMIT && aircraft_on_runway == 0)
    {
      switch_direction();
      pthread_cond_broadcast(&cond_commercial);
      pthread_cond_broadcast(&cond_cargo);
      pthread_cond_broadcast(&cond_emergency);
    } 
     // Fulfills Requirement 7
     if (aircraft_on_runway == 0 && waiting_emergencies == 0) 
     {
      int should_switch =
      (current_direction == NORTH && waiting_commercial == 0 && waiting_cargo > 0) ||
      (current_direction == SOUTH && waiting_cargo == 0 && waiting_commercial > 0);
      
      if (should_switch) 
      {
        switch_direction();
        pthread_cond_broadcast(&cond_commercial);
        pthread_cond_broadcast(&cond_cargo);
      }
    }
   
  

    sleep(1);
    pthread_testcancel();
  }
  



    /* TODO */
    /* Add code here to handle aircraft requests, controller breaks,      */
    /* and runway direction switches.                                     */
    /* Currently the body of the loop is empty.  There's no communication */
    /* between controller and aircraft, i.e. all aircraft are admitted    */
    /* without regard for runway capacity, aircraft type, direction,      */
    /* priorities, and whether the controller needs a break.              */
    /* You need to add all of this.                                       */
    
    /* Allow thread to be cancelled */
    pthread_testcancel();
    usleep(100000); // 100ms sleep to prevent busy waiting
  
  pthread_exit(NULL);
}


/* Code executed by a commercial aircraft to enter the runway.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */

 /*
 * Function: fuel_monitor
 * Parameters: void *arg
 * Returns: void 
 * Description:
 * Monitors aircraft fuel level potentially changes to emergency status
 */

void *fuel_monitor(void *arg)
{
  aircraft_info *ai = (aircraft_info *)arg;
  time_t start = time(NULL);

  while (1)
  {
    sleep(1);

    pthread_mutex_lock(&mutex);

    if (ai->arrival_timestamp == 0) 
    {
      pthread_mutex_unlock(&mutex);
      break;
    }

    double elapsed = difftime(time(NULL), start);

    // Fulfills Requirement 8
    // Fuel is used while waiting
    if (ai->aircraft_type != EMERGENCY && elapsed >= ai->fuel_reserve &&
        ai->arrival_timestamp != 0)
    {
      printf("Aircraft %d is low on fuel declaring emergency!\n",ai->aircraft_id);
      // Fulfills Requirement 6 and 8
      ai->aircraft_type = EMERGENCY;
      pthread_cond_broadcast(&cond_emergency);
      pthread_cond_broadcast(&cond_commercial);
      pthread_cond_broadcast(&cond_cargo);
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);
   
  }

  pthread_exit(NULL);

}

 /*
 * Function: commercial_enter
 * Parameters: aircraft_info, *arg
 * Returns: void 
 * Description:
 * Blocks until a commercial plane can enter and monitors aircraft fuel level 
 * to potentially escalate to an emergency
 */
void commercial_enter(aircraft_info *arg) 
{
    // Suppress the compiler warning
    (void)arg;

  /* TODO */
  /* Request permission to use the runway. You might also want to add      */
  /* synchronization for the simulation variables below.                   */
  /* Consider: runway capacity, direction (commercial prefer NORTH),       */
  /* controller breaks, fuel levels, emergency priorities, and fairness.   */
  /*  YOUR CODE HERE.                                                      */ 

    pthread_mutex_lock(&mutex);
    waiting_commercial++;
    // wait while runway is full, cargo is present, controller is on break, for emergencies, 
    // or if the direction is not north
    // Fulfills Requirement 1,2,3, and 7
    while (aircraft_on_runway >= MAX_RUNWAY_CAPACITY || cargo_on_runway > 0
    || controller_on_break || waiting_emergencies > 0 || current_direction != NORTH)
    {
      if (arg->aircraft_type == EMERGENCY) 
      { 
        waiting_commercial--;
        arg->arrival_timestamp = 0;
        pthread_mutex_unlock(&mutex);
        emergency_enter(arg);
        return;
      }

      pthread_cond_wait(&cond_commercial, &mutex);
      // Fulfills Requirement 9
    }
   
    waiting_commercial--;

      
    aircraft_on_runway++;
    aircraft_since_break++;
    commercial_on_runway++;
    consecutive_direction++;
    consecutive_commercial++;
    consecutive_cargo = 0;
    arg->arrival_timestamp = 0;
    pthread_mutex_unlock(&mutex);
}

/* Code executed by a cargo aircraft to enter the runway.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */

/*
*Enforces runway capacity.
*Blocks aircraft if there are already two on the runway
*Used Mutex.c for mutual exclusion
*/

/*
 * Function: cargo_enter
 * Parameters: aircraft_info *ai
 * Returns: void
 * Description:
 * Handles synchronization and blocks until runway is available.
 * 
 */

void cargo_enter(aircraft_info *ai) 
{
    (void)ai;

  /* TODO */
  /* Request permission to use the runway. You might also want to add      */
  /* synchronization for the simulation variables below.                   */
  /* Consider: runway capacity, direction (cargo prefer SOUTH),            */
  /* controller breaks, fuel levels, emergency priorities, and fairness.   */
  /*  YOUR CODE HERE.                                                      */ 
    pthread_mutex_lock(&mutex);
    waiting_cargo++;

    // wait while runway is full, cargo is present, controller is on break, for emergencies, 
    // or if the direction is not south
    // Fulfills Requirement 1,2,3, and 7
    while (aircraft_on_runway >= MAX_RUNWAY_CAPACITY || commercial_on_runway > 0
    || controller_on_break || waiting_emergencies > 0 || current_direction != SOUTH)
    {
      if (ai->aircraft_type == EMERGENCY) 
      { 
        waiting_cargo--;
        ai->arrival_timestamp = 0;
        pthread_mutex_unlock(&mutex);
        emergency_enter(ai);
        return;
      }
      pthread_cond_wait(&cond_cargo, &mutex);
      // Fulfills Requirement 9
    }
    waiting_cargo--;

    aircraft_on_runway++;
    aircraft_since_break++;
    cargo_on_runway++;
    consecutive_direction++;
    consecutive_cargo++;
    consecutive_commercial = 0;
    ai->arrival_timestamp = 0;
    pthread_mutex_unlock(&mutex);
}


/* Code executed by an emergency aircraft to enter the runway.
 * You have to implement this.  Do not delete the assert() statements,
 * but feel free to add your own.
 */

 /*
 * Function: emergency_enter
 * Parameters: aircraft_info *arg
 * Returns: void
 * Description:
 * Handles emergency aircraft access, gives priority to emergencies
 * and enforces a 30-second timeout for runway access.
 */

void emergency_enter(aircraft_info *ai) 
{
    (void)ai;

  /* TODO */
  /* Request permission to use the runway. You might also want to add      */
  /* synchronization for the simulation variables below.                   */
  /* Emergency aircraft have priority and must be admitted within 30s,     */
  /* but still respect runway capacity and controller breaks.              */
  /* Emergency aircraft can use either direction.                          */
  /*  YOUR CODE HERE.                                                      */ 

    pthread_mutex_lock(&mutex);
    
    // Track emergencies to prioritize them
    waiting_emergencies++;

    time_t start = time(NULL);
    int escalated = 0;

    // Fulfills Requirement 1,2,3,6, and 8
    while (controller_on_break || aircraft_on_runway >= MAX_RUNWAY_CAPACITY) 
    {
      if (!escalated && difftime(time(NULL), start) >= EMERGENCY_TIMEOUT) 
      {
        escalated = 1;
        printf("EMERGENCY aircraft %d escalating priority after %d seconds\n",
                ai->aircraft_id, EMERGENCY_TIMEOUT);
        pthread_cond_broadcast(&cond_emergency);
        pthread_cond_broadcast(&cond_commercial);
        pthread_cond_broadcast(&cond_cargo);
      }
      pthread_cond_wait(&cond_emergency, &mutex);
      // Fulfills Requirement 9
    }
   
    waiting_emergencies--;

    aircraft_on_runway++;
    aircraft_since_break++;
    emergency_on_runway++;
    consecutive_direction++;

    ai->arrival_timestamp = 0;
    pthread_mutex_unlock(&mutex);
}


/* Code executed by an aircraft to simulate the time spent on the runway
 * You do not need to add anything here.  
 */
static void use_runway(int t) 
{
  sleep(t);
}


/* Code executed by a commercial aircraft when leaving the runway.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */


 /*
 * Function: commercial_leave
 * Parameters: none
 * Returns: void
 * Description:
 * Releases the runway after a commercial aircraft finishes
 * Broadcasts condition variables to wake waiting aircraft
 */

static void commercial_leave() 
{
  /* 
   *  TODO 
   *  YOUR CODE HERE. 
   */
  // release one commercial slot then take anything waiting.
  pthread_mutex_lock(&mutex);

  aircraft_on_runway--;
  commercial_on_runway--;

  pthread_cond_broadcast(&cond_commercial);
  pthread_cond_broadcast(&cond_cargo);
  pthread_cond_broadcast(&cond_emergency);
  sleep(1); 
  pthread_mutex_unlock(&mutex);
}

/* Code executed by a cargo aircraft when leaving the runway.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */

/*
 * Function: cargo_leave
 * Parameters: none
 * Returns: void
 * Description:
 * Releases the runway after a cargo aircraft finishes
 * Broadcasts condition variables to wake waiting aircraft
 */

static void cargo_leave() 
{
  /* 
   * TODO
   * YOUR CODE HERE. 
   */

  // release one cargo slot then take anything waiting.
  pthread_mutex_lock(&mutex);

  aircraft_on_runway--;
  cargo_on_runway--;

  pthread_cond_broadcast(&cond_commercial);
  pthread_cond_broadcast(&cond_cargo);
  pthread_cond_broadcast(&cond_emergency);
  sleep(1);
  pthread_mutex_unlock(&mutex);
}

/* Code executed by an emergency aircraft when leaving the runway.
 * You need to implement this.  Do not delete the assert() statements,
 * but feel free to add as many of your own as you like.
 */

  /*
 * Function: emergency_leave
 * Parameters: none
 * Returns: void
 * Description:
 * Releases the runway after an emergency aircraft finishes
 * Broadcasts condition variables to wake waiting aircraft
 */

static void emergency_leave() 
{
  /* 
   * TODO
   * YOUR CODE HERE. 
   */

  // release one emergency slot then take anything waiting.
  pthread_mutex_lock(&mutex);

  aircraft_on_runway--;
  emergency_on_runway--;

  pthread_cond_broadcast(&cond_commercial);
  pthread_cond_broadcast(&cond_cargo);
  pthread_cond_broadcast(&cond_emergency);
  sleep(1);
  pthread_mutex_unlock(&mutex);
}

/* Main code for commercial aircraft threads.  
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* commercial_aircraft(void *ai_ptr) 
{
  aircraft_info *ai = (aircraft_info*)ai_ptr;
  
  /* Record arrival time for fuel tracking */
  ai->arrival_timestamp = time(NULL);
  // From pthread.c
  pthread_t fuel_tid;
  pthread_create(&fuel_tid, NULL, fuel_monitor, ai);
  pthread_detach(fuel_tid);

  /* Request runway access */
  commercial_enter(ai);
  if (ai->aircraft_type == EMERGENCY) 
  {
    pthread_mutex_lock(&mutex);
    printf("Commercial aircraft %d (fuel: %ds) is now on the runway (direction: %s)\n", 
         ai->aircraft_id, ai->fuel_reserve,
         current_direction == NORTH ? "NORTH" : "SOUTH");
  
    assert(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0);
    assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
    assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
    assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);
      
    pthread_mutex_unlock(&mutex);
    
    printf("EMERGENCY aircraft %d begins runway operations for %d seconds\n",
          ai->aircraft_id, ai->runway_time);
    use_runway(ai->runway_time);
    printf("EMERGENCY aircraft %d completes runway operations and prepares to depart\n",
          ai->aircraft_id);

    emergency_leave();  
    printf("EMERGENCY aircraft %d has cleared the runway\n", ai->aircraft_id);
    pthread_exit(NULL);
}
  
  pthread_mutex_lock(&mutex);

  assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway == 0); 
  // fulfills requirement 2
  pthread_mutex_unlock(&mutex);
 


  /* Use runway  --- do not make changes to the 3 lines below*/
  printf("Commercial aircraft %d begins runway operations for %d seconds\n", 
         ai->aircraft_id, ai->runway_time);
  use_runway(ai->runway_time);
  printf("Commercial aircraft %d completes runway operations and prepares to depart\n", 
         ai->aircraft_id);

  /* Leave runway */
  commercial_leave();  

  printf("Commercial aircraft %d has cleared the runway\n", ai->aircraft_id);

  pthread_mutex_lock(&mutex);

  if (!(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0)) 
  {
    printf("ASSERT FAILURE: aircraft_on_runway=%d (should be 0-%d)\n", aircraft_on_runway, MAX_RUNWAY_CAPACITY);
    printf("Runway state: commercial=%d, cargo=%d, emergency=%d, direction=%s\n", 
           commercial_on_runway, cargo_on_runway, emergency_on_runway,
           current_direction == NORTH ? "NORTH" : "SOUTH");
  }
  assert(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);
  
  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}

/* Main code for cargo aircraft threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* cargo_aircraft(void *ai_ptr) 
{
  aircraft_info *ai = (aircraft_info*)ai_ptr;
  
  /* Record arrival time for fuel tracking */
  ai->arrival_timestamp = time(NULL);

  /* Request runway access */
  cargo_enter(ai);
    
  pthread_mutex_lock(&mutex);

  assert(aircraft_on_runway   <= MAX_RUNWAY_CAPACITY && aircraft_on_runway   >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway      >= 0 && cargo_on_runway      <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway  >= 0 && emergency_on_runway  <= MAX_RUNWAY_CAPACITY);
  assert(commercial_on_runway == 0); 

  pthread_mutex_unlock(&mutex);



  printf("Cargo aircraft %d (fuel: %ds) is now on the runway (direction: %s)\n", 
         ai->aircraft_id, ai->fuel_reserve,
         current_direction == NORTH ? "NORTH" : "SOUTH");
    
  pthread_mutex_lock(&mutex);
  if (!(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0)) {
    printf("ASSERT FAILURE: aircraft_on_runway=%d (should be 0-%d)\n", aircraft_on_runway, 
            MAX_RUNWAY_CAPACITY);
    printf("Runway state: commercial=%d, cargo=%d, emergency=%d, direction=%s\n", 
           commercial_on_runway, cargo_on_runway, emergency_on_runway,
           current_direction == NORTH ? "NORTH" : "SOUTH");
  }
  assert(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(commercial_on_runway == 0 ); 
  
  pthread_mutex_unlock(&mutex);

  printf("Cargo aircraft %d begins runway operations for %d seconds\n", 
         ai->aircraft_id, ai->runway_time);
  use_runway(ai->runway_time);
  printf("Cargo aircraft %d completes runway operations and prepares to depart\n", 
         ai->aircraft_id);

  /* Leave runway */
  cargo_leave();        

  printf("Cargo aircraft %d has cleared the runway\n", ai->aircraft_id);
  
  pthread_mutex_lock(&mutex);

  if (!(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0)) {
    printf("ASSERT FAILURE: aircraft_on_runway=%d (should be 0-%d)\n", 
           aircraft_on_runway, MAX_RUNWAY_CAPACITY);
    printf("Runway state: commercial=%d, cargo=%d, emergency=%d, direction=%s\n", 
           commercial_on_runway, cargo_on_runway, emergency_on_runway,
           current_direction == NORTH ? "NORTH" : "SOUTH");
  }
  assert(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);

  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}

/* Main code for emergency aircraft threads.
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void* emergency_aircraft(void *ai_ptr) 
{
  aircraft_info *ai = (aircraft_info*)ai_ptr;
  
  /* Record arrival time for fuel and emergency timeout tracking */
  ai->arrival_timestamp = time(NULL);

  /* Request runway access */
  emergency_enter(ai);

    
  pthread_mutex_lock(&mutex);

  assert(aircraft_on_runway   <= MAX_RUNWAY_CAPACITY && aircraft_on_runway   >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway      >= 0 && cargo_on_runway      <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway  >= 0 && emergency_on_runway  <= MAX_RUNWAY_CAPACITY);

  pthread_mutex_unlock(&mutex);
  

  printf("EMERGENCY aircraft %d (fuel: %ds) is now on the runway (direction: %s)\n", 
         ai->aircraft_id, ai->fuel_reserve,
         current_direction == NORTH ? "NORTH" : "SOUTH");
  
  pthread_mutex_lock(&mutex);

  if (!(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0)) {
    printf("ASSERT FAILURE: aircraft_on_runway=%d (should be 0-%d)\n", aircraft_on_runway, 
            MAX_RUNWAY_CAPACITY);
    printf("Runway state: commercial=%d, cargo=%d, emergency=%d, direction=%s\n", 
           commercial_on_runway, cargo_on_runway, emergency_on_runway,
           current_direction == NORTH ? "NORTH" : "SOUTH");
  }
  assert(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);

  pthread_mutex_unlock(&mutex);

  printf("EMERGENCY aircraft %d begins runway operations for %d seconds\n", 
         ai->aircraft_id, ai->runway_time);
  use_runway(ai->runway_time);
  printf("EMERGENCY aircraft %d completes runway operations and prepares to depart\n", 
         ai->aircraft_id);

  /* Leave runway */
  emergency_leave();        

  printf("EMERGENCY aircraft %d has cleared the runway\n", ai->aircraft_id);

  pthread_mutex_lock(&mutex);

  if (!(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0)) {
    printf("ASSERT FAILURE: aircraft_on_runway=%d (should be 0-%d)\n", 
           aircraft_on_runway, MAX_RUNWAY_CAPACITY);
    printf("Runway state: commercial=%d, cargo=%d, emergency=%d, direction=%s\n", 
           commercial_on_runway, cargo_on_runway, emergency_on_runway,
           current_direction == NORTH ? "NORTH" : "SOUTH");
  }
  assert(aircraft_on_runway <= MAX_RUNWAY_CAPACITY && aircraft_on_runway >= 0);
  assert(commercial_on_runway >= 0 && commercial_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(cargo_on_runway >= 0 && cargo_on_runway <= MAX_RUNWAY_CAPACITY);
  assert(emergency_on_runway >= 0 && emergency_on_runway <= MAX_RUNWAY_CAPACITY);

  pthread_mutex_unlock(&mutex);
  pthread_exit(NULL);
}

/* Main function sets up simulation and prints report
 * at the end.
 * GUID: 355F4066-DA3E-4F74-9656-EF8097FBC985
 */

 /*
 * Function: main
 * Parameters: int nargs, char **args
 * Returns: int
 * Description:
 * Starts the simulation and prints the report
 */

int main(int nargs, char **args) 
{
  int i;
  int result;
  int num_aircraft;
  void *status;
  pthread_t controller_tid;
  pthread_t aircraft_tid[MAX_AIRCRAFT];
  aircraft_info ai[MAX_AIRCRAFT];

  if (nargs != 2) 
  {
    printf("Usage: runway <name of inputfile>\n");
    return EINVAL;
  }

  num_aircraft = initialize(ai, args[1]);
  if (num_aircraft > MAX_AIRCRAFT || num_aircraft <= 0) 
  {
    printf("Error:  Bad number of aircraft threads. "
           "Maybe there was a problem with your input file?\n");
    return 1;
  }

  printf("Starting runway simulation with %d aircraft ...\n", num_aircraft);

  result = pthread_create(&controller_tid, NULL, controller_thread, NULL);

  if (result) 
  {
    printf("runway:  pthread_create failed for controller: %s\n", strerror(result));
    exit(1);
  }

  for (i=0; i < num_aircraft; i++) 
  {
    ai[i].aircraft_id = i;
    sleep(ai[i].arrival_time);
                
    if (ai[i].aircraft_type == COMMERCIAL)
    {
      result = pthread_create(&aircraft_tid[i], NULL, commercial_aircraft, 
                             (void *)&ai[i]);
    }
    else if (ai[i].aircraft_type == CARGO)
    {
      result = pthread_create(&aircraft_tid[i], NULL, cargo_aircraft, 
                             (void *)&ai[i]);
    }
    else 
    {
      result = pthread_create(&aircraft_tid[i], NULL, emergency_aircraft, 
                             (void *)&ai[i]);
    }

    if (result) 
    {
      printf("runway: pthread_create failed for aircraft %d: %s\n", 
            i, strerror(result));
      exit(1);
    }
  }

  /* wait for all aircraft threads to finish */
  for (i = 0; i < num_aircraft; i++) 
  {
    pthread_join(aircraft_tid[i], &status);
  }

  /* tell the controller to finish. */
  pthread_cancel(controller_tid);
  pthread_join(controller_tid, &status);

  printf("Runway simulation done.\n");

  return 0;
}
