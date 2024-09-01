#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define PROC_FILE "/proc/pq_basic"

void write_proc(int fd, const char *data, size_t len) {
    if (write(fd, data, len) < 0) {
        perror("Failed to write to /proc file");
        exit(EXIT_FAILURE);
    }
}

void read_proc(int fd) {
    char buffer[4];
    int32_t value;

    ssize_t len;

    lseek(fd, 0, SEEK_SET); // Reset file offset to the beginning

    len = read(fd, &value, sizeof(value));
    if (len < 0) {
        perror("Failed to read from /proc file");
        exit(EXIT_FAILURE);
    } else if (len == 0) {
        printf("No data read from /proc file\n");
        return;
    }

    printf("Data read from /proc file: %d\n", value);
}

int main() {
    int fd;
    int8_t queue_size;
    int32_t value, priority;
    char data[4];
    int choice;

    // Open the /proc file
    fd = open(PROC_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open /proc file");
        exit(EXIT_FAILURE);
    }

    // Ask for the queue size before showing the menu
    printf("Enter queue size (1 to 100): ");
    scanf("%hhd", &queue_size);
    if (queue_size < 1 || queue_size > 100) {
        printf("Invalid queue size. Exiting.\n");
        close(fd);
        return 1;
    }
    data[0] = queue_size;
    write_proc(fd, data, sizeof(int8_t));

    while (1) {
        printf("\nInteractive Test for Priority Queue LKM\n");
        printf("1. Push Value\n");
        printf("2. Pop Value\n");
        printf("3. Read Top Value\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to push: ");
                scanf("%d", &value);
                printf("Enter priority for the value: ");
                scanf("%d", &priority);

                // Set write_state
                write_proc(fd, "push", strlen("push"));
                memcpy(data, &value, sizeof(int32_t));
                write_proc(fd, data, sizeof(int32_t));
                memcpy(data, &priority, sizeof(int32_t));
                write_proc(fd, data, sizeof(int32_t));
                break;

            case 2:
                // Set read_state to pop
                write_proc(fd, "pop", strlen("pop"));
                read_proc(fd);
                break;

            case 3:
                // Set read_state to read top
                write_proc(fd, "top", strlen("top"));
                read_proc(fd);
                break;

            case 4:
                close(fd);
                exit(EXIT_SUCCESS);

            default:
                printf("Invalid choice, please try again.\n");
                break;
        }
    }

    return 0;
}

