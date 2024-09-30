# FighterProject - 3D Beat 'Em Up Code Showcase

Welcome to **FighterProject**, a 3D beat 'em up game developed in Unreal Engine using C++. This repository serves as a **code showcase** and is not intended as a full game distribution. The purpose of this repo is to highlight specific gameplay mechanics, systems, and design patterns used in this Unreal Engine project.

## Project Overview
- **Type**: 3D Beat 'Em Up Game
- **Engine**: Unreal Engine
- **Language**: Primarily developed in C++
## Main Features
- **Combat System**: 
  - A player-customizable combo system
  - AI attack selection capable of hosting different movesets for diverse enemy designs
  - Characters properties designed to be tweaked and adjusted to design enemy power levels and variety
  - Hit effects, and hit states (i.e. juggling, stagger, stun, launched) enemies
  - Directional hit reactions
  - Enemy grab system
  - Finisher system
  - Special attacks system
  - [View Special Attacks Code](Components/Component_SpecialAttacks.cpp)
  
- **Throwable Objects and Weapons**: 
  - Various objects that can be thrown during combat.
  - Weapons with unique movesets for both player and AI.

- **Level Management**
  - Level loading system
  - Persistent data handling to manage the flow between:
    - HUB level
    - Arena levels
    - Mission levels


## Code Structure
- **Source**: The repository contains the core C++ source code used in the game.
- **Exclusions**: All paid assets, game content, and non-code resources (like models, textures, animations, etc.) have been excluded to comply with licensing requirements.

## Purpose
The primary goal of this repository is to demonstrate:
- Advanced use of **Unreal Engine C++** features.
- **AI-controlled combat behavior**.
- **Input handling and custom attack systems**.
- Implementation of **root motion** mechanics to enhance melee combat animations.

## Disclaimer
- **Assets**: The project contains paid assets that are **not included** in this repository.
- **Gameplay**: This is not a fully playable game. It is a work-in-progress project meant to show code and system logic.

## Setup Instructions
1. Clone the repository to your local machine.
2. Ensure you have Unreal Engine installed.
3. Open the `.uproject` file in Unreal Engine.
4. Compile the project using Visual Studio.

## Contact
For questions or feedback, feel free to contact me via my portfolio website.
