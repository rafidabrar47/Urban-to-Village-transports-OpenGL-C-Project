<div align="center">

🏙️ Urban to Rural Journey 🌉
A 2D Simulation in C++ and OpenGL
</div>

This project merges three distinct scenarios—an interactive Urban Area, the landmark Padma Bridge, and a peaceful Village Scene—into a single, continuous visual narrative. The simulation tells a story of modern Bangladesh, where rapid urban development and traditional rural life are linked by monumental infrastructure.


📖 Table of Contents
About The Project

Key Features

Technologies Used

Getting Started

How to Use

Contributors

🎯 About The Project
Our goal was to apply fundamental computer graphics principles to create a dynamic and culturally significant world. The simulation is not just a collection of animated objects, but a diorama that captures the essence of connectivity and progress in modern Bangladesh.

Key Features:
🏙️ Dynamic Urban Scene:

Interactive traffic system with cars, buses, and a train.

User-controlled traffic lights (R, Y, G) that vehicles and pedestrians obey.

Animated pedestrian who crosses the street at red lights and waits for traffic otherwise.

Dynamic scaling of the pedestrian to simulate perspective and distance.

🌉 The Padma Bridge:

Serves as the visual and thematic link between the urban and rural environments.

Features a flowing river and animated boats to create a lively scene.

🏡 Serene Village Scene:

Represents the traditional, calmer side of the country, completing the journey.

Features a detailed house, lush green fields, and procedurally drawn trees.

An animated rickshaw travels along a winding road.

🎮 Interactive Controls:

Manually operate the city's traffic signals using the keyboard.

Control urban vehicle speed with mouse clicks (single-click to speed up, double-click to reset).

Control the village rickshaw's speed with the F (faster) and S (slower) keys.

Toggle the entire simulation between day and night with a mouse click.

☀️ Environmental Effects:

A full day-night cycle that changes the ambient lighting, sky color (sun/moon), and activates glowing windows, headlights, and lamp posts across all scenes.

🛠️ Technologies Used
Language: C++

Graphics API: OpenGL

Utility Toolkit: GLUT (OpenGL Utility Toolkit)

🚀 Getting Started
To get a local copy up and running, follow these simple steps.

Prerequisites
You will need a C++ compiler (like g++) and the OpenGL/GLUT libraries installed on your system.

On Windows: We recommend setting up GLUT with a compiler like MinGW.

On macOS: Xcode Command Line Tools should include the necessary frameworks.

On Linux: You can install the libraries using your package manager (e.g., sudo apt-get install freeglut3-dev).

Installation & Execution
Clone the repository:

git clone [https://github.com/your_username/your_repository_name.git](https://github.com/your_username/your_repository_name.git)

Navigate to the project directory:

cd your_repository_name

Compile the code:

g++ -o simulation main.cpp -lglut -lglu -lgl

Run the executable:

./simulation

🕹️ How to Use the Simulation
Once the application is running, you can interact with the scene using the following controls:

Control

Action

Scene

Mouse Left-Click

Toggles between Day and Night mode for the entire simulation.

Global

R, Y, G Keys

Controls the Red, Yellow, and Green traffic lights.

Urban

Double Left-Click

Resets urban vehicle speeds to their default values.

Urban

F Key

Makes the rickshaw move Faster.

Village

S Key

Makes the rickshaw move Slower.

Village

🤝 Contributors
Rafid Abrar

Adnan Hossain Ratul

Iftekhar Ismail Swadheen

<p align="center">
This project was created for academic purposes at <b>American International University-Bangladesh</b>.
<br>
Last updated: September 14, 2025.
</p>
