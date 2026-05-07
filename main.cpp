#include "domain.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const double dt             = 0.005;
const int    num_steps      = 2000;
const int    write_interval = 50;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int id, p;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    srand(time(NULL) + id * 10);

    CParticle::create_datatype();
    distribute_initial(id, p);

    for (int step = 0; step < num_steps; step++)
    {
        CParticle* current = CParticle::start;
        while (current != nullptr)
        {
            current->timestep(dt);
            current = current->next;
        }

        migrate_particles(id, p);

        if (step % write_interval == 0)
            write_particles(id, step);
    }

    int local_count = 0;
    CParticle* current = CParticle::start;
    while (current != nullptr) { local_count++; current = current->next; }

    int global_count = 0;
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (id == 0)
        cout << "total particles: " << global_count << " (expected " << max_particles << ")" << endl;

    current = CParticle::start;
    while (current != nullptr)
    {
        CParticle* next = current->next;
        delete current;
        current = next;
    }

    MPI_Type_free(&CParticle::MPI_type);
    MPI_Finalize();
}
