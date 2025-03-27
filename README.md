# THIS REPOSITORY HAS BEEN MOVED TO https://github.com/AlgTUDelft/cTORS


# Treinonderhoud- en -rangeersimulator (TORS)
This implementation of TORS consists of a backend written in C++ (cTORS), and a front-end written in python (TORS).

## Project setup
The basic project setup uses the structure provided by cmake. The subfolders are subprojects:
* cTORS: The c++ implementation of TORS
* cTORSTest: The tests for cTORS
* pyTORS: The python interface for cTORS
* TORS: The challenge environment, in python


# Native support
* Linux [YES]
* macOS [NO] - via Dev-Container/Docker [YES]
* Winfows [NO] - via Dev-Container Docker [YES]

# Note:
The tool was developed on Linux and building the tool on macOS might cause compilation and execution errors*. Therefore, a Dockerized version is also avalable in this repository. Moreover, to facilitate the development the tool is available in **[Dev-Container](https://code.visualstudio.com/docs/devcontainers/tutorial)** 

(*) With gcc@9 Homebrew protobufer native libraries must be modified wichi is not a good practice
(*) With llvm Homebrew installation basic C Test filse cannot be compiled on Intel-based mac systems
(*) Compile process is sucessfull under native clang (14), however, SIGILL - illegal instruction signal errors can happen during the tool's execution.  
 


# Build process - Native Linux

## Install dependencies 
### Install gcc
The following section explains how to compile this source code

Before build on Linux - Native support

Other dependencies to install:

```bash
sudo apt update

sudo apt install gcc-9

sudo apt install g++-9

```
If **error**: `No CMAKE_CXX_COMPILER could be found`

```bash
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90
sudo update-alternatives --config g++
```
Choose the correct version: Select the number corresponding to the version of g++ that is aimed to be used.

### Install Cmake and Python development libraries
To compile cTORS, cmake 3.11 (or higher) is required and the python development libraries:
```
apt-get install cmake
apt-get install python3-dev
```

### Install anaconda3

```bash
wget https://repo.anaconda.com/archive/Anaconda3-2024.06-1-Linux-x86_64.sh
bash Anaconda3-2024.06-1-Linux-x86_64.sh

sudo rm Anaconda3-2024.06-1-Linux-x86_64.sh
conda init
```



### Create and activate a `conda` environment

Create env:
```bash
conda env create -f env.yml
```

Activate environment:
```bash
conda activate my_proto_env
```

### Build with setuptools
You can build cTORS and the pyTORS library with the following command.
```sh
mkdir build
python setup.py build
python setup.py install
```

### Compile cTORS from C++ source
In the source directory execute the following commands:
**Don't forget to specify** the `-DCONDA_ENV="path/to/conda_env"`
```bash
mkdir build
cd build
cmake .. -DCONDA_ENV="path/to/conda_env
cmake --build .
```
This has been tested with gcc 9.4.0 Older versions may not support the c++17 standard. 


# Building process - Dev-Container

## Dev-Container setup
The usage of **[Dev-Container](https://code.visualstudio.com/docs/devcontainers/tutorial)** is highly recommanded in macOS environment. Running **VS Code** inside a Docker container is useful, since it allows to compile and use cTORS without plaform dependencies. In addition, **Dev-Container** allows to an easy to use dockerized development since the mounted `ctors` code base can be modified real-time in a docker environment via **VS Code**.

* 1st - Install **Docker**

* 2nd - Install **VS Code** with the **Dev-Container** extension. 

* 3rd - Open the project in **VS Code**

* 4th - `Ctrl+Shif+P` → Dev Containers: Rebuild Container (it can take a few minutes) - this command will use the [Dockerfile](.devcontainer/Dockerfile) and [devcontainer.json](.devcontainer/devcontainer.json) definitions unde [.devcontainer](.devcontainer).

* 5th - Build process of the tool is below: 
Note: all the dependencies are alredy contained by the Docker instance.

### Create and activate a `conda` environment

Create env:
```bash
conda env create -f env.yml
```

Activate environment:
```bash
conda activate my_proto_env
```

### Build with setuptools
You can build cTORS and the pyTORS library with the following command.
```sh
mkdir build
python setup.py build
python setup.py install
```

### Compile cTORS from C++ source
In the source directory execute the following commands:
**Don't forget to specify** the `-DCONDA_ENV="path/to/conda_env"`
```bash
mkdir build
cd build
cmake .. -DCONDA_ENV="path/to/conda_env
cmake --build .
```


# Usage of sceneario, location and plan converters
The sceneario, location and plan converters are used to convert HIP input files (scenarion, location) and results (plan - scheduling) to cTORS. This conversion allows cTORS to evaluate HIP plans. Converters:

* [preprocess_location.py](/data/Demo/location/preprocess_location.py)
* [process_scenario.py](/data/Demo/location/process_scenario.py)
* [preprocess_plan.py](/data/Demo/plan/preprocess_plan.py)

**Location conversion**
In the code `input_location.json` specifies the HIP location file , which wll be converted into a cTORS enabled location file. 
```bash
cd data/Demo/location
python3 process_scenario.py
```

**Scenario conversion**
In the code `scenario_input.json` specifies the HIP scenario file , which wll be converted into a cTORS enabled scenario file. 
```bash
cd data/Demo/location
python3 preprocess_location.py
```

**Plan conversion**
In the code `plan_input.json` specifies the HIP plan file and `scenario.json` specifies the converted scenario file, which is also needed to create the plan. The HIP plan is converted into a cTORS enabled plan file. 
```bash
cd data/Demo/plan
python3 preprocess_plan.py
```


# Usage of the Plan evaluator
This mode of the program is designed to evaluate the feasibility of different plans (shunting yard schedules). ***Current version is able to evaluate HIP issued plans***. 

In [EngineTest.cpp](cTORSTest/EngineTest.cpp):

* `LocationEngine engine(path)` - `path` specifies the path to the folder where the `location` and `scenario` files are stored. 

* `engine.GetScenario(path+name)`- `path+name` specifies the path and file name to the `scenario` file.

* `GetRunResultProto(path+name)` - `path+name` specifies the path and file name to the `plan` (JSON) file, that must be evaluated.

Usage:

```bash
cd build/cTORSTest
./EngineTest
```

In case of modification of the code, compile wiht: 

```bash
cd build
cmake --build .
```

# Basic usage

## Run the challenge environment
To run challenge environment, run the following code

```sh
cd TORS
python run.py
```

Optionally you can change the episode or agent data by changing the parameters
```sh
python run.py --agent agent.json --episode episode.json
```
The `--agent` option sets the file that configures the agent.
The `--episode` option sets the file that configures the episode.

You can also run the file with the `--train` flag to train the agent instead of eveluating its performance.

## Usage in Python
To use cTORS in python, you need to import they `pyTORS` library. E.g.

```python
from pyTORS import Engine

engine = Engine("data/Demo")
scenario = engine.get_scenario("data/Demo/scenario.json")
state = engine.start_session(scenario)

actions = engine.step(state)
engine.apply_action(actions[0])

engine.end_session(state)
```

## Running the visualizer

The visualizer runs as a flask server. Install the dependencies in `TORS/requirements-visualizer` first.
```sh
pip install -r TORS/requirements-visualizer
```
Now flask can be run by running the commands:
```sh
cd TORS/visualizer
export FLASK_APP=main.py
export FLASK_ENV=development
export FLASK_RUN_PORT=5000
python -m flask run
```

## Running the example RL-agent with gym
The repository also includes example code that wraps cTORS in a gym-environment and uses an RL implementation from stable-baselines3 to learn a policy. To run this example, first install the requirements:
```sh
pip install -r TORS/requirements-gym
```
Then run:
```sh
cd TORS
python run_gym.py
```
You can check the learning progress using tensorboard:
```sh
tensorboard --logdir ./log_tensorboard/
```

## Configuration
TORS can be configured through configuration files. Seperate configuration exists for
1. The location
2. The scenario
3. The simulator
3. The episode
4. The agent

### Configuring the location
A location is described by the `location.json` file in the data folder.
It describes the shunting yard: how all tracks are connected, what kind of tracks they are, and distances among tracks.

In order to use the visualizer for that location, you need to provided another file `vis_config.json`. See the folder `data/Demo` and `data/KleineBinckhorstVisualizer` for examples.

### Configuring the scenario
A scenario is described by the `scenario.json` file in the data folder.
It describes the scenario: which employees are available, shunting units' arrivals and departures, and possible disturbances.

### Configuring the simulator
The simulator can be configured by the `config.json` file in the data folder.
It describes which business rules need to be checked and the parameters for the actions

### Configuring the episode
You can provide an episode configuration and pass it to `TORS/run.py` with the `--episode` parameter.
This file describes the evaluation/training episode.
It contains the path to the data folder, the number of runs, RL parameters and parameters for scenario generation.

### Configuring the agent
You can provide an agent configuration and pass it to `TORS/run.py` with the `--agent` parameter.
This file prescribes which agent to use, and passes parameters to the agent.

## Tests
### Run the cTORS tests
To run the cTORS tests, execute the commands
```sh
cd build
ctest
```

## Documentation
The documentation in the C++ code is written in the Doxygen format. Install doxygen (optional) to generate the documentation, or check the full documentation online at [algtudelft.github.io/cTORS](https://algtudelft.github.io/cTORS/).

### Dependencies installation
To generate the documentation, install the following programs:
```sh
apt-get install -y doxygen graphviz fonts-freefont-ttf
apt-get install -y libclang-dev
python -m pip install git+git://github.com/pybind/pybind11_mkdoc.git@master
python -m pip install pybind11_stubgen
```

### Generate the documentation
With the dependencies installed, cmake automatically generates the documentation. It can also be generated manually by running
```sh
cd cTORS
doxygen Doxyfile
cd ..
python -m pybind11_mkdoc -o pyTORS/docstrings.h cTORS/include/*.h -I build/cTORS
```
This produces as output the `cTORS/doc` folder and the `pyTORS/docstrings.h` source file. This last file is used in `pyTORS/module.cpp` to generate the python docs.

## Contributors
* Mathijs M. de Weerdt: Conceptualization, Supervision, Project administration, Funding acquisition, Writing - review & editing
* Bob Huisman: Conceptualization
* Koos van der Linden: Software, Writing - Original draft
* Jesse Mulderij: Writing - Original draft
* Marjan van den Akker: Supervision of the bachelor team
* Han Hoogeveen: Supervision of the bachelor team
* Joris den Ouden: Conceptualization, Supervision of the bachelor team
* Demian de Ruijter: Conceptualization, Supervision of the bachelor team
* Bachelor-team, consisting of Dennis Arets, Sjoerd Crooijmans, Richard Dirven, Luuk Glorie, Jonathan den Herder, Jens Heuseveldt, Thijs van der Horst, Hanno Ottens, Loriana Pascual, Marco van de Weerthof, Kasper Zwijsen: Software, Visualization

