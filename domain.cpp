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
                CParticle* copy = new CParticle;
                copy->v[0] = current->v[0]; copy->v[1] = current->v[1];
                copy->x[0] = current->x[0]; copy->x[1] = current->x[1];
                copy->next = self_list;
                copy->prev = nullptr;
                if (self_list != nullptr) self_list->prev = copy;
                self_list = copy;
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

void migrate_particles(int id, int p)
{
    vector<vector<CParticle>> staging(p);

    CParticle* current = CParticle::start;
    while (current != nullptr)
    {
        CParticle* next_particle = current->next;
        int dest = owner_of(current->x[0], p);

        if (dest != id)
        {
            CParticle copy;
            copy.v[0] = current->v[0]; copy.v[1] = current->v[1];
            copy.x[0] = current->x[0]; copy.x[1] = current->x[1];
            staging[dest].push_back(copy);

            if (current->prev != nullptr) current->prev->next = current->next;
            if (current->next != nullptr) current->next->prev = current->prev;
            if (current == CParticle::start) CParticle::start = current->next;
            delete current;
        }

        current = next_particle;
    }

    vector<int> send_counts(p, 0), recv_counts(p, 0);
    for (int r = 0; r < p; r++)
        send_counts[r] = static_cast<int>(staging[r].size());

    MPI_Alltoall(send_counts.data(), 1, MPI_INT,
                 recv_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);
}
