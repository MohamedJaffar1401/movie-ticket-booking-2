#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 50                 /* max pending requests in the queue */
#define NUM_MOVIES 3
#define ROWS 2                 /* rows A and B */
#define COLS 10                /* seats 1 to 10 in every row */
#define SEATS_PER_MOVIE (ROWS * COLS)
#define MAX_PER_BOOKING 6
#define MAX_CONFIRMED (NUM_MOVIES * SEATS_PER_MOVIE)

struct Movie {
    char name[30];
    int availableSeats;
    int seatMap[SEATS_PER_MOVIE];  /* 0 = free, otherwise the booking ID that holds it */
};

struct Booking {
    int id;
    char name[30];
    int movieIndex;            /* which movie this booking is for */
    int seats;                 /* how many seats */
    int seatNo[MAX_PER_BOOKING]; /* the exact seats the customer chose (0-based) */
};

struct Movie movies[NUM_MOVIES] = {
    {"Neon Harbor",      SEATS_PER_MOVIE, {0}},
    {"The Last Lantern", SEATS_PER_MOVIE, {0}},
    {"Paper Moons",      SEATS_PER_MOVIE, {0}}
};

struct Booking queue[MAX];
struct Booking stack[MAX_CONFIRMED];   /* a booking needs at least 1 seat */
int front = 0, rear = -1, count = 0;
int top = -1;

void flushLine() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int readInt() {
    int n;
    if (scanf("%d", &n) != 1) {
        flushLine();
        return -1;
    }
    return n;
}

void printSeatList(struct Booking *b) {
    int i;
    for (i = 0; i < b->seats; i++)
        printf("%c%d%s", 'A' + b->seatNo[i] / COLS, b->seatNo[i] % COLS + 1,
               i < b->seats - 1 ? ", " : "");
}

void displayMovies() {
    int i;
    printf("No.\tMovie\t\t\tSeats left\n");
    for (i = 0; i < NUM_MOVIES; i++)
        printf("%d\t%-18s\t%d\n", i + 1, movies[i].name,
               movies[i].availableSeats);
}

/* Draw the seat map of one movie. XX means the seat is taken. */
void displaySeats(int m) {
    int r, c, idx;
    printf("\nSeats for %s\n", movies[m].name);
    printf("      ======== SCREEN ========\n");
    for (r = 0; r < ROWS; r++) {
        printf("  ");
        for (c = 0; c < COLS; c++) {
            idx = r * COLS + c;
            if (movies[m].seatMap[idx] != 0)
                printf("[XX ]");
            else
                printf("[%c%-2d]", 'A' + r, c + 1);
        }
        printf("\n");
    }
    printf("  XX = taken\n\n");
}

int idInUse(int id) {
    int i, pos;
    for (i = 0; i < count; i++) {
        pos = (front + i) % MAX;
        if (queue[pos].id == id) return 1;
    }
    for (i = 0; i <= top; i++)
        if (stack[i].id == id) return 1;
    return 0;
}

void requestBooking() {
    struct Booking b;
    struct Movie *m;
    int choice, n, i, j, idx, col, dup;
    char row;
    if (count == MAX) {
        printf("Booking queue is full!\n");
        return;
    }
    printf("Enter Booking ID: ");
    b.id = readInt();
    if (b.id <= 0) {
        printf("Invalid Booking ID!\n");
        return;
    }
    if (idInUse(b.id)) {
        printf("That Booking ID is already in use!\n");
        return;
    }
    printf("Enter name: ");
    scanf("%29s", b.name);
    displayMovies();
    printf("Choose movie number: ");
    choice = readInt();
    if (choice < 1 || choice > NUM_MOVIES) {
        printf("Invalid movie!\n");
        return;
    }
    b.movieIndex = choice - 1;
    m = &movies[b.movieIndex];
    if (m->availableSeats == 0) {
        printf("Sorry, %s is sold out.\n", m->name);
        return;
    }

    displaySeats(b.movieIndex);
    printf("How many seats (1-%d)? ", MAX_PER_BOOKING);
    n = readInt();
    if (n < 1 || n > MAX_PER_BOOKING || n > m->availableSeats) {
        printf("Invalid number of seats! (%d free)\n", m->availableSeats);
        return;
    }

    /* The customer chooses each seat. */
    b.seats = 0;
    while (b.seats < n) {
        printf("Pick seat %d of %d (for example A3): ", b.seats + 1, n);
        if (scanf(" %c%d", &row, &col) != 2) {
            if (feof(stdin)) return;
            flushLine();
            printf("Type a seat like A3.\n");
            continue;
        }
        row = toupper((unsigned char)row);
        if (row < 'A' || row >= 'A' + ROWS || col < 1 || col > COLS) {
            printf("There is no seat %c%d.\n", row, col);
            continue;
        }
        idx = (row - 'A') * COLS + (col - 1);
        if (m->seatMap[idx] != 0) {
            printf("Seat %c%d is already taken. Pick another.\n", row, col);
            continue;
        }
        dup = 0;
        for (j = 0; j < b.seats; j++)
            if (b.seatNo[j] == idx) dup = 1;
        if (dup) {
            printf("You already picked %c%d.\n", row, col);
            continue;
        }
        b.seatNo[b.seats++] = idx;
    }

    /* Hold the chosen seats so nobody else can pick them while we wait. */
    for (i = 0; i < n; i++)
        m->seatMap[b.seatNo[i]] = b.id;
    m->availableSeats -= n;

    rear = (rear + 1) % MAX;
    queue[rear] = b;
    count++;
    printf("Booking request added to queue! Seats held: ");
    printSeatList(&b);
    printf("\n");
}

void displayQueue() {
    int i, pos;
    if (count == 0) {
        printf("No pending booking requests.\n");
        return;
    }
    printf("ID\tName\tMovie\t\tSeats\n");
    for (i = 0; i < count; i++) {
        pos = (front + i) % MAX;
        printf("%d\t%s\t%s\t", queue[pos].id, queue[pos].name,
               movies[queue[pos].movieIndex].name);
        printSeatList(&queue[pos]);
        printf("\n");
    }
}

void confirmBooking() {
    struct Booking b;
    if (count == 0) {
        printf("No pending booking requests.\n");
        return;
    }
    b = queue[front];                  /* dequeue from the front */
    front = (front + 1) % MAX;
    count--;

    top++;                             /* push onto the stack */
    stack[top] = b;
    printf("Booking confirmed for %s (ID %d) - %s, seats ", b.name, b.id,
           movies[b.movieIndex].name);
    printSeatList(&b);
    printf("\n");
}

void displayConfirmed() {
    int i;
    if (top == -1) {
        printf("No confirmed bookings.\n");
        return;
    }
    printf("ID\tName\tMovie\t\tSeats\n");
    for (i = top; i >= 0; i--) {
        printf("%d\t%s\t%s\t", stack[i].id, stack[i].name,
               movies[stack[i].movieIndex].name);
        printSeatList(&stack[i]);
        printf("\n");
    }
}

void cancelLast() {
    int i;
    struct Movie *m;
    if (top == -1) {
        printf("No confirmed bookings to cancel.\n");
        return;
    }
    /* The exact seats go back to that booking's own movie only. */
    m = &movies[stack[top].movieIndex];
    for (i = 0; i < stack[top].seats; i++)
        m->seatMap[stack[top].seatNo[i]] = 0;
    m->availableSeats += stack[top].seats;
    printf("Cancelled booking of %s (ID %d) - %s, seats ", stack[top].name,
           stack[top].id, m->name);
    printSeatList(&stack[top]);
    printf(" are free again.\n");
    top--;
}

void searchBooking() {
    int id, i, pos, found = 0;
    printf("Enter Booking ID to search: ");
    id = readInt();
    for (i = 0; i < count; i++) {
        pos = (front + i) % MAX;
        if (queue[pos].id == id) {
            printf("Found in pending queue: %s, %s, seats ",
                   queue[pos].name, movies[queue[pos].movieIndex].name);
            printSeatList(&queue[pos]);
            printf("\n");
            found = 1;
        }
    }
    for (i = 0; i <= top; i++) {
        if (stack[i].id == id) {
            printf("Found in confirmed list: %s, %s, seats ",
                   stack[i].name, movies[stack[i].movieIndex].name);
            printSeatList(&stack[i]);
            printf("\n");
            found = 1;
        }
    }
    if (found == 0)
        printf("Booking ID not found.\n");
}

int main() {
    int choice;
    while (1) {
        printf("\n--- MOVIE TICKET BOOKING SYSTEM ---\n");
        displayMovies();
        printf("1.Request Booking 2.Display Pending 3.Confirm Next\n");
        printf("4.Display Confirmed 5.Cancel Last 6.Search 7.Exit\n");
        printf("Enter choice: ");
        choice = readInt();
        if (feof(stdin)) return 0;
        switch (choice) {
            case 1: requestBooking(); break;
            case 2: displayQueue(); break;
            case 3: confirmBooking(); break;
            case 4: displayConfirmed(); break;
            case 5: cancelLast(); break;
            case 6: searchBooking(); break;
            case 7: printf("Thank you!\n"); return 0;
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}
