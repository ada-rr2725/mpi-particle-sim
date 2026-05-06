#include "domain.h"
#include <vector>

using namespace std;

int owner_of(double x_pos, int p)
{
    int owner = static_cast<int>(x_pos * p / x_max);
    if (owner >= p) owner = p - 1;
    if (owner < 0)  owner = 0;
    return owner;
}

void distribute_initial(int id, int p)
{
    if (id == 0)
    {
        for (int i = 0; i < max_particles; i++)
        {
            CParticle* item = new CParticle;
            item->setup_random();
            item->next = CParticle::start;
            if (CParticle::start != nullptr)
                CParticle::start->prev = item;
            CParticle::start = item;
        }

        CParticle* current = CParticle::start;
        CParticle* self_list = nullptr;

        while (current != nullptr)
        {
            CParticle* next_particle = current->next;
            int dest = owner_of(current->x[0], p);

            if (dest == 0)
            {
                current->next = self_list;
                if (self_list != nullptr) self_list->prev = current;
                current->prev = nullptr;
                self_list = current;
            }
            else
            {
                MPI_Send(current, 1, CParticle::MPI_type, dest, 0, MPI_COMM_WORLD);
            }

            current = next_particle;
        }

        current = CParticle::start;
        while (current != nullptr)
        {
            CParticle* next_particle = current->next;
            delete current;
            current = next_particle;
        }

        CParticle::start = self_list;

        for (int i = 1; i < p; i++)
            MPI_Send(nullptr, 0, CParticle::MPI_type, i, 0, MPI_COMM_WORLD);
    }
    else
    {
        while (true)
        {
            CParticle* temp = new CParticle;
            MPI_Status status;
            MPI_Recv(temp, 1, CParticle::MPI_type, 0, 0, MPI_COMM_WORLD, &status);

            int num_received;
            MPI_Get_count(&status, CParticle::MPI_type, &num_received);

            if (num_received == 0)
            {
                delete temp;
                break;
            }

            temp->next = CParticle::start;
            if (CParticle::start != nullptr)
                CParticle::start->prev = temp;
            CParticle::start = temp;
        }
    }
}
