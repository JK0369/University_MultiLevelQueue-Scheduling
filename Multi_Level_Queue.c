#include <stdio.h>
#include <stdlib.h>

/* source program ����
1.Element ����(����ü����)
2.Ready_Queue ����
3.�������� ����
4.bubbleSort ���� (arrival_time�� ����ϱ� ���� �ʿ��� �����Լ�)
5.I/O �����Լ�
6.�˰��� ����(��ü���� �帧�� main�� / RR,SJF,FCFS ����)
*/

/*        1.Element ����         */
// Element : ���μ����� ������ ������ ����
// queue���� ���Ұ� �� Element Ÿ�� (����, ���� ���꿡 ������ linked-list�� ����)
typedef struct Element {
	int process_number;
	int priority;
	int arrival_time;
	int burst_time;
	int finish_time;
	int waiting_time;

	int burst_time_init; // �ʱ� burst time ���� (scheduling�� waiting_time ���� �� ���)
	int arrival_time_init; // �ʱ� arrival time (scheduling�� waiting_time ���� �� ���)

	struct Element * next; // linked-list ����
}Element;

// ���μ��� ���� �ʱ�ȭ �Լ�
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

// ���� ���μ��� ������ �Ȱ��� �����ؼ� ��ȯ�ϴ� �Լ�
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

/*        2.Ready_Queue ����         */
// linked-list�� ������ ready_queue ����ü
typedef struct {
	Element * front;
	Element * rear;
	int count;
}Ready_Queue;

// Ready_Queue �ʿ��� �Լ�
// format �Լ��� 4���� ���μ��� ������ �޾Ƽ� Element ��ҷ� ��ȯ�ϴ� �Լ�
Element format(int process_number, int priority, int arrival_time, int burst_time);
void initQueue(Ready_Queue * queue);
int isEmpty(Ready_Queue * queue);
void push(Ready_Queue * queue, Element e);
Element* pop(Ready_Queue * queue);
Element* popObject(Ready_Queue * queue, Element * e);

// ���� �Է� ���� ��, 4���� ������ �޾Ƽ� �����ϴ� �Լ�
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

	// queue�� ��������� �� �տ� e ����, �׷��� ������ ���� �� ���� ������ e ����.
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

	Element * front_free = queue->front; // �����ϱ� ���� �ּ� ����

	Element * front_tmp = (Element*)malloc(sizeof(Element));
	initElement(front_tmp);
	front_tmp = copyElement(queue->front); // ��ȯ�ϱ� ���� �� ����

	front_tmp->next = NULL;

	// ���Ұ� �ϳ��ۿ� ���� ��� rear�� NULL������ ����
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

// Ư�� ��ġ pop ����
Element* popObject(Ready_Queue * queue, Element * e) {
	if (isEmpty(queue)) return NULL;

	Element * tmp;
	Element * pre = queue->front;

	// �����Ϸ��� ��ġ Ž��
	for (tmp = queue->front; tmp != e; tmp = tmp->next) {}

	// �ٷ� ������ ��� Ž��
	while (1) {
		if (pre == NULL) break;
		// pre�� ���� ã�� ���
		if (pre->next == e) {
			break;
		}
		pre = pre->next;
	}

	// �� ó�� ��Ҹ� pop�ϴ� ���
	if (queue->front == tmp) {
		return pop(queue);
	}
	// �� ������ ��Ҹ� pop�ϴ� ���
	else if (queue->rear == tmp) {
		queue->rear = pre;
		pre->next = NULL;
		queue->count--;
		return tmp;
	}
	// �� ó��, �� ������ ��Ұ� �ƴ� �߰� ��Ҹ� pop�ϴ� ���
	else {
		pre->next = tmp->next;
		queue->count--;
		return tmp;
	}
	return NULL;
}

/*        3.�������� ����         */

#define MAX_SIZE 70000	   // ���μ��� ������� �� �� �ִ� ũ��
int current_time = 0;		   // ���� �����층 ó�� ���� �ð�
Ready_Queue ready_queue[3];	      // multilevel queue, [0]�� RR, [1]�� SJF, [2]�� FCFS
Element * current_process;	  // start ���μ����� ã������ �������� ����
int process_waiting_time[MAX_SIZE];	  // ���μ��� ���� waiting_time ������� ����� ����
int UseCpuQueue = -1;		  // cpu�� ���� � queue�� ����ϴ��� üũ

							  /*        4.bubbleSort : queue���� �����͸� arrival_time ���� �������� ����           */
void bubbleSort(Ready_Queue * queue) {

	//queue�� ����ְų� ������ �Ѱ��� ��� : ���� ���ص� ��
	if (isEmpty(queue)) return;
	if (queue->count == 1) return;

	int i;
	Element * j;

	// queue�� �����Ͱ� 2�� �̻� ��� : ����
	for (i = 0; i < queue->count; i++) {
		for (j = queue->front; j->next != NULL; j = j->next) {
			if (j->arrival_time > j->next->arrival_time) {

				// swap�ϱ� ����, �ٷ� ������ ��� Ž��
				Element * pre = queue->front;
				while (1) {
					// ���� ��Ұ� front�� ���
					if (pre == j) {
						pre = NULL;
						break;
					}

					// pre�� ���� ã�� ���
					if (pre->next == j) {
						break;
					}

					pre = pre->next;
				}

				// swap�ϱ� ����, �ٷ� ������ ���� ��� Ž��
				Element * post = j->next->next;

				// �ٲٷ��� ù ��° ������
				Element * current = (Element*)malloc(sizeof(Element));
				current = copyElement(j);

				// �ٲٷ��� �� ��° ������
				Element * current_next = (Element*)malloc(sizeof(Element));
				current_next = copyElement(j->next);

				// ���� ��Ұ� rear�� ��� rear�� ����
				if (queue->rear == j->next) {
					queue->rear = current;
				}

				//���� ��Ұ� front�� ���
				if (j == queue->front) {

					queue->front = current_next;
					current_next->next = current;
					current->next = post;
					j = current_next;
				}
				// �ٷ� ���� ��Ұ� front�� �ƴϰ� ���� ��Ұ� front�� �ƴ� ���
				else if (pre != queue->front && pre != NULL) {

					pre->next = current_next;
					current_next->next = current;
					current->next = post;
					j = current_next;
				}

				// �ٷ� ���� ��Ұ� front�� ���
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

/*        5.I/O �۾�           */
int duplicateCheck[MAX_SIZE] = { 0 }; // process number�� �ߺ��� ��츦 check �ϴ� ����

int fileOpen() {
	int process_number = -1;
	int priority = -1;
	int arrival_time = -1;
	int burst_time = -1;

	FILE * fp = fopen("process.in", "r");

	if (fp == NULL) {
		printf("������ �������� ���� = ����\n");
		exit(1);
	}

	while (1) {
		int ret = fscanf(fp, "%d %d %d %d",
			&process_number,
			&priority,
			&arrival_time,
			&burst_time);
		if (ret == EOF) break;

		// ���� ó�� -> �� ���� �Է��� ������ ���
		if (process_number == -1 || priority == -1 || arrival_time == -1 || burst_time == -1) {
			printf("������ �߸��� ���� = ����\n");
			exit(1);
		}

		// ���� ó�� -> process_number�� ������ ���
		if (process_number < 0) {
			printf("process_number�� ���� (�߸��Է�) = ����\n");
			exit(1);
		}

		// ���� ó�� -> priority�� 1~3�� �ƴ� ���
		if (priority < 0 || priority > 3) {
			printf("priority�� �������� ��� = ����\n");
			exit(1);
		}

		// ���� ó�� -> arrival_time�� ������ ���.
		if (arrival_time < 0) {
			printf("arrival_time�� ������ = ����\n");
			exit(1);
		}

		// ���� ó�� -> burst_time�� ������ ���.
		if (burst_time < 0) {
			printf("burst_time�� ������ = ����\n");
			exit(1);
		}

		// ���� ó�� -> process_number�� �ߺ��� ���.
		if (duplicateCheck[process_number] != 0) {
			printf("process_number �ߺ� = ����\n");
			exit(1);
		}

		// ���μ��� ���� ����
		Element e;
		e = format(process_number, priority, arrival_time, burst_time);
		duplicateCheck[process_number] = 1;

		// ready_queue�� ���μ��� ���� ����
		push(&ready_queue[priority - 1], e);

		// ���� ó���� ���ؼ� �ʱ�ȭ �۾�
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

/*        6.�˰���           */
void execute();
int isEndScheduling();
void checkNextProcess();
void RR();
void SJF();
void FCFS();

int main(void) {

	// ���������� �ִ� ready_queue, current_process �ʱ�ȭ
	for (int i = 0; i < 3; i++) {
		initQueue(&ready_queue[i]);
	}

	// ��� �� -1�� �ʱ�ȭ
	for (int i = 0; i < MAX_SIZE; i++) process_waiting_time[i] = -1;

	current_process = (Element *)malloc(sizeof(Element));
	current_process = NULL;

	// ���� ����
	if (!fileOpen()) {
		printf("������ ������ ����\n");
		return 0;
	}

	// ù ��°�� ������ process Ž��
	checkNextProcess();

	// sheduling ����
	execute();

	// sheduling ��� ���
	printf("process_number\twaiting_time\n");
	for (int i = 0; i < MAX_SIZE; i++) {
		if (process_waiting_time[i] != -1)
			printf("%d\t\t%d\n", i, process_waiting_time[i]);
	}

	return 0;
}

void execute() {
	while (!(isEndScheduling())) { // scheduling �۾��� ���� �� ���� ����
		checkNextProcess();

		// ���� �����ؾ��� ���μ����� NULL�� ��� = ���� ��� ���� ���.
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

// ���� ������ ���μ����� ã�� �Լ�
// 1) ���μ����� ���� ������ ���� ���μ��� Ž��
// 2) ���μ��� ���ϴٰ� ready_queue ���� ���θ� �ľ�
void checkNextProcess() {

	// ���� ������ ���μ����� ã�Ұų�, ó���� ���� ���� ��� ����
	// �켱������ ���� ready_queue���� Ž��
	while (current_process == NULL && !(isEndScheduling())) {
		// FCFS queue�� �켱���� ���� �����Ƿ� ���� ����� �ƴ�
		if (ready_queue[2].count != 0 && UseCpuQueue == -1) {
			if (current_time >= ready_queue[2].front->arrival_time)
				current_process = ready_queue[2].front;
		}

		// SJF queue�� ���� ��� queue�� RR queue�� �ƴϸ� ���� ����� �� �� ����
		// ��, SJF queue ������ ���� ����̹Ƿ�, ���� SJF queue�̸� ���� ����� �ƴ�
		if (ready_queue[1].count != 0 && UseCpuQueue != 0 && UseCpuQueue != 1) {
			Element * i = (Element*)malloc(sizeof(Element));
			initElement(i);
			if (ready_queue[1].front->arrival_time <= current_time)
				current_process = ready_queue[1].front;

			// burst_time�� ���� ���� �� Ž��
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

		// ���� queue�� process�� �ƹ��͵� �������� �ʾҰ� 
		//���� cpu�� scheduling���ϴ� ��� : ���� �ð� ����
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

	// RR �˰��� : ���� +1���� �� burst_time�� 0����(���� ������) üũ, 
	// �������� ������Ʈ�Ѵ�.(��, �����ð� ���)
	// ���� ������ �ʾ����� +1�� �� �� �� ready_queue�� push�� �� ���� process�� �ݺ��Ѵ�.
	while (1) {

		// ready_queue���� ���μ����� ��� ó���� ��� ����
		if (ready_queue[0].count == 0) {
			current_process = NULL;
			UseCpuQueue = -1;
			checkNextProcess();
			break;
		}

		current_process = ready_queue[0].front;
		UseCpuQueue = 0;

		// ���� ���� �ð����� �����ð��� ���� ��� �ٸ� ready queue Ž��
		if (current_process->arrival_time > current_time) {
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			return;
		}

		current_process = pop(&ready_queue[0]);

		// ó�� ���� �� ���� burst_time�� 0�� ���
		if (current_process->burst_time == 0) {
			current_process->finish_time = current_time;
			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;

			// ��� �� ����
			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
			return;
		}

		current_process->burst_time--;
		current_time++;

		// 1�� ���� �� ���� ���� ���
		if (current_process->burst_time == 0) {
			current_process->finish_time = current_time;
			current_process->waiting_time = current_process->finish_time
				- current_process->arrival_time_init - current_process->burst_time_init;

			// ��� �� ����
			process_waiting_time[current_process->process_number]
				= current_process->waiting_time;
			UseCpuQueue = -1;
			current_process = NULL;
			checkNextProcess();
		}
		// ���� ������ ���� ���
		else {
			current_process->burst_time--;
			current_time++;

			// ���� 2���� ���� ���� ���
			if (current_process->burst_time == 0) {
				current_process->finish_time = current_time;
				current_process->waiting_time = current_process->finish_time
					- current_process->arrival_time_init - current_process->burst_time_init;

				// ����� ����
				process_waiting_time[current_process->process_number]
					= current_process->waiting_time;
				UseCpuQueue = -1;
				current_process = NULL;
			}
			// 2�� �Ŀ��� ���� �ȳ��� ��� : queue�� ���� ��, ���� process �ٽ� ����
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

	// ready_queue�� ó���� ���μ��� ���� ��� ����
	if (ready_queue[1].count == 0) {
		UseCpuQueue = -1;
		checkNextProcess();
		return;
	}

	int min_burst = 987654321;
	// queue���� burst_time�� ���� ���� �� Ž�� : current_process ������Ʈ
	for (i = ready_queue[1].front; i != NULL; i = i->next) {
		if (i->arrival_time <= current_time && min_burst > i->burst_time) {
			min_burst = i->burst_time;
			current_process = i;
		}
	}

	// queue���� �ش� ��� pop
	current_process = popObject(&ready_queue[1], current_process);
	UseCpuQueue = 1;

	while (1) {
		// �������� burst_time�� 0�� ���, ���� �� return
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

		// ���� ������ ���� ���, ready_queue �������θ� �Ǵ�
		Element * stored_process = copyElement(current_process);
		if (current_process->burst_time != 0) {
			current_process = NULL;
			checkNextProcess();

			// ������ �Ǹ� ������ process�� queue�� ������ �� 
			// �ٸ� queue������ �۾� ����
			if (current_process != NULL) {
				if (current_process->priority != 2) {
					stored_process->arrival_time = current_time;
					push(&ready_queue[1], *stored_process);
					bubbleSort(&ready_queue[1]);
					UseCpuQueue = -1;
					return;
				}
			}

			// ���� ���� ������ ��� �ش� queue���� �۾� ����
			else {
				current_process = stored_process;
			}
		}

		// ���� ���� ��� ��� �� ���� ��, ���ο� ready_queue Ž��
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

// First Come First Served �˰���
void FCFS() {

	Element * i;

	i = (Element*)malloc(sizeof(Element));
	initElement(i);

	// ready_queue�� ó���� ���μ��� ���� ��� ����
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
		// �������� burst_time�� 0�� ���, ���� �� return
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

		// ���� ������ ���� ���, ready_queue �������θ� �Ǵ�
		Element * stored_process = copyElement(current_process);
		if (current_process->burst_time != 0) {
			current_process = NULL;
			checkNextProcess();

			// ������ �Ǹ� ������ process�� queue�� ������ �� �ٸ� queue������ �۾� ����
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

			// ���� ���� ������ ��� �ش� queue���� �۾� ����
			else {
				current_process = stored_process;
			}
		}

		// ���� ���� ��� ��� �� ���� ��, ���ο� ready_queue Ž��
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