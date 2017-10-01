var b = require('bonescript');

var MSBDATA = 7;
var LSBDATA = 14;
var PORT = "P8";

var clock = 15;
var load = 16;

//loop through the pins and set pinmode output
for (var i = MSBDATA; i <= LSBDATA; i += 1) {
    b.pinMode(PORT + "_" + String(i), b.OUTPUT);
}
//set pins to outs
b.pinMode(PORT + "_" + String(clock), b.OUTPUT);
b.pinMode(PORT + "_" + String(load), b.OUTPUT);

//set load high
b.digitalWrite(PORT + "_" + String(load), b.HIGH);
//set clock low;
b.digitalWrite(PORT + "_" + String(clock), b.LOW);


for (var data = 0; data <= 12; data++) {

        console.log(data);
		setDataLinesToValue(data);
		//pulse clock to write to register;
		b.digitalWrite(PORT + "_" + String(clock), b.HIGH);
		b.digitalWrite(PORT + "_" + String(clock), b.LOW);
}

function setDataLinesToValue(value) {
	//loop all the pins
	var count = 7;
	for (i = MSBDATA; i <= LSBDATA; i += 1) {
		var bitState = b.bitRead(value, count);
		b.digitalWrite(PORT + "_" + String(i), bitState);
		count = count - 1;
	}
}


