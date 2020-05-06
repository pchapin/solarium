
import LinearAlgebra: norm

#### Constants

const OBJECT_COUNT = 1000 # Number of simulated objects.
const TIME_STEP    = 3600 # Time step in seconds.
const G  = 6.673E-11      # Universal gravitational constant.
const ME = 5.97220E+24    # Mass of the Earth
const MS = 1.98892E+30    # Mass of the Sun

#### Types

mutable struct Dynamics
    position :: Array{Float64, 1}
    velocity :: Array{Float64, 1}
end

struct Object
    mass :: Float64
end

#### Functions

function time_step(current_dynamics, next_dynamics, objects)

    # For every object in the universe...
    for i = 1:OBJECT_COUNT
        total_force = zeros(3)

        # Compute the total force on object #i
        for j = 1:OBJECT_COUNT
            if i == j; continue; end
            displacement = current_dynamics[j].position - current_dynamics[i].position
            distance = norm(displacement)
            force_magnitude = (G * objects[i].mass * objects[j].mass)/(distance * distance)
            force = (force_magnitude/distance) * displacement
            total_force = total_force + force
        end

        # Compute the new velocity and position at the end of this time step.
        acceleration = total_force/objects[i].mass
        delta_position = TIME_STEP * current_dynamics[i].velocity
        delta_v = TIME_STEP * acceleration
        next_dynamics[i].position = current_dynamics[i].position + delta_position
        next_dynamics[i].velocity = current_dynamics[i].velocity + delta_v
    end

    # Swap the dynamics arrays.
    temp                             = current_dynamics[1:OBJECT_COUNT]
    current_dynamics[1:OBJECT_COUNT] = next_dynamics[1:OBJECT_COUNT]
    next_dynamics[1:OBJECT_COUNT]    = temp
end

#### Main Program

# Initialize the system (in a truely hacked way).
objects          = fill(Object(ME), OBJECT_COUNT)
current_dynamics = fill(Dynamics(fill(0.0, 3), fill(0.0, 3)), OBJECT_COUNT)
next_dynamics    = fill(Dynamics(zeros(3), zeros(3)), OBJECT_COUNT)

# Execute on time step.
time_step(current_dynamics, next_dynamics, objects)

print(current_dynamics)
