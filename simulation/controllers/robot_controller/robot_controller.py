from controller import Robot
import struct
import math
import random
import sys

robot = Robot()
timeStep = int(robot.getBasicTimeStep())

leftMotor = robot.getDevice("left_motor")
rightMotor = robot.getDevice("right_motor")
leftMotor.setPosition(float('inf'))
rightMotor.setPosition(float('inf'))
leftMotor.setVelocity(0.0)
rightMotor.setVelocity(0.0)

dsFront = robot.getDevice("sensor_front")
dsFront.enable(timeStep)
dsLeft = robot.getDevice("sensor_left")
dsLeft.enable(timeStep)
dsRight = robot.getDevice("sensor_right")
dsRight.enable(timeStep)

imu = robot.getDevice("imu")
imu.enable(timeStep)

receiver = robot.getDevice("receiver")
receiver.enable(timeStep)
emitter = robot.getDevice("emitter")

Pid_P, Pid_I, Pid_D = 1.0, 0.0, 0.0
turnProportionalSpeed = 60
turnMinSpeed, turnMaxSpeed = 120, 180
baseSpeed = 130
maxMotorSpeed, minMotorSpeed = 150, -10
delayBeforeTurn = 310.0

distanceFromSingleWall = 8.0
distanceFromSingleWallTreshold = 5.0
distanceFromFrontWall = 16.0

DIRECTION_FRONT = 0
DIRECTION_LEFT = 1
DIRECTION_RIGHT = 2
distances = [0.0, 0.0, 0.0]

lastCorrectAngle = 0.0
accumulated_yaw = 0.0
last_yaw = None

class SimplePID:
    def __init__(self):
        self.integral = 0.0
        self.prev_error = 0.0
        
    def compute(self, input_val, setpoint):
        error = setpoint - input_val
        self.integral += error
        derivative = error - self.prev_error
        self.prev_error = error
        return Pid_P * error + Pid_I * self.integral + Pid_D * derivative

pid = SimplePID()

class RunAborted(Exception):
    pass

def checkSupervisorMessages():
    global Pid_P, Pid_I, Pid_D, turnProportionalSpeed, turnMinSpeed, turnMaxSpeed
    global baseSpeed, maxMotorSpeed, minMotorSpeed, delayBeforeTurn
    global distanceFromSingleWall, distanceFromSingleWallTreshold
    global accumulated_yaw, last_yaw, lastCorrectAngle, pid
    
    if receiver.getQueueLength() > 0:
        data = receiver.getBytes()
        params = struct.unpack('11d', data)
        Pid_P, Pid_I, Pid_D = params[0], params[1], params[2]
        turnProportionalSpeed = params[3]
        turnMinSpeed, turnMaxSpeed = params[4], params[5]
        baseSpeed = params[6]
        maxMotorSpeed = params[7]
        minMotorSpeed = params[8]
        delayBeforeTurn = params[9]
        distanceFromSingleWall = params[10]
        distanceFromSingleWallTreshold = distanceFromSingleWall / 2.0
        receiver.nextPacket()
        
        # Reset state for a new GA run
        accumulated_yaw = 0.0
        last_yaw = None
        lastCorrectAngle = 0.0
        pid = SimplePID()
        return True
    return False

def check_collision_and_send():
    # Deprecated: The supervisor now reads physical contacts directly from the physics engine
    return False

def get_distance_side(rawValue):
    analogValues = [2500, 1500, 880, 600, 450, 405, 370, 330, 300, 150, 100]
    distanceLookup = [1.0, 2.5, 5.0, 7.5, 10.0, 12.5, 15.0, 17.5, 20.0, 35.0, 45.0]
    
    if rawValue < 100: return 50.0
    if rawValue > 2500: return 1.0
    
    for i in range(len(analogValues) - 1):
        if analogValues[i] >= rawValue > analogValues[i + 1]:
            proportion = (rawValue - analogValues[i + 1]) / float(analogValues[i] - analogValues[i + 1])
            return distanceLookup[i] + (1.0 - proportion) * (distanceLookup[i + 1] - distanceLookup[i])
    return 15.0

def get_distance_front(rawValue):
    analogValues = [2000, 1200, 750, 500, 375, 300, 200, 150]
    distanceLookup = [5, 10, 15, 20, 25, 30, 35, 40]
    
    if rawValue < 150: return 70.0
    if rawValue > 2000: return 5.0
    
    for i in range(len(analogValues) - 1):
        if analogValues[i] >= rawValue > analogValues[i + 1]:
            proportion = (rawValue - analogValues[i + 1]) / float(analogValues[i] - analogValues[i + 1])
            return distanceLookup[i + 1] + proportion * (distanceLookup[i] - distanceLookup[i + 1])
            
    return 60000.0 * math.pow(max(rawValue, 1), -1.05) + 3.0

def measureDistanceAllDirections():
    rawFront = dsFront.getValue() + random.gauss(0, 10.0) # Add simulated ADC noise
    rawLeft = dsLeft.getValue() + random.gauss(0, 10.0)
    rawRight = dsRight.getValue() + random.gauss(0, 10.0)
    
    distances[DIRECTION_FRONT] = get_distance_front(rawFront)
    distances[DIRECTION_LEFT] = get_distance_side(rawLeft)
    distances[DIRECTION_RIGHT] = get_distance_side(rawRight)

def do_step():
    """Wrapper for robot.step that cleanly aborts long-running logic if a collision or reset occurs."""
    if robot.step(timeStep) == -1:
        sys.exit(0)
    measureDistanceAllDirections()
    
    if checkSupervisorMessages():
        raise RunAborted()
    if check_collision_and_send():
        raise RunAborted()

def getAngleZ():
    global accumulated_yaw, last_yaw
    yaw = imu.getRollPitchYaw()[2] * 180.0 / math.pi
    if last_yaw is not None:
        diff = yaw - last_yaw
        if diff > 180.0:
            diff -= 360.0
        elif diff < -180.0:
            diff += 360.0
        accumulated_yaw += diff
    else:
        accumulated_yaw = yaw
    last_yaw = yaw
    return accumulated_yaw

def mapSpeed(arduinoSpeed):
    return (arduinoSpeed / 255.0) * leftMotor.getMaxVelocity()

def drive(motorSpeedLeft, motorSpeedRight):
    leftMotor.setVelocity(mapSpeed(motorSpeedLeft))
    rightMotor.setVelocity(mapSpeed(motorSpeedRight))

def stop():
    drive(0, 0)

def delay_ms(ms):
    steps = int(ms / timeStep)
    for _ in range(steps):
        do_step()

def thereIsAWall(direction):
    if distances[direction] >= 1.0 and distances[direction] <= (distanceFromSingleWall + distanceFromSingleWallTreshold):
        return True
    return False

def PidDrive(distanceFromMiddle, maxSpeed, isThereAWall):
    global lastCorrectAngle
    output = pid.compute(distanceFromMiddle, 0)
    
    motorSpeedLeft = maxSpeed - output
    motorSpeedRight = maxSpeed + output
    
    motorSpeedLeft = max(minMotorSpeed, min(motorSpeedLeft, maxMotorSpeed))
    motorSpeedRight = max(minMotorSpeed, min(motorSpeedRight, maxMotorSpeed))
    
    drive(motorSpeedLeft, motorSpeedRight)
    
    if isThereAWall:
        lastCorrectAngle = getAngleZ()

def forwardWithAlignment(maxSpeed):
    if thereIsAWall(DIRECTION_LEFT) and thereIsAWall(DIRECTION_RIGHT):
        distanceFromMiddle = distances[DIRECTION_RIGHT] - distances[DIRECTION_LEFT]
        PidDrive(distanceFromMiddle, maxSpeed, True)
        
    elif thereIsAWall(DIRECTION_LEFT) and not thereIsAWall(DIRECTION_RIGHT):
        distanceFromMiddle = (distanceFromSingleWall - distances[DIRECTION_LEFT]) * 2.0
        PidDrive(distanceFromMiddle, maxSpeed, True)
        
    elif not thereIsAWall(DIRECTION_LEFT) and thereIsAWall(DIRECTION_RIGHT):
        distanceFromMiddle = (distances[DIRECTION_RIGHT] - distanceFromSingleWall) * 2.0
        PidDrive(distanceFromMiddle, maxSpeed, True)
        
    else:
        angle = getAngleZ()
        error = (angle - lastCorrectAngle) * 0.2
        PidDrive(error, maxSpeed, False)

def turnLeft(desiredangle):
    global lastCorrectAngle
    startAngle = getAngleZ()
    currentAngle = getAngleZ()

    drive(-turnMaxSpeed, turnMaxSpeed)

    while currentAngle <= startAngle + desiredangle:
        do_step()
        currentAngle = getAngleZ()
        howFareAreWeFromDestinacion = ((startAngle + desiredangle) - currentAngle) / desiredangle
        howFareAreWeFromDestinacion = howFareAreWeFromDestinacion * 2.0
        
        speed = turnMinSpeed + (turnProportionalSpeed * howFareAreWeFromDestinacion)
        speed = max(turnMinSpeed, min(speed, turnMaxSpeed))
        drive(-speed, speed)
        
    drive(80, -80)
    delay_ms(60)
    stop()
    lastCorrectAngle = getAngleZ()

def turnRight(desiredangle):
    global lastCorrectAngle
    startAngle = getAngleZ()
    currentAngle = getAngleZ()

    drive(turnMaxSpeed, -turnMaxSpeed)

    while currentAngle >= startAngle - desiredangle:
        do_step()
        currentAngle = getAngleZ()
        howFareAreWeFromDestinacion = (currentAngle - (startAngle - desiredangle)) / desiredangle
        howFareAreWeFromDestinacion = howFareAreWeFromDestinacion * 2.0
        
        speed = turnMinSpeed + (turnProportionalSpeed * howFareAreWeFromDestinacion)
        speed = max(turnMinSpeed, min(speed, turnMaxSpeed))
        drive(speed, -speed)
        
    drive(-80, 80)
    delay_ms(60)
    stop()
    lastCorrectAngle = getAngleZ()

# Send ready signal
emitter.send(struct.pack('d', 1.0))

# Main loop
while robot.step(timeStep) != -1:
    try:
        measureDistanceAllDirections()
        if checkSupervisorMessages():
            continue
        if check_collision_and_send():
            stop()
            continue
            
        if distances[DIRECTION_FRONT] > distanceFromFrontWall:
            forwardWithAlignment(baseSpeed)
        else:
            stop()
            delay_ms(delayBeforeTurn)
            if distances[DIRECTION_LEFT] >= distances[DIRECTION_RIGHT]:
                turnLeft(79.0)
            else:
                turnRight(79.0)
    except RunAborted:
        stop()
        continue
