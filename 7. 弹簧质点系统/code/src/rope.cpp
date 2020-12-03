#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        Vector2D pos = (end - start) / (num_nodes - 1);
        masses.push_back(new Mass(start, node_mass, false));
        for(int i=1; i<num_nodes; i++){
            masses.push_back(new Mass(start+i*pos, node_mass, false));
            springs.push_back(new Spring(masses[i-1], masses[i], k));        
        }
        for (auto &i : pinned_nodes) {
            masses[i] -> pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            Vector2D direct = s->m1->position - s->m2->position;
            double force = s->k * (s->rest_length - direct.norm());
            s->m1->forces += (force * direct.unit());
            s->m2->forces += (-force * direct.unit());
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                m->forces += gravity;
                double k_d = 0.01;
                m->forces -= (k_d*m->velocity);
                //m->position = m->position + m->velocity * delta_t;
                m->velocity = m->velocity + m->forces / m->mass * delta_t;
                m->position = m->position + m->velocity * delta_t;
            }

            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            Vector2D direct = s->m1->position - s->m2->position;
            double diff_length = s->rest_length - direct.norm();
            direct = direct.unit();
            if(s->m1->pinned){
                s->m2->position = s->m2->position - diff_length * direct;
            }
            else if(s->m2->pinned){
                s->m1->position = s->m1->position + diff_length * direct;
            }
            else{
                s->m1->position = s->m1->position + diff_length * direct / 2;
                s->m2->position = s->m2->position - diff_length * direct / 2;
            }
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;
                double k_d = 0.00005;
                m->position = m->position + (1-k_d) * (m->position - m->last_position) + gravity / m->mass * delta_t * delta_t;
                m->last_position = temp_position;
            }
        }
    }
}
