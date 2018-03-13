var ReconnectingWebSocket = require('reconnecting-websocket');
var execFile = require('child_process').execFile;
var exec = require('child_process').exec;

var fs = require('fs');
var path = require('path');
var config = JSON.parse(fs.readFileSync('config.json', 'utf8'));

var socket = new ReconnectingWebSocket('ws://' + config.server_ip + ':33116/', [], {
	maxReconnectionDelay: 1000,
	minReconnectionDelay: 1000,
	reconnectionDelayGrowFactor: 1,
	connectionTimeout: 1000,
	maxRetries: Infinity,
	constructor: require('websocket').w3cwebsocket
});

var controlProcess = '';
var checkProcess = '';

function sendData(msg) {
	try {
		socket.send(JSON.stringify({ type: 'output', output: msg }));
	} catch(e) {

	}
};

socket.addEventListener('message', function(msg) {
	var data = JSON.parse(msg.data);
	console.log('recv', data);
	if(data.type == 'control') {
		if(checkProcess !== '') {
			checkProcess.kill();
		}
		if(controlProcess !== '') {
			controlProcess.kill();
		}
		setTimeout(function() {
			console.log('spawning control process');
			controlProcess = execFile(path.join(__dirname, 'control'), {
				cwd: __dirname
			});
			controlProcess.stdout.on('data', function(chunk) {
				sendData(chunk.toString());
			});
			controlProcess.stderr.on('data', function(chunk) {
				sendData(chunk.toString());
			});
			controlProcess.on('exit', function() {
				controlProcess = '';
			});
			controlProcess.on('error', function(err) {
				console.log('error', err);
			});
		}, 200);
	} else if(data.type == 'check') {
		if(controlProcess !== '') {
			controlProcess.kill();
		}
		var to = 0;
		if(checkProcess === '') {
			console.log('spawning check process');
			checkProcess = execFile(path.join(__dirname, 'check'), {
				cwd: __dirname
			});
			checkProcess.on('exit', function() {
				checkProcess = '';
			});
			checkProcess.on('error', function(err) {
				console.log('error', err);
			});
			to = 200;
		}
		setTimeout(function() {
			try {
				checkProcess.stdin.write(data.part.toString());
				checkProcess.stdin.write('\n');
			} catch(e) {
				console.log('e', e);
			}
		}, to);
	} else if(data.type == 'poweroff') {
		if(controlProcess !== '') {
			controlProcess.kill();
		}
		if(checkProcess !== '') {
			checkProcess.kill();
		}
		socket.close();
		setTimeout(function() {
			exec('poweroff');
			process.exit();
		}, 100);
	} else if(data.type == 'reboot') {
		if(controlProcess !== '') {
			controlProcess.kill();
		}
		if(checkProcess !== '') {
			checkProcess.kill();
		}
		socket.close();
		setTimeout(function() {
			exec('reboot');
			process.exit();
		}, 100);
	}
});

socket.addEventListener('open', function(e) {
	var msg = {
		type: 'id',
		boardId: config.board_id
	};
	socket.send(JSON.stringify(msg));
	console.log('connected', msg);
});

