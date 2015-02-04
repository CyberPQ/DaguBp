#! /usr/bin/python
# -*- coding: utf-8 -*-

import time
import json
import sys
import os.path
from math import *
from flask import Flask, render_template, request, jsonify, url_for, send_from_directory
from rover import *

app = Flask(__name__)

@app.route("/")
def show_ihm():
	global maPiste
	template_data = {
		'IP'             : "<???>",
	}
	return render_template('basic.html', **template_data)
	
@app.route('/virtualjoystick.js')
def virtualjoystick():
    return send_from_directory(os.path.join(app.root_path, 'static'), 'virtualjoystick.js', mimetype='application/javascript')
	
@app.route('/jquery.min.js')
def jquery():
    return send_from_directory(os.path.join(app.root_path, 'static'), 'jquery.min.js', mimetype='application/javascript')
	
@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'), 'favicon.ico', mimetype='image/png')
	
@app.route('/apple-touch-icon.png')
def apple_touch_icon():
    return send_from_directory(os.path.join(app.root_path, 'static'), 'apple-touch-icon.png', mimetype='image/png')
	
# Appel ajax GET pour rafraichir l'etat du robot
@app.route("/maj_etat")
def maj_etat():
	reponse_json_str = ""
	return reponse_json_str
	
# Appel ajax GET pour demander l'allumage du laser
@app.route("/allume_laser/<IdxLaser>")
def allume_laser(IdxLaser):
	return "<not done yet>"

# Appel ajax GET de commandes des chenilles en mode Gauche/Droite
@app.route("/cmd_chenilleGD/<CmdGauche>/<CmdDroite>")
def cmd_chenilleGD(CmdGauche, CmdDroite):
	#transformation en entier
	CmdGauche = int(float(CmdGauche))
	CmdDroite = int(float(CmdDroite))
	
	#print "cmd_chenilleGD : {:+3} {:+3}".format(CmdGauche, CmdDroite)
	
	if(CmdGauche>0):
		rover.set_speed_a(DaguRover.FORWARD, int(CmdGauche))
	else:
		rover.set_speed_a(DaguRover.REVERSE, -int(CmdGauche))
		
	if(CmdDroite>0):
		rover.set_speed_b(DaguRover.FORWARD, int(CmdDroite))
	else:
		rover.set_speed_b(DaguRover.REVERSE, -int(CmdDroite))
		
	return "OK"
	
	# Appel ajax GET de commandes des chenilles depuis un joystick (X,Y)
@app.route("/cmd_chenilleXY/<x>/<y>")
def cmd_chenilleXY(x, y):
	#transformation en flottant
	x = float(x)
	y = float(y)
	xDeflection = 0
	yDeflection = 0
	deadzone = 0.25
	
	#Suppression de la zone morte du joystick
	# cf http://stackoverflow.com/questions/1944499/joystick-deadzone-calculation
	if (x > 5):
		xDeflection = (1 - deadzone) * x + deadzone * 255;
	elif (x < -5):
		xDeflection = (1 - deadzone) * x - deadzone * 255;
    
	if (y > 5):
		yDeflection = (1 - deadzone) * y + deadzone * 255;
	elif (y < -5):
		yDeflection = (1 - deadzone) * y - deadzone * 255;
    
	#if (x != 0 or y != 0):
	#	distRange = 1 - 0.71 * deadzone;
	#	dist = distRange * sqrt(x * x + y * y) + deadzone*255;
	#	angle = atan2(x, y);
	#	xDeflection = dist * sin(angle);
	#	xDeflection = min(255, max(-255, xDeflection));
	#	yDeflection = dist * cos(angle);
	#	yDeflection = min(255, max(-255, yDeflection));
	
	#print "cmd_chenilleXY : {:+3} {:+3} --> {:+10.4f} {:+10.4f} {:+10.4f}".format(x, y, dist, xDeflection, yDeflection)
	
	#Transformation de la commande de joystick (xDeflection, yDeflection) en vitesse (Vgauche, Vdroite)
	k = 0.8;
	CmdGauche = yDeflection - k * xDeflection;
	CmdDroite = yDeflection + k * xDeflection;
	
	#Application de la consigne
	rc = cmd_chenilleGD(CmdGauche, CmdDroite)
		
	return rc
	
if __name__ == "__main__":
	rover = DaguRover(0, 0x0f) # update with your bus number and address
	time.sleep(0.1)
	print "Lancement du serveur Web sur le port 80 (http://127.0.0.1:80)";
	
	# rover.set_speed_a(DaguRover.FORWARD, 100)
	# rover.set_speed_b(DaguRover.FORWARD, 100)
	# time.sleep(0.5)
	rover.set_speed_a(DaguRover.STOP, 0)
	rover.set_speed_b(DaguRover.STOP, 0)
	
	app.debug=True
	app.silent=True
	app.run(host='0.0.0.0', port=80)
