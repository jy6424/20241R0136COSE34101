#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define QUEUE_SIZE 10

int avg_TAT[6];
int avg_WT[6];
 

//process 구조체 선언
typedef struct {
    pid_t pid;
    int cpu_burst_time;
    int io_burst_time;
    int arrival_time;
    int remaining_time;
    int priority;
    int waiting_time;
    int turnaround_time;
} process;

//job queue 선언
typedef struct {
    process process_items[QUEUE_SIZE];
    int front;
    int rear;
} Queue;

    

//job queue 초기화
void init_Queue(Queue *Q) {
    Q->front = Q->rear = -1;
}

//circular queue의 형태로 구현
int isempty(Queue *Q) {
    return Q->front == Q->rear;
}

int isfull(Queue *Q){
    return (Q->rear +1)%QUEUE_SIZE == Q->front;
}

//ready queue에 process 삽입
void enqueue(Queue *Q, process data) {
    if (isfull(Q)) printf("job queue is full\n");
    else {
        Q->rear = (Q->rear+1)%QUEUE_SIZE;
        Q->process_items[Q->rear] = data;
    }
}

//process 출력
process dequeue(Queue *Q) { 
    if(isempty(Q)) {
        printf("job queue is empty\n");
    }
    else {
        Q->front = (Q->front+1)%QUEUE_SIZE;
        return Q->process_items[Q->front];

    }
}

//proess 생성
process Create_Process(int cpu , int io , int arr , int pri){
    static int pid = 1; //unique함 보장
    process new_process;

    new_process.pid = pid++;
    new_process.cpu_burst_time = cpu;
    new_process.io_burst_time = io;
    new_process.arrival_time = arr;
    new_process.priority = pri;
    
    return new_process;
}

//Gantt Chart 생성함수
void Gantt_Chart(Queue job) {

    int time = 0;
    float average_TAT=0.00;
    float average_WT=0.00;

    //프로세스 표시
    for (int i = 0; i < QUEUE_SIZE; i++) {
        //해당 시간에 처리할 프로세스가 없는 경우 idle 상태 표시
        if (job.process_items[i].arrival_time - time > 0) {
            
            //idle_time은 얼마나 긴 시간동안 idle상태인지를 나타내는 변수
            int idle_time = job.process_items[i].arrival_time - time;
            printf("| Idle ");

            //idle 시간만큼 공백문자열 출력해 Gantt Chart에 시간 반영
            for (int k = 0; k < idle_time; k++) {
                printf(" ");
            }

            //idle_time만큼 시간이 흘렀다는 점 반영
            time += idle_time;
        }
        
        // pid출력
        printf("| P%d ", job.process_items[i].pid);

        // 프로세싱한 시간만큼 공백 출력
        for (int j = 0; j < job.process_items[i].cpu_burst_time; j++) {
            printf(" ");
        }

        //프로세싱 시간 반영
        time += job.process_items[i].cpu_burst_time;
    }
    //Gantt Chart 끝부분 |
    printf("|\n");

    //Gantt Chart 밑에 시간 표시
    time = 0;
    for (int i = 0; i < QUEUE_SIZE; i++) {

        //idle_time 계산하기
        if (i + 1 < QUEUE_SIZE && job.process_items[i].arrival_time - time>0) {
            int idle_time = job.process_items[i].arrival_time - time;
            
            if(time<10) printf("%d      ", time);
            else printf("%d     ", time);

            for (int k = 0; k < idle_time; k++) {
                printf(" ");
                
            }
            time += idle_time;
        }
        
        //수행시간 표시
        if(job.process_items[i].pid<10) {
            if(time>=10)printf("%d   ", time);
            else printf("%d    ", time);
        }
        else {
            if(time>=10)printf("%d    ", time);
        else printf("%d     ", time);
        }


        //시간 사이 공백 표시
        for (int j = 0; j < job.process_items[i].cpu_burst_time; j++) {
            printf(" ");
        }
        
        
        time += job.process_items[i].cpu_burst_time;
        job.process_items[i].turnaround_time = time - job.process_items[i].arrival_time;
        job.process_items[i].waiting_time = job.process_items[i].turnaround_time - job.process_items[i].cpu_burst_time;
        
    
    }
    //마지막 시간 표시
    printf("%d\n", time);

    //average TAT , average WT 출력 (스케줄링 기법 평가)

    for(int a = 0 ; a<QUEUE_SIZE ; a++) {
        average_TAT += job.process_items[a].turnaround_time;
        average_WT += job.process_items[a].waiting_time;
    }
    printf("average turnaround time is %.2f\n", average_TAT/QUEUE_SIZE);
    printf("average waiting time is %.2f\n\n", average_WT/QUEUE_SIZE);

    
}



//FCFS
void FCFS(Queue job) {
    
    for (int i=0 ; i< QUEUE_SIZE-1 ; i++) {
        for (int j=0 ; j< QUEUE_SIZE-1-i ; j++) {

            //프로세스의 arrival time을 비교해서 다음 프로세스가 arrival time이 적다면 먼저 처리(버블정렬 사용)
            if (job.process_items[j].arrival_time > job.process_items[j+1].arrival_time){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
            }
        }
    }

    printf("<FCFS>\n");
    Gantt_Chart(job);

}


//Non-preemptive SJF
void SJF(Queue job) {

    int running =0;

    Queue run_queue[QUEUE_SIZE];

    
    for (int i=0 ; i< QUEUE_SIZE-1 ; i++) {
        for (int j=0 ; j< QUEUE_SIZE-1 ; j++) {

            //프로세스의 arrival time을 비교해서 다음 프로세스가 arrival time이 적다면 먼저 처리(버블정렬 사용)
            if (job.process_items[j].arrival_time > job.process_items[j+1].arrival_time){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
            }
        }
    }    

    
    for (int i=0 ; i< QUEUE_SIZE ; i++) {
        for (int j=0 ; j< QUEUE_SIZE ; j++) {

            //프로세스의 arrival time을 비교해서 다음 프로세스가 arrival time이 적다면 먼저 처리(버블정렬 사용)
            if (j+1 < QUEUE_SIZE && job.process_items[j].arrival_time == job.process_items[j+1].arrival_time && job.process_items[j].cpu_burst_time > job.process_items[j+1].cpu_burst_time){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
            }
        }
    }   

    printf("<SJF>\n");
    Gantt_Chart(job);
}


//Priority
void PRIORITY(Queue job) {


    for (int i=0 ; i< QUEUE_SIZE-1 ; i++) {
        for (int j=0 ; j< QUEUE_SIZE-1 ; j++) {

            //프로세스의 arrival time을 비교해서 다음 프로세스가 arrival time이 적다면 먼저 처리(버블정렬 사용)
            if (job.process_items[j].arrival_time > job.process_items[j+1].arrival_time){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
            }
        }
    }


    for (int i=0 ; i< QUEUE_SIZE ; i++) {
        for (int j=0 ; j< QUEUE_SIZE ; j++) {

            //프로세스의 arrival time을 비교해서 다음 프로세스가 arrival time이 적다면 먼저 처리(버블정렬 사용)
            if (j+1 < QUEUE_SIZE && job.process_items[j].arrival_time == job.process_items[j+1].arrival_time && job.process_items[j].priority < job.process_items[j+1].priority){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
            }
        }
    }

    printf("<HIGHER_PRIORITY>\n");
    Gantt_Chart(job);
}


//Round Robin
void RR(Queue job) {

    // arrival time 순으로 우선 정렬

    int time = 0;
    int finished_process=0;
    //주기 3으로 초기화
    int quantum = 3;
    float average_TAT=0.00;
    float average_WT=0.00;

        for (int i=0 ; i< QUEUE_SIZE-1 ; i++) {
            for (int j=0 ; j< QUEUE_SIZE-1-i ; j++) {

                if (job.process_items[j].arrival_time > job.process_items[j+1].arrival_time){
                    process temp = job.process_items[j];
                    job.process_items[j] = job.process_items[j+1];
                    job.process_items[j+1] = temp;
                }
            }
        }

    //Round Robin으로 정렬 & Gantt Chart 표시
    printf("<ROUND_ROBIN>\n");
    for (int a=0 ; a<QUEUE_SIZE; a++) {
        job.process_items[a].remaining_time = job.process_items[a].cpu_burst_time;
    }
    //모든 프로세스가 완료되면 while문 종료
    while(finished_process < QUEUE_SIZE) {

        for (int k=0 ; k<QUEUE_SIZE;k++) {
            
            //remaining time이 있는 프로세스만 처리
            if(job.process_items[k].remaining_time>0) {
                
                //만약 현재 처리할 수 있는 프로세스가 존재하지 않을 때, idle상태임을 출력
                if(job.process_items[k].arrival_time>time){
                    printf("| Idle ");
                    for(int a ; a<job.process_items[k].arrival_time-time ; a++){
                        printf(" ");
                    }
                    time+=job.process_items[k].arrival_time;
                }

                

                //cpu사용시간이 주기보다 많이 남아있는 경우
                if(job.process_items[k].remaining_time>quantum) {

                    time += quantum;
                    job.process_items[k].remaining_time -= quantum; 
                    printf("| P%d ", job.process_items[k].pid);
                    //주기만큼 공백문자열 출력해 Gantt Chart에 길이 반영
                    for (int a=0 ; a<quantum ; a++){
                        printf(" ");
                    }
                }

                //cpu사용시간이 주기보다 짧게 남아있는 경우
                else {
                    time += job.process_items[k].remaining_time;    
                    printf("| P%d ", job.process_items[k].pid);
                    //사용시간만큼 공백문자열 출력해 Gantt Chart에 길이 반영
                    for(int b=0; b<job.process_items[k].remaining_time; b++){
                        printf(" ");
                    }
                    job.process_items[k].remaining_time =0;
                    
                    //turnaround time = (현재 시간(끝난 시간)) - (도착 시간)
                    //waiting time = (turnaround time) - (cpu사용시간)
                    job.process_items[k].turnaround_time = time - job.process_items[k].arrival_time;
                    job.process_items[k].waiting_time = job.process_items[k].turnaround_time - job.process_items[k].cpu_burst_time;

                    //finished_process 하나 추가
                    finished_process++;
                }
                
            }
            
        }
        
    }
    printf("|\n");


    //숫자
    for (int a=0 ; a<QUEUE_SIZE; a++) {
        job.process_items[a].remaining_time = job.process_items[a].cpu_burst_time;
    }
    finished_process=0;
    time = 0;
    while(finished_process < QUEUE_SIZE) {

        for (int k=0 ; k<QUEUE_SIZE;k++) {
            //idle
            if(job.process_items[k].arrival_time>time){
                printf("%d       ", time);
                for(int a ; a<job.process_items[k].arrival_time-time ; a++){
                    printf(" ");
                }
                time+=job.process_items[k].arrival_time;
            }
            //도착한 프로세스인데 remaining time이 남아있는 프로세스 출력
            if(job.process_items[k].remaining_time>0) {
                //remaining time >quantum
                if(job.process_items[k].remaining_time>quantum) {

                    if(job.process_items[k].pid<10) {
                        if(time>=10)printf("%d   ", time);
                        else printf("%d    ", time);
                    }
                    else {
                        if(time>=10)printf("%d    ", time);
                        else printf("%d     ", time);
                    }

                    for (int a=0 ; a<quantum ; a++){
                        printf(" ");
                    }
                    time += quantum;
                    job.process_items[k].remaining_time -= quantum; 
                }

                //remaining time <=quantum (프로세스 완료)
                else {
                    if(job.process_items[k].pid<10) {
                        if(time>=10)printf("%d   ", time);
                        else printf("%d    ", time);
                    }
                    else {
                        if(time>=10)printf("%d    ", time);
                        else printf("%d     ", time);
                    }

                    for(int b=0; b<job.process_items[k].remaining_time; b++){
                        printf(" ");
                    }
                    time += job.process_items[k].remaining_time;   
                    job.process_items[k].remaining_time =0;
                    finished_process++;
                }
            }
            
        }
        
    }
    printf("%d\n", time);

   
    for(int a = 0 ; a<QUEUE_SIZE ; a++) {
        average_TAT += job.process_items[a].turnaround_time;
        average_WT += job.process_items[a].waiting_time;
    }
    printf("average turnaround time is %.2f\n", average_TAT/QUEUE_SIZE);
    printf("average waiting time is %.2f\n\n", average_WT/QUEUE_SIZE);
}


//Preemptive SJF(SRJF)

void SRJF(Queue job){
    
    int time = 0;
    int finished_process =0;
    float average_TAT=0.00;
    float average_WT=0.00;

    //arrival time 순으로 우선 정렬
    for (int i=0 ; i< QUEUE_SIZE-1 ; i++) {
        for (int j=0 ; j< QUEUE_SIZE-1-i ; j++) {

            if (job.process_items[j].arrival_time > job.process_items[j+1].arrival_time){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
            }
        }
    }
    //remaining time 초기화
    for (int a=0 ; a<QUEUE_SIZE; a++) {
        job.process_items[a].remaining_time = job.process_items[a].cpu_burst_time;
    }
    
    printf("<Preemptive SJF>\n");

    //현재 실행중인 프로세스 인덱스 번호 초기화 (이후 Gantt Chart 그릴 때 만약 같은 프로세스가 계속 사용하고 있다면 끊지 않고 공백 삽입해 이어지게 만듬)
    int current_index = -1;

    while (finished_process < QUEUE_SIZE) {
        // 우선순위 변수 초기화 (초기 값이므로 최대한 크게 설정)
        int cpu_burst = 1e9;
        //우선순위 프로세스의 인덱스 번호 초기화
        int cpu_burst_index = -1;

        for (int k = 0; k < QUEUE_SIZE; k++) {
            //프로세스가 도착했는지, 처리해야할 시간이 남아있는지 확인 후, 해당 프로세스의 remainingtime과 현재 처리중인 프로세스의 값(cpu_burst)를 비교한다
            if (job.process_items[k].arrival_time <= time && job.process_items[k].remaining_time > 0) {
                //현재 처리중인 프로세스의 cpu_burst 값을 업데이트 한다. 같은 프로세스여도 업데이트 된다.
                if (job.process_items[k].remaining_time < cpu_burst) {
                    cpu_burst = job.process_items[k].remaining_time;
                    cpu_burst_index = k;

                }
            }
        }


        // 해당 시간에 아무 프로세스도 없을 경우 idle 표시
        if (cpu_burst_index == -1) {
            printf("| Idle ");
            time++;
            continue;
        }

        //프로세스 출력
        // 이전에 실행중인 프로세스와 다른 경우, | P1 과 같은 방식으로 새로운 프로세스임을 나타내고, 만약 같은 경우, | 를 사용하지 않고 대신 공백으로 길이 표현
        if(current_index != cpu_burst_index){
            printf("| P%d ", job.process_items[cpu_burst_index].pid);
        }
        // 공백으로 길이 표현
        printf(" ");
        current_index = cpu_burst_index;
        //remaining time 줄이고, time 값 늘리기
        job.process_items[cpu_burst_index].remaining_time--;
        time++;
        

        // 완료되면 finished_process에 1을 더함(모든 프로세스가 실행된 이후 while문 빠져나가기 위함)
        if (job.process_items[cpu_burst_index].remaining_time == 0) {
            
            //turnaround time, waiting time 계산
            job.process_items[cpu_burst_index].turnaround_time = time - job.process_items[cpu_burst_index].arrival_time;
            job.process_items[cpu_burst_index].waiting_time = job.process_items[cpu_burst_index].turnaround_time - job.process_items[cpu_burst_index].cpu_burst_time;
            
            finished_process++;
        }

    }
    printf("|\n");

    //remaining time 초기화
    for (int a=0 ; a<QUEUE_SIZE; a++) {
        job.process_items[a].remaining_time = job.process_items[a].cpu_burst_time;
    }
    
    current_index = -1;
    finished_process = 0;
    time=0;

    while (finished_process < QUEUE_SIZE) {
        // 우선순위 변수 초기화
        int cpu_burst = 1e9;
        //우선순위 프로세스의 인덱스 번호 초기화
        int cpu_burst_index = -1;

        for (int k = 0; k < QUEUE_SIZE; k++) {
            if (job.process_items[k].arrival_time <= time && job.process_items[k].remaining_time > 0) {
                if (job.process_items[k].remaining_time < cpu_burst) {
                    cpu_burst = job.process_items[k].remaining_time;
                    cpu_burst_index = k;
                    
                }
            }
        }

        // 해당 시간에 아무 프로세스가 없을 경우 idle 표시
        if (cpu_burst_index == -1) {
            if(time<10) printf("%d      ", time);
            else printf("%d    ", time);
            time++;
            
            continue;
        }

        // 가장 높은 우선순위 출력
        // 이전에 실행중인 프로세스와 같은 경우 time값이 아니라, 공백으로 길이 표현
        if(current_index != cpu_burst_index){
            if(time<10) printf("%d    ", time);
            else printf("%d   ", time);
        }
        // 공백으로 길이 표현
        printf(" ");
        current_index = cpu_burst_index;
        job.process_items[cpu_burst_index].remaining_time--;
        
        time++;

        // 완료되면 finished_process에 1을 더함(모든 프로세스가 실행된 이후 while문 빠져나가기 위함)
        if (job.process_items[cpu_burst_index].remaining_time == 0) {
            finished_process++;
        }

    }
    printf("%d\n", time);

    //average TAT, average WT 출력 (평가)
    for(int a = 0 ; a<QUEUE_SIZE ; a++) {
        average_TAT += job.process_items[a].turnaround_time;
        average_WT += job.process_items[a].waiting_time;
    }
    printf("average turnaround time is %.2f\n", average_TAT/QUEUE_SIZE);
    printf("average waiting time is %.2f\n\n", average_WT/QUEUE_SIZE);

}


//Preemptive Priority
void Preemptive_pri(Queue job){
    
    
    int time = 0;
    int finished_process =0;
    float average_TAT=0.00;
    float average_WT=0.00;

    //arrival time 순으로 우선 정렬
    for (int i=0 ; i< QUEUE_SIZE-1 ; i++) {
        for (int j=0 ; j< QUEUE_SIZE-1-i ; j++) {

            
            if (job.process_items[j].arrival_time > job.process_items[j+1].arrival_time){
                process temp = job.process_items[j];
                job.process_items[j] = job.process_items[j+1];
                job.process_items[j+1] = temp;
                
            }
        }
    }
    //remaining time 초기화
    for (int a=0 ; a<QUEUE_SIZE; a++) {
        job.process_items[a].remaining_time = job.process_items[a].cpu_burst_time;
    }
    
    printf("<Preemptive Priority>\n");

    //현재 실행중인 프로세스 인덱스 번호 초기화 (이후 Gantt Chart 그릴 때 만약 같은 프로세스가 계속 사용하고 있다면 끊지 않고 공백 삽입해 이어지게 만듬)
    int current_index = -1;

    while (finished_process < QUEUE_SIZE) {
        // 우선순위 변수 초기화
        int max_priority = -1;
        //우선순위 프로세스의 인덱스 번호 초기화
        int max_priority_index = -1;

        //해당 시간에 처리 가능한 프로세스중에서 priority가 높은 것 판단
        for (int k = 0; k < QUEUE_SIZE; k++) {
            if (job.process_items[k].arrival_time <= time && job.process_items[k].remaining_time > 0) {
                //같은 프로세스여도 일단 max_priority, max_priority_index 다시 들어감
                if (job.process_items[k].priority > max_priority) {
                    max_priority = job.process_items[k].priority;
                    max_priority_index = k;
                    
                }
            }
        }

        // 해당 시간에 아무 프로세스가 없을 경우 idle 표시
        if (max_priority_index == -1) {
            printf("| Idle ");
            time++;
            continue;
        }

        // 가장 높은 우선순위 출력
        // 이전에 실행중인 프로세스와 같은 경우, | 를 사용하지 않고 공백으로 길이 표현
        if(current_index != max_priority_index){
            printf("| P%d ", job.process_items[max_priority_index].pid);
        }
        // 공백으로 길이 표현
        printf(" ");
        
        //현재 인덱스를 최대 인덱스로 초기화 해 이후에 | P%d 다시 출력되는 현상 방지
        current_index = max_priority_index;
        //remaining time 줄이고, time 늘리기
        job.process_items[max_priority_index].remaining_time--;
        time++;

        // 완료되면 finished_process에 1을 더함(모든 프로세스가 실행된 이후 while문 빠져나가기 위함)
        if (job.process_items[max_priority_index].remaining_time == 0) {
            finished_process++;
        }
        
    }   
    printf("|\n");

    //remaining time 초기화
    for (int a=0 ; a<QUEUE_SIZE; a++) {
        job.process_items[a].remaining_time = job.process_items[a].cpu_burst_time;
    }
    
    //현재 실행중인 프로세스 인덱스 번호 초기화 (이후 Gantt Chart 그릴 때 만약 같은 프로세스가 계속 사용하고 있다면 끊지 않고 공백 삽입해 이어지게 만듬)
    current_index = -1;
    finished_process = 0;
    time=0;

    while (finished_process < QUEUE_SIZE) {
        // 우선순위 변수 초기화
        int max_priority = -1;
        //우선순위 프로세스의 인덱스 번호 초기화
        int max_priority_index = -1;

        for (int k = 0; k < QUEUE_SIZE; k++) {
            if (job.process_items[k].arrival_time <= time && job.process_items[k].remaining_time > 0) {
                if (job.process_items[k].priority > max_priority) {
                    max_priority = job.process_items[k].priority;
                    max_priority_index = k;
                    
                }
            }
        }

        // 해당 시간에 아무 프로세스가 없을 경우 idle 표시
        if (max_priority_index == -1) {
            if(time<10) printf("%d      ", time);
            else printf("%d    ", time);
            time++;
            continue;
        }
        
        // 가장 높은 우선순위 출력
        // 이전에 실행중인 프로세스와 같은 경우, time 출력하지 않고 공백으로 길이 표현
        if(current_index != max_priority_index){
            if(job.process_items[current_index].pid>=10) printf(" ");
            if(time<10) printf("%d    ", time);
            else printf("%d   ", time);
        }
        // 공백으로 길이 표현
        printf(" ");
        current_index = max_priority_index;
        job.process_items[max_priority_index].remaining_time--;
        time++;

        // 완료되면 finished_process에 1을 더함(모든 프로세스가 실행된 이후 while문 빠져나가기 위함)
        if (job.process_items[max_priority_index].remaining_time == 0) {

            //turnaround time, waiting time 계산
            job.process_items[max_priority_index].turnaround_time = time - job.process_items[max_priority_index].arrival_time;
            job.process_items[max_priority_index].waiting_time = job.process_items[max_priority_index].turnaround_time - job.process_items[max_priority_index].cpu_burst_time;
            
            //프로세스 마쳤다는 신호 보내기
            finished_process++;
        }

    }
    printf("%d\n", time);


    //average TAT, average WT 출력 (평가)
    for(int a = 0 ; a<QUEUE_SIZE ; a++) {
        average_TAT += job.process_items[a].turnaround_time;
        average_WT += job.process_items[a].waiting_time;
    }
    printf("average turnaround time is %.2f\n", average_TAT/QUEUE_SIZE);
    printf("average waiting time is %.2f\n\n", average_WT/QUEUE_SIZE);

}


//메인함수 실행
int main() {

    Queue Queue;
    init_Queue(&Queue);

    
    //cputime , iotime , arrivaltime, priority
    
    for(int i=0; i<QUEUE_SIZE;i++){    
        int list[3];
            for (int j=0; j<4; j++){
                int a = rand()%10;
                list[j]=a;      
            }
        enqueue(&Queue, Create_Process(list[0],list[1],list[2],list[3]));
    }

    for(int i=0;i<QUEUE_SIZE;i++){
        printf("P%d : CPU:%d , IO:%d, ARR:%d, PRI:%d \n", Queue.process_items[i].pid , Queue.process_items[i].cpu_burst_time , Queue.process_items[i].io_burst_time, Queue.process_items[i].arrival_time, Queue.process_items[i].priority);
    }

    FCFS(Queue);
    SJF(Queue);
    PRIORITY(Queue);
    SRJF(Queue);
    Preemptive_pri(Queue);
    RR(Queue);
        
}