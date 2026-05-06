#include <mpi.h>

const double x_max = 5.0;
const double y_max = 5.0;
const double gravity = 9.81;
const int max_particles = 10000;

class CParticle
{
public:
    CParticle();
    ~CParticle();

    double v[2];
    double x[2];

    CParticle* next, * prev;

    void setup_random();
    void timestep(double dt);

    static MPI_Datatype MPI_type;
    static void create_datatype();

    static CParticle* start;
};
