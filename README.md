# 🧪 Physics Playground (UE5)

A first-person physics sandbox prototype built entirely in Unreal Engine 5.  
Developed as part of a university practical assignment to explore physics simulation, interactivity, and combat systems.

---

## 🎮 Gameplay Overview

The player explores a small test environment filled with destructible objects, physical interactions, and enemy targets.  
The goal: use your physics tools and weapons to destroy all statues and survive explosive enemies.

---

## 🧠 Features Implemented

### 🧲 Object Interaction

- Hold **Right Mouse Button** to grab any physics-enabled object in front of the player
- Maintain a consistent holding distance and rotate objects naturally
- **Release RMB** to drop or throw the object, using impulse force

### ✨ Highlight System

- Objects that can be grabbed are outlined when in front of the player
- Makes interaction intuitive and readable

### 🏃‍♂️ Sprint + Stamina System

- **Hold Shift** to sprint while moving
- UI stamina bar drains while sprinting, refills otherwise
- Sprint automatically cancels when stamina hits zero
- Sprinting is re-enabled only after releasing and pressing Shift again

---

## 🔫 Weapons

### 1. Hitscan Gun
- Fires instant raycast bullets
- On hit: applies damage, a directional force, and a visual particle effect

### 2. Projectile Gun
- Fires linear, gravity-free projectiles
- On hit: applies damage and impulse force

### 3. Grenade Launcher
- Fires arcing, gravity-affected grenades
- On impact: triggers an **explosion** with radial force and VFX

---

## 💀 Enemies

- Can be killed with **any weapon type**
- On death:
  - Enter **ragdoll** state using physics simulation
- On contact with the player:
  - **Explode into destructible parts**, launching mesh fragments using Chaos physics
  - Visual consistency preserved (no material artifacts)

---

## 🚪 Environmental Systems

### Physics Doors
- Hinged using physics **constraints**
- Can be grabbed and swung open
- Applying enough force will break the door off its pivot

### Destructible Statues (Objective)
- Statues break on:
  - Weapon impact
  - High-speed collisions
- Implemented using Unreal’s **Chaos Destruction System**
- Tracks number of destroyed statues via dynamic UI counter

### Breakable Props
- At least one scene prop can be broken after repeated hits from any weapon
- Uses Chaos for realistic visual fragmentation

---

## 🧩 UI Elements

- **Stamina bar** updates in real-time during sprint
- **Statue counter** shows total and remaining objectives

---

## 🛠️ Technologies Used

- Unreal Engine 5
- Blueprint and C++ integration
- Chaos Physics & Destruction
- Niagara (for particles)
- UMG (for UI)
- Constraint physics (for doors and props)

---

## 👨‍💻 Author

**Alex**  
[LinkedIn](https://ch.linkedin.com/in/alexandrepages-ch) • [Portfolio](https://www.artstation.com/apages)
