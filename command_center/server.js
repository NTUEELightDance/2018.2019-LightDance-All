var WebSocket = require('ws');
var fs = require('fs');

var stdin = process.stdin;

var CLI = require('clui'),
    clc = require('cli-color');

var Line          = CLI.Line,
    LineBuffer    = CLI.LineBuffer;

const BOARD_NUM = 10;

var wss = new WebSocket.Server({ port: 33116 });

var boardData = JSON.parse(fs.readFileSync('data.json', 'utf8'));;

var status = [];
var lastPing = [];
var cmdBuf = '';
for(var i = 0; i < BOARD_NUM; ++i) {
    status.push(['DISCONNECTED', clc.red]);
    lastPing.push(0);
}

setInterval(() => {
    for(var i = 0; i < BOARD_NUM; ++i) {
        ++lastPing[i];
    }
}, 1000);

wss.on('connection', (ws) => {
    ws.boardId = -1;

    ws.on('message', (msg) => {
        try {
            msg = JSON.parse(msg);
            if(msg.type == 'id') {
                let id = parseInt(msg.boardId);
                if(id >= 0 && id < BOARD_NUM) {
                    ws.boardId = id;
                    status[ws.boardId] = ['CONNECTED', clc.green];
                }
            }
            else if(msg.type == 'output') {
                let output = msg.output.split('\n');
                let lastline = output[output.length-1];
                if(lastline == '' && output.length > 1) lastline = output[output.length-2];
                if(lastline != '' && ws.boardId != -1) {
                    status[ws.boardId] = [lastline, clc.whiteBright];
                }
            }
            else if(msg.type == 'ping') {
                if(ws.boardId != -1) {
                    lastPing[ws.boardId] = 0;
                }
            }
        } catch(e) {

        }
    });

    ws.on('close', (code, reason) => {
        if(ws.boardId != -1) {
            status[ws.boardId] = ['DISCONNECTED', clc.red];
        }
    });
});

stdin.setRawMode(true);
stdin.resume();
stdin.setEncoding('utf8');
stdin.on('data', function(key) {
    if(key === '\u0003'){
        process.exit();
    }
    else if(typeof key == 'string' && ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z') || key == ' ' || key == '-')) {
        cmdBuf += key;
    }
    else if(key.charCodeAt(0) === 13) {
        try {
            var arr = cmdBuf.split(' ');
            if(arr.length < 2) {
                throw 'no enough arguments';
            }
            var msg = { type: 'nope' };
            if(arr[1] == 'check') {
                if(arr.length < 3) throw 'no enough arguments';
                var id = parseInt(arr[2]);
                if(id < -2 || id > 15) throw 'invalid id';
                msg = { type: 'check', part: id };
            } else if(arr[1] == 'control') {
                msg = { type: 'control' };
            } else if(arr[1] == 'controllocal' || arr[1] == 'cl') {
                msg = { type: 'controllocal' };
            } else if(arr[1] == 'poweroff') {
                msg = { type: 'poweroff' };
            } else if(arr[1] == 'reboot') {
                msg = { type: 'reboot' };
            } else if(arr[1] == 'upload') {
                msg = { type: 'upload' };
            } else if(arr[1] == 'reset' || arr[1] == 'rst') {
                msg = { type: 'reset' };
            }
            if(msg.type == 'nope') throw 'no op';
            if(arr[0] == 'all') {
                wss.clients.forEach((client) => {
                    if(client.readyState === WebSocket.OPEN && client.boardId > -1) {
                        if(msg.type == 'upload') {
                            var boardMsg = {
                                type: 'upload',
                                data: boardData[client.boardId]
                            };
                            client.send(JSON.stringify(boardMsg));
                        } else {
                            client.send(JSON.stringify(msg));
                        }
                    }
                });
            } else {
                var boardId = parseInt(arr[0]);
                wss.clients.forEach((client) => {
                    if(client.readyState === WebSocket.OPEN && client.boardId == boardId) {
                        if(msg.type == 'upload') {
                            var boardMsg = {
                                type: 'upload',
                                data: boardData[boardId]
                            };
                            client.send(JSON.stringify(boardMsg));
                        } else {
                            client.send(JSON.stringify(msg));
                        }
                    }
                });
            }
        } catch(e) {

        }
        cmdBuf = '';
    }
    else if(key.charCodeAt(0) === 127) {
        cmdBuf = cmdBuf.substring(0, cmdBuf.length-1);
    }
});

setInterval(() => {
    var outputBuffer = new LineBuffer({
        x: 0,
        y: 0,
        width: 'console',
        height: BOARD_NUM+2
    });
    var line;
    line = new Line(outputBuffer)
        .column('ID', 4, [clc.whiteBright])
        .column('Status', 50, [clc.whiteBright])
        .column('Ping', 4, [clc.whiteBright])
        .fill()
        .store();
    for(var i = 0; i < BOARD_NUM; ++i) {
        line = new Line(outputBuffer)
            .column(i.toString(), 4, [clc.cyan])
            .column(status[i][0], 50, [status[i][1]])
            .column(lastPing[i].toString(), 4, [lastPing[i] > 5 ? clc.red : clc.green])
            .fill()
            .store();
    }
    line = new Line(outputBuffer)
        .column(cmdBuf, 100)
        .fill()
        .store();
    outputBuffer.output();
}, 100);