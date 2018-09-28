var http = require('http'),
    net = require('net'),
    fs = require('fs'),
    request = require('request'),
    WebSocket = require('ws'),
    watchr = require('watchr');

require('events').EventEmitter.defaultMaxListeners = 5000;

var HOST = "127.0.0.1";
var PORT = 8080;

var ipStorage = process.env.STORAGE_MACHINE;
var portStorage = 9601;

var files = [];

//**************** SOCKET CLIENT FOR COMMUNCATION WITH NETWORK CODING CLIENT PROCESS ****************

//Wrapper for managing client socket reconnections.

var timeout = 1000;
var retrying = false;
// Functions to handle socket events
function makeConnection () {
    client.connect(PORT, HOST);
}
function connectEventHandler() {
    console.log('[LOCAL SOCKET CLIENT] Connected!');
    retrying = false;
}
function dataEventHandler(data) {
    console.log(data.toString());
}
function endEventHandler() {
    // console.log('end');
}
function timeoutEventHandler() {
    // console.log('timeout');
}
function drainEventHandler() {
    // console.log('drain');
}
function errorEventHandler() {
    // console.log('error');
}
function closeEventHandler () {
    // console.log('close');
    if (!retrying) {
        retrying = true;
        console.log('[LOCAL SOCKET CLIENT] Reconnecting...');
    }
    setTimeout(makeConnection, timeout);
}

// Create socket and bind callbacks
var client = new net.Socket();
client.on('connect', connectEventHandler);
client.on('data',    dataEventHandler);
client.on('end',     endEventHandler);
client.on('timeout', timeoutEventHandler);
client.on('drain',   drainEventHandler);
client.on('error',   errorEventHandler);
client.on('close',   closeEventHandler);

//End of wrapping

//Connect
console.log('[LOCAL SOCKET CLIENT] Connecting to ' + HOST + ':' + PORT + '...');
makeConnection();

// ********** WEB SOCKET SERVER FOR COMMUNCATION WITH STORAGE CONTAINER **********

var wss = new WebSocket.Server({port: 8100 });
console.log("[LOG] Web Socket Server listening on port 8100");

wss.on('connection', function connection(ws) {
	ws.on('message', function incoming(message){

		if(message == 'hello'){
			console.log('[WEB SOCKET SERVER] Storage requested connection: accepted.');
			ws.send('welcome');
		} else{
			//Sending message to Network Coding Client program
			client.write(message);
			ws.send(message);
		}
	});
});

//**************** UPLOAD REQUEST ****************


//File uploading in storage container
var url = "http://" + ipStorage + ":" + portStorage + "/upload/";

//**************** FILE WATCHER FOR CHUNKS FOLDER ****************

/*
 * The code below listen on file added in /home/cache folder. Everytime a chunk is added, it is uploaded in storage container.
 */
/*
var watcher = chokidar.watch('/home/cache', {ignored: '/(^|[\/\\])\../', awaitWriteFinish: {stabilityThreshold: 1800,pollInterval: 90}})
	.on('add', function(event, path){
		console.log('File added: ' + event);
		files.push(event);
});
*/

var path = '/home/cache';

function listener (changeType, fullPath, currentStat, previousStat) {
	if (changeType == 'create'){
		console.log("[FILE LISTENER] File " + fullPath + " was created: " + currentStat);
		files.push(fullPath);
	}
	sendFiles();
}


function next (err) {
	if ( err )  return console.log('[FILE LISTENER] Watch failed on', path, 'with error', err)
	console.log('[FILE LISTENER] Watch successful on', path)
}

var watcher = watchr.create(path);

watcher.on('change', listener);
watcher.on('log',console.log);
watcher.once('close', function (reason) {
	console.log('[FILE LISTENER]','closed', path, 'because', reason)
	watcher.removeAllListeners()  // as it is closed, no need for our change or log listeners any more
});

watcher.setConfig({
	stat: null,
	interval: 5007,
	persistent: true,
	catchupDelay: 2000,
	preferredMethods: ['watch', 'watchFile'],
	followLinks: true,
	ignorePaths: false,
	ignoreHiddenFiles: false,
	ignoreCommonPatterns: true,
	ignoreCustomPatterns: null
})

watcher.watch(next);

//************ CODE FOR CHUNK LOADING ***********
function sendFiles(){
	//console.log('Sending files...');
	for(var i=0; i<100; i++){
		if(files.length > 0){
			fileToSend = files.pop();
			console.log("[FILE SENDER] File to send: " + fileToSend);

			var upload_req = request.post(url, function (err, resp, body) {
				if (err) {
					console.log(err, resp, body);
				} else {
					//console.log('URL: ' + body);
				}
			});

			var form = upload_req.form();
			form.append('file', fs.createReadStream(fileToSend));
		}
	}
}

