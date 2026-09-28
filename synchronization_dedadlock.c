/*
 * College Printer Server Simulation
 * ---------------------------------
 * 3 student threads, 2 printers, 1 shared print queue.
 *
 * Synchronization used:
 *   - Counting semaphore (printers_sem, initial value 2)
 *         -> counts free printers; a student blocks when none are free.
 *   - Mutex (queue_mutex)
 *         -> only ONE thread at a time may modify the print queue
 *            or the printer-status table.
 *   - Mutex (log_mutex)
 *         -> keeps console lines from mixing.
 *
 * Deadlock prevention:
 *   1. Each student needs only ONE resource (a printer), so there is no
 *      circular wait between different resources.
 *   2. No hold-and-wait: a thread NEVER holds queue_mutex while blocking on
 *      the semaphore. It waits on the semaphore first, and takes the mutex
 *      only for short, non-blocking critical sections.
 *   3. Fixed lock ordering everywhere: semaphore -> queue_mutex -> log_mutex.
 *   4. A printer is always released (state cleared, then sem_post), so a
 *      waiting student is guaranteed to be woken up eventually.
 *
 * Compile:  gcc -Wall -pthread printer_server.c -o printer_server
 * Run:      ./printer_server
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_STUDENTS 3
#define NUM_PRINTERS 2
#define QUEUE_SIZE   10

/* ---------- Hardcoded student data ---------- */
typedef struct {
    int id;              /* Student number                                */
    int preferred;       /* Requested printer (0 = any available printer) */
    int start_delay_ms;  /* When the student arrives                      */
    int print_time_s;    /* How long the print job takes                  */
} Student;

static Student students[NUM_STUDENTS] = {
    /* id, preferred, start_delay_ms, print_time_s */
    { 1, 1, 0,   3 },
    { 2, 2, 100, 5 },
    { 3, 0, 300, 4 }
};

/* ---------- Shared resources ---------- */
static int print_queue[QUEUE_SIZE];          /* shared print queue (student ids) */
static int queue_count = 0;
static int printer_owner[NUM_PRINTERS + 1];  /* index 1..2; 0 = free             */
static int release_count = 0;                /* only used for output grouping    */

static sem_t printers_sem;                   /* counts free printers             */
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t log_mutex   = PTHREAD_MUTEX_INITIALIZER;
static int last_phase = 0;

/* ---------- Helpers ---------- */
static void msleep(int ms) { usleep(ms * 1000); }

/* Thread-safe print; inserts a blank line when the output "phase" changes. */
static void log_msg(int phase, const char *fmt, ...)
{
    va_list args;
    pthread_mutex_lock(&log_mutex);
    if (last_phase != 0 && phase != last_phase)
        printf("\n");
    last_phase = phase;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
    pthread_mutex_unlock(&log_mutex);
}

/* Queue operations: caller MUST hold queue_mutex. */
static void queue_add(int id)
{
    if (queue_count < QUEUE_SIZE)
        print_queue[queue_count++] = id;
}

static void queue_remove(int id)
{
    for (int i = 0; i < queue_count; i++) {
        if (print_queue[i] == id) {
            for (int j = i; j < queue_count - 1; j++)
                print_queue[j] = print_queue[j + 1];
            queue_count--;
            return;
        }
    }
}

/* Pick a printer: preferred one if free, otherwise the first free one.
 * Caller MUST hold queue_mutex and must already own a semaphore token,
 * so at least one printer is guaranteed to be free. */
static int pick_printer(int student_id, int preferred)
{
    int chosen = 0;

    if (preferred >= 1 && preferred <= NUM_PRINTERS && printer_owner[preferred] == 0) {
        chosen = preferred;
    } else {
        for (int p = 1; p <= NUM_PRINTERS; p++) {
            if (printer_owner[p] == 0) { chosen = p; break; }
        }
    }
    printer_owner[chosen] = student_id;
    return chosen;
}

/* ---------- Student thread ---------- */
static void *student_thread(void *arg)
{
    Student *s = (Student *)arg;
    int waited = 0;
    int printer;

    msleep(s->start_delay_ms);                       /* student arrives */

    /* 1. Request */
    if (s->preferred)
        log_msg(1, "Student %d requested Printer %d\n", s->id, s->preferred);

    pthread_mutex_lock(&queue_mutex);                /* enter print queue */
    queue_add(s->id);
    pthread_mutex_unlock(&queue_mutex);

    /* 2. Try to get a printer WITHOUT holding any mutex (prevents deadlock) */
    if (sem_trywait(&printers_sem) != 0) {
        /* Resource unavailable: all printers are busy */
        log_msg(1, "Student %d waiting...\n", s->id);
        waited = 1;
        sem_wait(&printers_sem);                     /* block until a printer is freed */
    }

    /* 3. Allocate a specific printer and leave the queue (short critical section) */
    pthread_mutex_lock(&queue_mutex);
    queue_remove(s->id);
    printer = pick_printer(s->id, s->preferred);
    pthread_mutex_unlock(&queue_mutex);

    /* Simulated spooler set-up time (only for immediate grants) */
    if (!waited)
        msleep(1000);

    if (waited)
        log_msg(3, "Student %d allocated Printer %d\n", s->id, printer);
    else
        log_msg(2, "Printer %d allocated to Student %d\n", printer, s->id);

    /* 4. Print the document */
    sleep(s->print_time_s);

    /* 5. Release the printer: clear state first, then signal the semaphore */
    int phase;
    pthread_mutex_lock(&queue_mutex);
    printer_owner[printer] = 0;
    phase = (release_count++ == 0) ? 3 : 4;
    log_msg(phase, "Student %d released Printer %d\n", s->id, printer);
    pthread_mutex_unlock(&queue_mutex);

    sem_post(&printers_sem);                         /* wakes a waiting student */
    return NULL;
}

/* ---------- Main ---------- */
int main(void)
{
    pthread_t threads[NUM_STUDENTS];

    sem_init(&printers_sem, 0, NUM_PRINTERS);        /* 2 printers available */

    for (int i = 0; i < NUM_STUDENTS; i++)
        pthread_create(&threads[i], NULL, student_thread, &students[i]);

    for (int i = 0; i < NUM_STUDENTS; i++)
        pthread_join(threads[i], NULL);

    log_msg(4, "All print jobs completed. No deadlock occurred.\n");

    sem_destroy(&printers_sem);
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&log_mutex);
    return 0;
}