#include<mpi.h>
#include <iostream>

int main(int argc, char *agrv[]){
    int id, size, i, val_send, val_rec;

    MPI_Status st;
    MPI_Init(&argc, &agrv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (id == 0) {
        val_send = 100;
        MPI_Send(&val_send, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Recv(&val_rec, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &st);
        std::cout << "id " << id << " enviou " << val_send << " e recebeu " << val_rec << std::endl;
    } else if (id == 1) {
        val_send = 8008;
        MPI_Send(&val_send, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Recv(&val_rec, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &st);
        std::cout << "id " << id << " enviou " << val_send << " e recebeu " << val_rec << std::endl;
    } else {
        std::cout << "id " << id << " nao enviou nem recebeu mensagem" << std::endl;
    }
    MPI_Finalize();

    return 0;
}