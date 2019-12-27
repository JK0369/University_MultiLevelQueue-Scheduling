#include <stdio.h>
#include <stdlib.h>

/* source program 순서
1.Element 구현(구조체구현)
2.Ready_Queue 구현
3.전역변수 선언
4.bubbleSort 구현 (arrival_time을 고려하기 위해 필요한 정렬함수)
5.I/O 구현함수
6.알고리즘 구현(전체적인 흐름인 main부 / RR,SJF,FCFS 구현)
*/

/*        1.Element 구현         */
// Element : 프로세스의 정보를 가지고 있음
// queue안의 원소가 될 Element 타입 (삽입, 삭제 연산에 유리한 linked-list로 구현)
typedef struct Element {
	int process_number;
	int priority;
	int arrival_time;
	int burst_time;
	int finish_time;
	int waiting_time;

	int burst_time_init; // 초기 burst time 저장 (scheduling후 waiting_time 구할 때 사용)
	int arrival_time_init; // 초기 arrival time (scheduling후 waiting_time 구할 때 사용)

	struct Element * next; // linked-list 구현
}Element;

// 프로세스 정보 초기화 함수
initElement(Element * e) {
	e->process_number = 0;
	e->priority = 0;
	e->arrival_time = 0;
	e->burst_time = 0;
	e->finish_time = 0;
	e->waiting_time = 0;
	e->burst_time_init = 0;
	e->arrival_time_init = 0;
	e->next = NULL;
}

// 받은 프로세스 정보를 똑같이 복사해서 반환하는 함수
Element* copyElement(Element * e) {
	if (e == NULL) return NULL;

	Element * element = NULL;
	element = (Element *)malloc(sizeof(Element));

	element->process_number = e->process_number;
	element->priority = e->priority;
	element->arrival_time = e->arrival_time;
	element->burst_time = e->burst_time;
	element->finish_time = e->finish_time;
	element->waiting_time = e->waiting_time;
	element->burst_time_init = e->burst_time_init;
	element->arrival_time_init = e->arrival_time_init;
	element->next = e->next;
	return element;
}

/*        2.Ready_Queue 구현         */
// linked-list로 구현한 ready_queue 구조체
typedef struct {
	Element * front;
	Element * rear;
	int count;
}Ready_Queue;

// Ready_Queue 필요한 함수
// format 함수는 4개의 프로세스 정보를 받아서 Element 요소로 반환하는 함수
Element format(int process_number, int priority, int arrival_time, int burst_time);
void initQueue(Ready_Queue * queue);
int isEmpty(Ready_Queue * queue);
void push(Ready_Queue * queue, Element e);
Element* pop(Ready_Queue * queue);
Element* popObject(Ready_Queue * queue, Element * e);

// 파일 입력 받을 때, 4가지 정보를 받아서 생성하는 함수
Element format(int process_number, int priority, int arrival_time, int burst_time) {
	Element * e = (Element *)malloc(sizeof(Element));

	e->process_number = process_number;
	e->priority = priority;
	e->arrival_time = arrival_time;
	e->burst_time = burst_time;
	e->finish_time = -1;
	e->waiting_time = -1;
	e->burst_time_init = burst_time;
	e->arrival_time_init = arrival_time;
	e->next = NULL;

	return *e;
}

void initQueue(Ready_Queue * queue) {
	queue->front = NULL;
	queue->rear = NULL;
	queue->count = 0;
}

int isEmpty(Ready_Queue * queue) {
	if (queue->count == 0) return 1;
	else return 0;
}

void push(Ready_Queue * queue, Element e) {

	Element * tmp = (Element *)malloc(sizeof(Element));
	initElement(tmp);
	tmp = copyElement(&e);
	tmp->next = NULL;

	// queue가 비어있으면 맨 앞에 e 삽입, 그렇지 않으면 현재 맨 끝의 다음에 e 삽입.
	if (isEmpty(queue)) {
		queue->front = tmp;
		queue->rear = tmp;
	}
	else {
		queue->rear->next = tmp;
		queue->rear = tmp;
	}
	queue->count++;

}

Element* pop(Ready_Queue * queue) {
	if (isEmpty(queue)) {
		return NULL;
	}

	Element * front_free = queue->front; // 삭제하기 위해 주소 저장

	Element * front_tmp = (Element*)malloc(sizeof(Element));
	initElement(front_tmp);
	front_tmp = copyElement(queue->front); // 반환하기 위해 값 저장

	front_tmp->next = NULL;

	// 원소가 하나밖에 없는 경우 rear도 NULL값으로 지정
	if (queue->count == 1) {
		queue->front = NULL;
		queue->rear = NULL;
		queue->count--;
		return front_tmp;
	}

	queue->front = queue->front->next;
	queue->count--;

	free(front_free);
	return front_tmp;
}

// 특정 위치 pop 연산
Element* popObject(Ready_Queue * queue, Element * e) {
	if (isEmpty(queue)) return NULL;

	Element * tmp;
	Element * pre = queue->front;

	// 삭제하려는 위치 탐색
	for (tmp = queue->front; tmp != e; tmp = tmp->next) {}

	// 바로 직전의 요소 탐색
	while (1) {
		if (pre == NULL) break;
		// pre의 값을 찾은 경우
		if (pre->next == e) {
			break;
		}
		pre = pre->next;
	}

	// 맨 처음 요소를 pop하는 경우
	if (queue->front == tmp) {
		return pop(queue);
	}
	// 맨 마지막 요소를 pop하는 경우
	else if (queue->rear == tmp) {
		queue->rear = pre;
		pre->next = NULL;
		queue->count--;
		return tmp;
	}
	// 맨 처음, 맨 마지막 요소가 아닌 중간 요소를 pop하는 경우
	else {
		pre->next = tmp->next;
		queue->count--;
		return tmp;
	}
	return NULL;
}

/*        3.전역변수 선언         */

#define MAX_SIZE 70000	   // 프로세스 결과값이 들어갈 수 있는 크기
int current_time = 0;		   // 현재 스케쥴링 처리 진행 시간
Ready_Queue ready_queue[3];	      // multilevel queue, [0]은 RR, [1]은 SJF, [2]는 FCFS
Element * current_process;	  // start 프로세스를 찾기위한 전역변수 선언
int process_waiting_time[MAX_SIZE];	  // 프로세스 들의 waiting_time 결과값이 저장될 공간
int UseCpuQueue = -1;		  // cpu가 현재 어떤 queue를 사용하는지 체크

							  /*        4.bubbleSort : queue안의 데이터를 arrival_time 기준 오름차순 정렬           */
void bubbleSort(Ready_Queue * queue) {

	//queue가 비어있거나 개수가 한개인 경우 : 정렬 안해도 됨
	if (isEmpty(queue)) return;
	if (queue->count == 1) return;

	int i;
	Element * j;

	// queue에 데이터가 2개 이상 경우 : 정렬
	for (i = 0; i < queue->count; i++) {
		for (j = queue->front; j->next != NULL; j = j->next) {
			if (j->arrival_time > j->next->arrival_time) {

				// swap하기 위해, 바로 직전의 요소 탐색
				Element * pre = queue->front;
				while (1) {
					// 현재 요소가 front인 경우
					if (pre == j) {
						pre = NULL;
						break;
					}

					// pre의 값을 찾은 경우
					if (pre->next == j) {
						break;
					}

					pre = pre->next;
				}

				// swap하기 위해, 바로 다음의 다음 요소 탐색
				Element * post = j->next->next;

				// 바꾸려는 첫 번째 데이터
				Element * current = (Element*)malloc(sizeof(Element));
				current = copyElement(j);

				// 바꾸려는 두 번째 데이터
				Element * current_next = (Element*)malloc(sizeof(Element));
				current_next = copyElement(j->next);

				// 직후 요소가 rear인 경우 rear값 조정
				if (queue->rear == j->next) {
					queue->rear = current;
				}

				//현재 요소가 front인 경우
				if (j == queue->front) {

					queue->front = current_next;
					current_next->next = current;
					current->next = post;
					j = current_next;
				}
				// 바로 직전 요소가 front가 아니고 현재 요소가 front가 아닌 경우
				else if (pre != queue->front && pre != NULL) {

					pre->next = current_next;
					current_next->next = current;
					current->next = post;
					j = current_next;
				}

				// 바로 직전 요소가 front인 경우
				else {

					queue->front->next = current_next;
					current_next->next = current;
					current->next = post;
					j = current_next;
				}
			}

		}
	}

}

/*        5.I/O 작업           */
int duplicateCheck[MAX_SIZE] = { 0 }; // process number가 중복인 경우를 check 하는 변수

int fileOpen() {
	int process_number = -1;
	int priority = -1;
	int arrival_time = -1;
	int burst_time = -1;

	FILE * fp = fopen("process.in", "r");

	if (fp == NULL) {
		printf("파일이 존재하지 않음 = 종료\n");
		exit(1);
	}

	while (1) {
		int ret = fscanf(fp, "%d %d %d %d",
			&process_number,
			&priority,
			&arrival_time,
			&burst_time);
		if (ret == EOF) break;

		// 예외 처리 -> 한 개라도 입력을 못받은 경우
		if (process_number == -1 || priority == -1 || arrival_time == -1 || burst_time == -1) {
			printf("파일이 잘못된 구성 = 종료\n");
			exit(1);
		}

		// 예외 처리 -> process_number가 음수인 경우
		if (process_number < 0) {
			printf("process_number가 음수 (잘못입력) = 종료\n");
			exit(1);
		}

		// 예외 처리 -> priority가 1~3이 아닌 경우
		if (priority < 0 || priority > 3) {
			printf("priority가 범위에서 벗어남 = 종료\n");
			exit(1);
		}

		// 예외 처리 -> arrival_time이 음수인 경우.
		if (arrival_time < 0) {
			printf("arrival_time이 음수값 = 종료\n");
			exit(1);
		}

		// 예외 처리 -> burst_time이 음수인 경우.
		if (burst_time < 0) {
			printf("burst_time이 음수값 = 종료\n");
			exit(1);
		}

		// 예외 처리 -> process_number가 중복인 경우.
		if (duplicateCheck[process_number] != 0) {
			printf("process_number 중복 = 종료\n");
			exit(1);
		}

		// 프로세스 정보 생성
		Element e;
		e = format(process_number, priority, arrival_time, burst_time);
		duplicateCheck[process_number] = 1;

		// ready_queue에 프로세스 정보 삽입
		push(&ready_queue[priority - 1], e);

		// 예외 처리를 위해서 초기화 작업
		process_number = -1;
		priority = -1;
		arrival_time = -1;
		burst_time = -1;
	}

	// sort
	for (int i = 0; i < 3; i++) bubbleSort(&ready_queue[i]);

	fclose(fp);

	return 1;
}

/*        6.알고리즘           */
void execute();
int isEndScheduling();
void checkNextProcess();
void RR();
void SJF();
void FCFS();

int main(void) {

	// 전역변수에 있는 ready_queue, current_process 초기화
	for (int i = 0; i < 3; i++) {
		initQueue(&ready_queue[i]);
	}

	// 결과 값 -1로 초기화
	for (int i = 0; i < MAX_SIZE; i++) process_waiting_time[i] = -1;

	current_process = (Element *)malloc(sizeof(Element));
	current_process = NULL;

	// 파일 오픈
	if (!fileOpen()) {
		printf("파일이 열리지 않음\n");
		return 0;
	}

	// 첫 번째로 실행할 process 탐색
	checkNextProcess();

	// sheduling 실행
	execute();

	// sheduling 결과 출력
	printf("process_number\twaiting_time\n");
	for (int i = 0; i < MAX_SIZE; i++) {
		if (process_waiting_time[i] != -1)
			printf("%d\t\t%d\n", i, process_waiting_time[i]);
	}

	return 0;
}

void execute() {
	while (!(isEndScheduling())) { // scheduling 작업이 끝날 때 까지 진행
		checkNextProcess();

		// 현재 진행해야할 프로세스가 NULL인 경우 = 일이 모두 끝난 경우.
		if (current_process == NULL) return;
		switch (current_process->priority - 1) {
		case 0:RR(); break;
		case 1:SJF(); break;
		case 2:FCFS(); break;
		}
	}
}

int isEndScheduling() {
	for (int i = 0; i < 3; i++) {
		if (ready_queue[i].count != 0) return 0;
	}
	return 1;
}

// 다음 진행할 프로세스를 찾는 함수
// 1) 프로세스가 일이 끝나서 다음 프로세스 탐색
// 2) 프로세스 일하다가 ready_queue 선점 여부를 파악
void checkNextProcess() {

	// 다음 진행할 프로세스를 찾았거나, 처리할 일이 없는 경우 종료
	// 우선순위가 낮은 ready_queue부터 탐색
	while (current_process == NULL && !(isEndScheduling())) {
		// FCFS queue는 우선순위 가장 낮으므로 선점 대상이 아님
		if (ready_queue[2].count != 0 && UseCpuQueue == -1) {
			if (current_time >= ready_queue[2].front->arrival_time)
				current_process = ready_queue[2].front;
		}

		// SJF queue는 현재 사용 queue가 RR queue가 아니면 선점 대상이 될 수 있음
		// 단, SJF queue 끼리는 비선점 방식이므로, 현재 SJF queue이면 선점 대상이 아님
		if (ready_queue[1].count != 0 && UseCpuQueue != 0 && UseCpuQueue != 1) {
			Element * i = (Element*)malloc(sizeof(Element));
			initElement(i);
			if (ready_queue[1].front->arrival_time <= current_time)
				current_process = ready_queue[1].front;

			// burst_time이 가장 적은 것 탐색
			for (i = ready_queue[1].front; i->next != NULL; i = i->next) {
				if (current_time >= i->arrival_time && i->burst_time > i->next->burst_time) {
					current_process = i->next;
				}
			}
		}

		if (ready_queue[0].count != 0) {
			if (current_time >= ready_queue[0].front->arrival_time)
				current_process = ready_queue[0].front;
		}

		// 아직 queue에 process가 아무것도 도착하지 않았고 
		//현재 cpu가 scheduling안하는 경우 : 현재 시간 증가
		if (current_process == NULL && UseCpuQueue == -1) {
			int isIncrease = 0;

			if (!isEmpty(&ready_queue[0])) {
				if (ready_queue[0].front->arrival_time > current_time)
					isIncrease = 1;
			}
			if (!isEmpty(&ready_queue[1])) {
				if (ready_queue[1].front->arrival_time > current_time)
					isIncrease = 1;
				else isIncrease = 0;
			}
			if (!isEmpty(&ready_queue[2])) {
				if (ready_queue[2].front->arrival_time > current_time)
					isIncrease = 1;
				else isIncrease = 0;
			}
			if (isIncrease)	current_time++;
		}
		else return;
	}
}

void RR() {

	// RR 알고리즘 : 일을 +1수행 후 burst_time이 0인지(일이 끝난지) 체크, 
	// 끝났으면 업데이트한다.(단, 도착시간 고려)
	// 일이 끝나지 않았으면 +1을 더 한 후 ready_queue에 push한 후 다음 process도 반복한다.
	while (1) {

		// ready_queue에서 프로세스를 모두 처리한 경우 종료
		if (ready_queue[0].count == 0) {
			current_process = NULL;
			UseCpuQueue = -1;
			checkNextProcess();
			break;
		}

		current_process = ready_queue[0].front;
		UseCpuQueue = 0;

		// 현재 진행 시간보다 도착시간이 늦은 경우 다른 ready queue 탐색
		if (current_process->arrival_time > current_time) {
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			return;
		}

		current_process = pop(&ready_queue[0]);

		// 처음 들어올 때 부터 burst_time이 0인 경우
		if (current_process->burst_time == 0) {
			current_process->finish_time = current_time;
			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;

			// 결과 값 저장
			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			return;
		}

		current_process->burst_time--;
		current_time++;

		// 1초 지난 후 일이 끝난 경우
		if (current_process->burst_time == 0) {
			current_process->finish_time = current_time;
			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;

			// 결과 값 저장
			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
		}
		// 일이 끝나지 않은 경우
		else {
			current_process->burst_time--;
			current_time++;

			// 최종 2초후 일이 끝난 경우
			if (current_process->burst_time == 0) {
				current_process->finish_time = current_time;
				current_process->waiting_time = current_process->finish_time
					- current_process->arrival_time_init - current_process->burst_time_init;

				// 결과값 저장
				process_waiting_time[current_process->process_number]
					= current_process->waiting_time;
				UseCpuQueue = -1;
				current_process = NULL;
			}
			// 2초 후에도 일이 안끝난 경우 : queue에 삽입 후, 다음 process 다시 시작
			else {
				current_process->arrival_time = current_time;
				push(&ready_queue[0], *current_process);
				bubbleSort(&ready_queue[0]);
			}
		}
	}
}

void SJF() {
	Element * i;

	i = (Element*)malloc(sizeof(Element));
	initElement(i);

	// ready_queue에 처리할 프로세스 없는 경우 종료
	if (ready_queue[1].count == 0) {
		UseCpuQueue = -1;
		checkNextProcess();
		return;
	}

	int min_burst = 987654321;
	// queue에서 burst_time이 가장 적은 것 탐색 : current_process 업데이트
	for (i = ready_queue[1].front; i != NULL; i = i->next) {
		if (i->arrival_time <= current_time && min_burst > i->burst_time) {
			min_burst = i->burst_time;
			current_process = i;
		}
	}

	// queue에서 해당 요소 pop
	current_process = popObject(&ready_queue[1], current_process);
	UseCpuQueue = 1;

	while (1) {
		// 원래부터 burst_time이 0인 경우, 저장 후 return
		if (current_process->burst_time == 0) {
			current_process->finish_time = current_time;

			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;
			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;

			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			bubbleSort(&ready_queue[1]);
			return;
		}

		current_process->burst_time--;
		current_time++;

		// 일이 끝나지 않은 경우, ready_queue 선점여부를 판단
		Element * stored_process = copyElement(current_process);
		if (current_process->burst_time != 0) {
			current_process = NULL;
			checkNextProcess();

			// 선점이 되면 기존의 process를 queue에 삽입한 후 
			// 다른 queue에서의 작업 실행
			if (current_process != NULL) {
				if (current_process->priority != 2) {
					stored_process->arrival_time = current_time;
					push(&ready_queue[1], *stored_process);
					bubbleSort(&ready_queue[1]);
					UseCpuQueue = -1;
					return;
				}
			}

			// 선점 되지 않으면 계속 해당 queue에서 작업 실행
			else {
				current_process = stored_process;
			}
		}

		// 일이 끝난 경우 결과 값 저장 후, 새로운 ready_queue 탐색
		else {
			current_process->finish_time = current_time;

			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;
			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;

			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			bubbleSort(&ready_queue[1]);
			return;
		}
	}
}

// First Come First Served 알고리즘
void FCFS() {

	Element * i;

	i = (Element*)malloc(sizeof(Element));
	initElement(i);

	// ready_queue에 처리할 프로세스 없는 경우 종료
	if (ready_queue[2].count == 0) {
		UseCpuQueue = -1;
		current_process = NULL;
		checkNextProcess();
		return;
	}

	UseCpuQueue = 2;
	i = pop(&ready_queue[2]);
	current_process = i;

	while (1) {
		// 원래부터 burst_time이 0인 경우, 저장 후 return
		if (current_process->burst_time == 0) {
			current_process->finish_time = current_time;
			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;


			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			return;
		}

		current_process->burst_time--;
		current_time++;

		// 일이 끝나지 않은 경우, ready_queue 선점여부를 판단
		Element * stored_process = copyElement(current_process);
		if (current_process->burst_time != 0) {
			current_process = NULL;
			checkNextProcess();

			// 선점이 되면 기존의 process를 queue에 삽입한 후 다른 queue에서의 작업 실행
			if (current_process != NULL) {
				if (current_process->priority != 3) {
					stored_process->arrival_time = current_time;
					push(&ready_queue[2], *stored_process);
					bubbleSort(&ready_queue[2]);
					UseCpuQueue = -1;
					current_process = NULL;
					return;
				}
			}

			// 선점 되지 않으면 계속 해당 queue에서 작업 실행
			else {
				current_process = stored_process;
			}
		}

		// 일이 끝난 경우 결과 값 저장 후, 새로운 ready_queue 탐색
		else {
			current_process->finish_time = current_time;
			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;

			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			return;
		}
	}
}