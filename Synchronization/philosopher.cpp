/*
* Sarah Asad
* CSS 430 
* Program 3C: Synchronization 
*/

#include <pthread.h> // pthread
#include <stdio.h>
#include <stdlib.h>   // atoi( )
#include <unistd.h>   // sleep( )
#include <sys/time.h> // gettimeofday
#include <iostream>   // cout

#define PHILOSOPHERS 5
#define MEALS 3

using namespace std;

/*
* allows each philosopher to pick up and put down their left /right
* chopsticks through a monitor solution. allows for concurrency.  
*/
class Table2 {
public:
    Table2() {
        //initial state of all 5 philosophers should be thinking. 
        for (int i = 0; i < 5; i++) {
            state[i] = THINKING; 
        }

        pthread_mutex_init(&lock, NULL);
    }
    void pickup(int i) {

        pthread_mutex_lock(&lock);
        //philosopher is hungey so wants to pickup chopsticks
        state[i] = HUNGRY; 
        //call test method to see if he is able to pick up chopsticks
        test(i);
        //if he is unable to pick up chopsticks
        if (state[i] != EATING) {
            //then he needs to wait 
            pthread_cond_wait(&self[i], &lock);
        }
        
        cout << "philosopher[" << i << "] picked up chopsticks" << endl;

        pthread_mutex_unlock(&lock);
    }

    void putdown(int i) {
        pthread_mutex_lock(&lock);

        cout << "philosopher[" << i << "] put down chopsticks" << endl;

        //a philosopher puts down chopsticks when he is thinking 
        state[i] = THINKING; 
        //see if the philosophers sitting next to him are eating 
        test((i + 4) % 5);
        test((i + 1) % 4); 
       
        pthread_mutex_unlock(&lock);
    }

private:
    enum { THINKING, HUNGRY, EATING } state[PHILOSOPHERS];
    pthread_mutex_t lock;
    pthread_cond_t self[PHILOSOPHERS];

    void test(int i) {
        //if the current philosopher is hungry, and the person sitting on either side of the current 
        //philosopher is not hungry, then the current philosopher gets to eat. 
        if ((state[(i + 4) % 5] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % 5] != EATING)) {
            state[i] = EATING; 
            //signal 
            pthread_cond_signal(&self[i]);
        }
    }
};

/*
* allows each philosopher to lock the entire table before picking up their chopsticks
* and unlock the table after putting down the chipsticks. the logic is correct but runs 
* has no concurrency (runs the slowest)
*/
class Table1 {
public:
    Table1() {
        // initialize the mutex lock
        pthread_mutex_init(&lock, NULL);
    }
    void pickup(int i) {
        // lock
        pthread_mutex_lock(&lock);
        cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown(int i) {
        cout << "philosopher[" << i << "] put down chopsticks" << endl;
        // unlock
        pthread_mutex_unlock(&lock);
    }

private:
    //a mutex lock
    pthread_mutex_t lock;
};

class Table0 {
public:
    void pickup(int i) {
        cout << "philosopher[" << i << "] picked up chopsticks" << endl;
    }

    void putdown(int i) {
        cout << "philosopher[" << i << "] put down chopsticks" << endl;
    }
};

static Table2 table2;
static Table1 table1;
static Table0 table0;

static int table_id = 0;

void* philosopher(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < MEALS; i++) {
        switch (table_id) {
        case 0:
            table0.pickup(id);
            sleep(1);
            table0.putdown(id);
            break;
        case 1:
            table1.pickup(id);
            sleep(1);
            table1.putdown(id);
            break;
        case 2:
            table2.pickup(id);
            sleep(1);
            table2.putdown(id);
            break;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    pthread_t threads[PHILOSOPHERS];
    pthread_attr_t attr;
    int id[PHILOSOPHERS];
    table_id = atoi(argv[1]);

    pthread_attr_init(&attr);

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    for (int i = 0; i < PHILOSOPHERS; i++) {
        id[i] = i;
        pthread_create(&threads[i], &attr, philosopher, (void*)&id[i]);
    }

    for (int i = 0; i < PHILOSOPHERS; i++)
        pthread_join(threads[i], NULL);
    gettimeofday(&end_time, NULL);

    sleep(1);
    cout << "time = " << (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec) << endl;

    return 0;
}
