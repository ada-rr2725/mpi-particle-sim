#include "particle.h"

int owner_of(double x_pos, int p);
void distribute_initial(int id, int p);
void migrate_particles(int id, int p);
void write_particles(int id, int step);
