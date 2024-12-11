#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS 1000


typedef struct {
    int digits[MAX_DIGITS];
    int length;
} LongInt;

void initLongInt(LongInt* num, const char* str) {
    num->length = strlen(str);
    for (int i = 0; i < num->length; i++) {
        num->digits[i] = str[num->length - 1 - i] - '0';
    }
}

void printLongInt(const LongInt* num) {
    for (int i = num->length - 1; i >= 0; i--) {
        printf("%d", num->digits[i]);
    }
    printf("\n");
}

void multiplyByDigit(const LongInt* a, int digit, LongInt* result) {
    int carry = 0;
    result->length = a->length;

    for (int i = 0; i < a->length; i++) {
        int prod = a->digits[i] * digit + carry;
        result->digits[i] = prod % 10;
        carry = prod / 10;
    }

    if (carry > 0) {
        result->digits[result->length] = carry;
        result->length++;
    }
}

void addLongInts(const LongInt* a, const LongInt* b, LongInt* result) {
    int maxLen = (a->length > b->length) ? a->length : b->length;
    int carry = 0;
    result->length = maxLen;

    for (int i = 0; i < maxLen; i++) {
        int sum = carry;
        if (i < a->length) sum += a->digits[i];
        if (i < b->length) sum += b->digits[i];
        
        result->digits[i] = sum % 10;
        carry = sum / 10;
    }

    if (carry > 0) {
        result->digits[result->length] = carry;
        result->length++;
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: %s <number1> <number2>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    LongInt num1, num2, result, partial_result, final_result;
    
    if (rank == 0) {
        initLongInt(&num1, argv[1]);
        initLongInt(&num2, argv[2]);
        
        printf("Number 1: ");
        printLongInt(&num1);
        printf("Number 2: ");
        printLongInt(&num2);
    }

    MPI_Bcast(&num1.length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(num1.digits, num1.length, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(&num2.length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(num2.digits, num2.length, MPI_INT, 0, MPI_COMM_WORLD);

    int digits_per_proc = num2.length / size;
    int start_digit = rank * digits_per_proc;
    int end_digit = (rank == size - 1) ? num2.length : start_digit + digits_per_proc;

    memset(&partial_result, 0, sizeof(LongInt));
    partial_result.length = 1;

    for (int i = start_digit; i < end_digit; i++) {
        LongInt temp;
        multiplyByDigit(&num1, num2.digits[i], &temp);
        
        LongInt shifted;
        memset(&shifted, 0, sizeof(LongInt));
        for (int j = 0; j < temp.length; j++) {
            shifted.digits[j + i] = temp.digits[j];
        }
        shifted.length = temp.length + i;

        LongInt new_partial;
        addLongInts(&partial_result, &shifted, &new_partial);
        partial_result = new_partial;
    }

    if (rank == 0) {
        final_result = partial_result;
        for (int i = 1; i < size; i++) {
            LongInt received_result;
            MPI_Recv(&received_result, sizeof(LongInt), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            LongInt temp;
            addLongInts(&final_result, &received_result, &temp);
            final_result = temp;
        }
        
        printf("Result: ");
        printLongInt(&final_result);
    } else {
        MPI_Send(&partial_result, sizeof(LongInt), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}