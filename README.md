DaguBp
======

Baptiste's Dagu Rover, based on a Raspberry Pi and an Arduino.

Software Requirements
----------------------
*to be done...*

```bash
	sudo apt-get update 
	sudo apt-get install arduino picocom git i2c-tools  
	sudo apt-get install python-dev python-setuptools python-pip python-smbus
	pip install ino
	cd ~
	git clone https://github.com/CyberPQ/DaguBp.git
```


Electrical connexions
----------------------

Dagu Rover left track:

  - Encoder A   --> Arduino Pin 2
  - Encoder B   --> Arduino Pin 5
  - Motor       --> Arduino motor 1 (direction pin 7, PWM pin 9)

Dagu Rover right track:

  - Encoder A   --> Arduino Pin 3
  - Encoder B   --> Arduino Pin 4
  - Motor       --> Arduino motor 2 (direction pin 8, PWM pin 10)


Arduino programming
--------------------
The arduino board **Dagu Mini-Driver** (http://www.dagurobot.com/goods.php?id=142) can be programmed directly from the Raspberry Pi, using the **INO tool** software (http://inotool.org/).

To build the firmware and upload it onto the **Dagu Mini-Driver** :

```bash
	cd ~/DaguBp/src_arduino
	ino build
	ino upload
```

Direct use of the robot
------------------------
The Arduino can be control directly, using a kind of minimal shell over serial port:

```bash
	cd ~/DaguBp/src_arduino
	ino serial
```

The commands are :

| Cmd | Action																|
| :-: | --------------------------------------------------------------------|
| `z` | Release the motors (stop pids)										|
| `a` | Turn on the PIDs, and drives the motor as needed to the setpoint 	|
| `t` | Turn on the auto-tune on PIDs (not done yet !)						|
| `p` | Ask the robot to go forward 5cm										|
| `m` | Ask the robot to go backward 5cm									|
| `w` | Ask the robot to turn left 20°										|
| `x` | Ask the robot to turn right 20°										|
| `h` | Show help															|

To leave the minicom software and return to bash shell, hit `<Ctrl>+<A>` then `<Ctrl>+<X>`

