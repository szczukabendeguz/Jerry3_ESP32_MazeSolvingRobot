import sys
from controller import Supervisor
import random
import struct
import math

class GeneticAlgorithmTuner:
    def __init__(self):
        self.supervisor = Supervisor()
        self.time_step = int(self.supervisor.getBasicTimeStep())
        
        # Get robot node
        self.robot_node = self.supervisor.getFromDef("Jerry3")
        self.start_translation = self.robot_node.getField("translation").getSFVec3f()
        self.start_rotation = self.robot_node.getField("rotation").getSFRotation()
        
        self.emitter = self.supervisor.getDevice("emitter")
        self.receiver = self.supervisor.getDevice("receiver")
        self.receiver.enable(self.time_step)
        
        # GA Parameters
        self.pop_size = 10
        self.num_generations = 20
        self.mutation_rate = 0.1
        
        # Chromosome boundaries [min, max]
        self.bounds = [
            [1.0, 10.0],   # Pid_P (JSON: 5.0)
            [0.0, 0.5],    # Pid_I (JSON: 0.1)
            [1.0, 8.0],    # Pid_D (JSON: 4.0)
            [60.0, 60.0],  # turnProportionalSpeed (LOCKED)
            [40.0, 40.0],  # turnMinSpeed (LOCKED)
            [150.0, 150.0],# turnMaxSpeed (LOCKED)
            [80.0, 200.0], # baseSpeed (JSON: 130)
            [100.0, 250.0],# maxMotorSpeed (JSON: 150)
            [-50.0, 50.0], # minMotorSpeed (JSON: -10)
            [310.0, 310.0],# delayBeforeTurn (LOCKED)
            [5.0, 15.0]    # distanceFromSingleWall (JSON: 8.0)
        ]
        
        self.population = self._init_population()

    def _init_population(self):
        pop = []
        # Seed Individual 0 with the known-good competition baseline
        baseline = [5.0, 0.1, 4.0, 60.0, 40.0, 150.0, 130.0, 150.0, -10.0, 310.0, 8.0]
        pop.append(baseline)
        
        for _ in range(self.pop_size - 1):
            ind = [random.uniform(b[0], b[1]) for b in self.bounds]
            pop.append(ind)
        return pop

    def run_simulation(self, individual):
        # Reset simulation
        offset_y = random.uniform(-0.04, 0.04) # Not perfectly centered
        offset_yaw = random.uniform(-0.15, 0.15)
        
        start_t = [self.start_translation[0], self.start_translation[1] + offset_y, self.start_translation[2]]
        start_r = [self.start_rotation[0], self.start_rotation[1], self.start_rotation[2], self.start_rotation[3] + offset_yaw]
        
        self.robot_node.getField("translation").setSFVec3f(start_t)
        self.robot_node.getField("rotation").setSFRotation(start_r)
        self.robot_node.resetPhysics()
        self.supervisor.simulationResetPhysics()
        
        # Send params
        message = struct.pack('11d', *individual)
        self.emitter.send(message)
        
        # Clear any stale collision messages
        while self.receiver.getQueueLength() > 0:
            self.receiver.nextPacket()
            
        # Evaluate for max 30 seconds
        max_steps = int((30.0 * 1000) / self.time_step)
        total_distance = 0.0
        last_pos = None
        collisions = 0
        
        for step in range(max_steps):
            if self.supervisor.step(self.time_step) == -1:
                break
                
            pos = self.robot_node.getPosition()
            
            if last_pos is not None:
                dx = pos[0] - last_pos[0]
                dy = pos[1] - last_pos[1]
                dz = pos[2] - last_pos[2]
                total_distance += math.sqrt(dx*dx + dy*dy + dz*dz)
                
            last_pos = pos
            
            wall_hit = False
            for cp in self.robot_node.getContactPoints():
                # Floor contacts are at Z=0. Wall contacts happen higher up (wheels or chassis)
                if cp.point[2] > 0.015:
                    wall_hit = True
                    break
                    
            if wall_hit:
                collisions += 1
                break
                
            # Check if flipped over
            if pos[2] > 0.08 or pos[2] < 0.02:
                collisions += 1
                break
                
        fitness = (total_distance * 1000.0) - (step * 0.1) - (collisions * 5000.0)
        return fitness

    def run(self):
        for gen in range(self.num_generations):
            print(f"\\n=== Generation {gen} ===")
            fitnesses = []
            for i, ind in enumerate(self.population):
                print(f"Ind {i} | P:{ind[0]:.2f} I:{ind[1]:.2f} D:{ind[2]:.2f} | BaseSpd:{ind[6]:.0f} MaxSpd:{ind[7]:.0f} MinSpd:{ind[8]:.0f} | WallDist:{ind[10]:.1f}")
                fit = self.run_simulation(ind)
                fitnesses.append((fit, ind))
                print(f"  -> Fitness: {fit:.2f}")
                
            # Sort by fitness (descending)
            fitnesses.sort(key=lambda x: x[0], reverse=True)
            
            print(f"Best in Gen {gen}: {fitnesses[0][0]:.2f}")
            
            # Next generation
            new_pop = [fitnesses[0][1], fitnesses[1][1]] # Elitism
            
            while len(new_pop) < self.pop_size:
                # Tournament selection
                p1 = random.choice(fitnesses[:5])[1]
                p2 = random.choice(fitnesses[:5])[1]
                
                # Crossover
                child = []
                for i in range(len(self.bounds)):
                    if random.random() < 0.5:
                        child.append(p1[i])
                    else:
                        child.append(p2[i])
                        
                # Mutation
                for i in range(len(self.bounds)):
                    if random.random() < self.mutation_rate:
                        child[i] += random.gauss(0, (self.bounds[i][1] - self.bounds[i][0]) * 0.1)
                        child[i] = max(self.bounds[i][0], min(self.bounds[i][1], child[i]))
                        
                new_pop.append(child)
                
            self.population = new_pop
            
        print("Optimization Complete.")
        print("Best Parameters:", self.population[0])

if __name__ == '__main__':
    tuner = GeneticAlgorithmTuner()
    tuner.run()
