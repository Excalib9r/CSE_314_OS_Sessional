#include<bits/stdc++.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

using namespace std;



enum state {NONE, DONE, PRINTING, WAITING};

// variables
int N,M,w,x,y;
int submissions;
vector<int> grp_count;
vector<state> student_states;
time_t start;

// readWriteLock
pthread_rwlock_t rw_lock;

// semaphores
vector<sem_t> st_sem, leader_sem;

// mutex
sem_t print_sem[4];
vector<sem_t> grpcnt;
sem_t bind_lock;


// random number
default_random_engine rand_generator;
poisson_distribution<int> distribution(4.1);


// class
class Student{
    public:
	int id;
	int grp;	
    Student(int id, int grp){
        this->id = id;
        this->grp = grp;
    }
};

// helper function
bool is_anyone_printing(int print_num){
	for(int i = 0; i < N; i++){

		if(i%4 == print_num){
			if(student_states[i] == PRINTING) return true;
		}

	}
	return false;
}

int spent_time(){
	return (int) time(0)-start;
}

void get_printer(Student* st) {
	int id = st -> id -1;
    int print_num = id%4;
	sem_wait(&print_sem[print_num]);

	while(is_anyone_printing(print_num)){
		sem_post(&print_sem[print_num]);
		student_states[id]=WAITING;
		sem_wait(&st_sem[id]);
		sem_wait(&print_sem[print_num]);
	}

	sem_post(&print_sem[print_num]);
	student_states[id] = PRINTING;
	printf("Student %d has arrived at the print station at time %d\n", id + 1, spent_time());
}


void release_printer(Student* st) {
	int grp = st->grp - 1;
    int id = st->id - 1;

    student_states[id] = DONE;

	printf("Student %d has finished printing at time %d\n", id + 1, spent_time());

	// for own members
	for(int i=0;i<M;i++){
		if(student_states[grp*M+i] == WAITING){
			sem_post(&st_sem[grp*M+i]);
		}
	}

	// for other members;
	for(int i=0;i<N/M;i++){
		if(i==grp) continue;
		for(int j=0;j<M;j++){
			if(student_states[i*M+j] == WAITING){
				sem_post(&st_sem[i*M+j]);
			}
		}
	}

	sem_wait(&grpcnt[grp]);
	grp_count[grp]++;
	if(grp_count[grp] == M){
		sem_post(&leader_sem[grp]);
	}
	sem_post(&grpcnt[grp]);
}	

void* printing(void* p){
	sleep(distribution(rand_generator));

	Student* st=(Student*)p;
	int grp =st->grp - 1;
    int id=st->id - 1;

	get_printer(st);
	sleep(w);
	release_printer(st);

	if(id%M != M-1) return NULL;

    sem_wait(&leader_sem[grp]);

	printf("Group %d has finished printing at time %d\n", grp+1, spent_time());
    
    //binding
    sem_wait(&bind_lock);
    printf("Group %d has started binding at time %d\n", grp+1, spent_time());
    sleep(x);
    printf("Group %d has finished binding at time %d\n", grp+1, spent_time());
    sem_post(&bind_lock);

    //writing
    pthread_rwlock_wrlock(&rw_lock);
    sleep(y);
    printf("Group %d has submitted the report at time %d\n", grp+1, spent_time());
    submissions++;
    pthread_rwlock_unlock(&rw_lock);

    return NULL;

}


void* reading(void* p){
    while (true)
    {   
        sleep(distribution(rand_generator));
        pthread_rwlock_rdlock(&rw_lock);
        printf("Staff %d has started reading the entry book at time %d. No. of submission = %d\n", (int*)p, spent_time(), submissions);
        sleep(y);
        pthread_rwlock_unlock(&rw_lock);

        if(submissions == N/M) pthread_exit(NULL);
    }
}


int main(){
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);

    cin>>N>>M>>w>>x>>y;

	start = time(0);
	
	// globalVariable init
    submissions = 0;
	grp_count.resize(N/M);
	student_states.resize(N);

    for(int i = 0; i < N; i++){
		student_states[i] = NONE;
	}
    for(int i = 0; i < N/M; i++){
        grp_count[i] = 0;
    }

    // semaphore init
    st_sem.resize(N);
	leader_sem.resize(N/M);

	for(int i=0;i<N;i++) {
        sem_init(&st_sem[i], 0, 0);
    }

	for(int i=0;i<N/M;i++) {
		sem_init(&leader_sem[i], 0, 0);
	}

    // mutex init
    grpcnt.resize(N/M);
    for(int i=0;i<N/M;i++) {
		sem_init(&grpcnt[i], 0, 1);
	}
	for(int i = 0; i < 4; i++) {
		sem_init(&print_sem[i], 0, 1);
	}

    sem_init(&bind_lock, 0 ,2);

    // init threads

    pthread_t st_threads[N];
    pthread_t stf_threads[2];

	for(int i = 0; i < N; i++)
	{
		Student *student = new Student(i+1, i/M + 1);
		pthread_create(&st_threads[i],NULL,printing,(void*) student);
	}

    for(int i=0;i<2;i++){
        pthread_create(&stf_threads[i], NULL , reading ,(void*)(i+1));
    }

    for(int i=0;i<N;i++)
	{
		pthread_join(st_threads[i],NULL);
	}

	for(int i=0;i<2;i++){
		pthread_join(stf_threads[i],NULL);
	}
}