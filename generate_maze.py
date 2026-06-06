import math
import re

def create_wall(name, x, y, angle, length, width=0.015, height=0.15, offset=0):
    wall_x = x - offset * math.sin(angle)
    wall_y = y + offset * math.cos(angle)
    
    return f"""Solid {{
  translation {wall_x:.4f} {wall_y:.4f} {height/2:.3f}
  rotation 0 0 1 {angle:.4f}
  children [
    Shape {{
      appearance PBRAppearance {{
        baseColor 0.8 0.8 0.8
      }}
      geometry Box {{
        size {length:.4f} {width:.4f} {height:.3f}
      }}
    }}
  ]
  name "{name}"
  boundingObject Box {{
    size {length:.4f} {width:.4f} {height:.3f}
  }}
}}"""

L = 0.285
corridor_width = 0.285
half_width = corridor_width / 2.0

blocks = [
    ("L", "R"),
    ("L", "R"),
    ("L", "R"),
    ("none", "R"),  # Missing left
    ("L", "R"),
    ("L", "none"),  # Missing right
    ("L", "R"),
    ("none", "none"), # Missing both
    ("none", "none"), # Missing both
    ("L", "R"),
    ("PL", "R"), # Missing left, but perpendicular wall present
    ("L", "R"),
    ("L", "PR"), # Missing right, but perpendicular wall present
    ("L", "R"),
    ("L", "R"),
    ("L", "R"),
    ("end", "end") # Place a wall in front to stop
]

walls_vrml = ""
current_x = 0.0
current_y = 0.0

for i, (left, right) in enumerate(blocks):
    cx = current_x + (L/2.0)
    cy = current_y
    
    if left == "L":
        walls_vrml += create_wall(f"LeftWall_{i}", cx, cy, 0, L, offset=half_width) + "\n"
    elif left == "PL":
        # Perpendicular wall sticking outwards on the left
        walls_vrml += create_wall(f"PerpLeft_{i}", cx, cy + half_width + L/2.0, math.pi/2, L, offset=0) + "\n"
        
    if right == "R":
        walls_vrml += create_wall(f"RightWall_{i}", cx, cy, 0, L, offset=-half_width) + "\n"
    elif right == "PR":
        # Perpendicular wall sticking outwards on the right
        walls_vrml += create_wall(f"PerpRight_{i}", cx, cy - half_width - L/2.0, math.pi/2, L, offset=0) + "\n"
        
    if left == "end":
        walls_vrml += create_wall(f"EndWall_{i}", cx, cy, math.pi/2, corridor_width, offset=0) + "\n"
        
    current_x += L

walls_vrml += create_wall("StartWall", -0.1, 0, math.pi/2, corridor_width) + "\n"

with open("simulation/worlds/maze_world.wbt", "r") as f:
    content = f.read()

# Webots format might have multiple Solids, we just replace everything up to Jerry3
new_content = re.sub(r'Solid \{.*?DEF Jerry3 Jerry3 \{', walls_vrml + 'DEF Jerry3 Jerry3 {', content, flags=re.DOTALL)

with open("simulation/worlds/maze_world.wbt", "w") as f:
    f.write(new_content)
