#include "particle.h"

MPI_Datatype CParticle::MPI_type;
CParticle* CParticle::start = nullptr;

CParticle::CParticle()
{
    v[0] = v[1] = 0.0;
    x[0] = x[1] = 0.0;
    next = nullptr;
    prev = nullptr;
}

CParticle::~CParticle() {}

void CParticle::create_datatype()
{
    MPI_Aint displacements[2];
    int block_lengths[2];
    MPI_Datatype datatypes[2];

    CParticle temp;

    block_lengths[0] = 2;
    MPI_Get_address(temp.v, &displacements[0]);
    datatypes[0] = MPI_DOUBLE;

    block_lengths[1] = 2;
    MPI_Get_address(temp.x, &displacements[1]);
    datatypes[1] = MPI_DOUBLE;

    MPI_Aint base;
    MPI_Get_address(&temp, &base);
    for (int i = 0; i < 2; i++)
        displacements[i] -= base;

    MPI_Type_create_struct(2, block_lengths, displacements, datatypes, &MPI_type);
    MPI_Type_create_resized(MPI_type, 0, sizeof(CParticle), &MPI_type);
    MPI_Type_commit(&MPI_type);
}
