var http = require('http'),
    express = require('express'),
    path = require('path'),
    httpProxy = require('http-proxy'),
    proxy = httpProxy.createProxyServer({}),
    WebSocket = require('ws'),
    formidable = require('formidable'),
    fs = require('fs');

var IPCache = 0;
var avai =[];
var responses = [];
var count = 0;
var list;
var IPCacheFile;
var BackendPortfile;
var items = [];
var hit = [];
var list;
var reqFile=[];
var reqCount=[];

// var ipBackend = process.env.IP_BACKEND;
// var BackendPort = 0;

var ipBackend = "172.17.0.1";
var BackendPort = "8100";

var segmentSize = 64000;

require('events').EventEmitter.defaultMaxListeners = 500;

var mpd = express();
var app= express();
mpd.use(function(req,res,next){
	var hostname = req.headers.host.split(":")[0];
	console.log('[WEB SERVER] Requested mpd file ' + req.url);

	res.setHeader('Access-Control-Allow-Origin','*');
	res.setHeader('Access-Control-Allow-Headers','Range');
	res.setHeader('X-Real-IP',hostname);
	res.setHeader('X-Forwarded-For',hostname);

	next();
});
mpd.use(express.static(path.join(__dirname, 'mpd')));
mpd.listen(8687);

console.log("[LOG] MPD download server listening on port 8687");

// ********** SERVER FOR ACCEPTING REQUESTS FROM WEB APPLICATION **********
var server = http.createServer(function(req, res) {

	var mediaFile = req.url.substring(1);
	var mediaFileName = mediaFile.split('?')[0];
	// var mediaFile = req.url.substring(2);
	// console.log('req',req.url);
	// var mediaFileName = mediaFile.split('?')[0];

	var idFile;
	var result = null;
	for (var key in list){
		if (list.hasOwnProperty(key) && list[key].name === mediaFileName){
			result = list[key].value;
			//console.log("\nKEY:" + key + " NAME:" + list[key].name + " VALUE:" + list[key].value + "\n");
			idFile = key;
		}
	}
	req[idFile]++;
	
	console.log("[WEB SERVER] RESULT FOR " + mediaFileName + ": " + result);

	if(result == null){
		console.log("[WEB SERVER] Media '" + mediaFileName + "' does not exist.");
        	res.writeHead(404, {"Content-Type": "text/plain"});
        	res.end("File not found");
		}
	else{
		if (req.method == "OPTIONS") {
			res.writeHead(200,{
				'Access-Control-Allow-Origin': '*',
				'Access-Control-Allow-Headers': 'Content-Type,Range',
				'Access-Control-Allow-Credentials': 'true',
				'Content-Length': 0,
				'Content-Type': 'text/plain',
				'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
				'Host': req.headers.host.split(":")[0]
				});
				res.end();
		}else{
			if (result == 0){
				reqFile.push(mediaFileName);
				//CASE 0: cache miss of file but not already requested at backend
				console.log("[WEB SERVER] Cache miss for file [" + idFile + "] [" + mediaFileName + "], result = " + result);
				list[idFile]['value'] = 1;

			    	responses.push([idFile,req,res]); //Storing the couple (req,res), necessary for proxying
				count++;
				
				wsc.send(idFile); //Request to storage container to obtain file with id: idFile
				}
			else if (result == 1){
				console.log("[WEB SERVER] Cache miss for file [" + idFile + "] [" + mediaFileName + "], result = " + result);
				//CASE 1: cache miss for file but already requested at backend
				
				responses.push([idFile,req,res]);
					count++;
					if(reqCount[idFile]=3){
						fixFile();
					};

			}else{
				//CASE 2: cache hit for file
				console.log("[WEB SERVER] Cache hit for file [" + idFile + "] [" + mediaFileName + "]");
				console.log("[WEB SERVER] Proxy to http://" + req.headers.host.split(":")[0] + ":8688/" + idFile);
				proxy.web(req, res, { target: 'http://' + req.headers.host.split(":")[0] + ':8688/'});
				
			}
		}
	}
}).listen(8686);
console.log("[LOG] Server listening on port 8686");

// ********** SERVER FOR REQUESTED MEDIA FROM CACHE **********

app.use(function(req,res,next){
	var hostname = req.headers.host.split(":")[0];

	res.setHeader('Host',req.headers.host.split(":")[0]);
	res.setHeader('Access-Control-Allow-Origin','*');
	res.setHeader('Access-Control-Allow-Headers','Range');
	res.setHeader('X-Real-IP',hostname);
	res.setHeader('X-Forwarded-For',hostname);

	console.log('[STORAGE SERVER] Proxying file ' + req.url);

	next();
});

app.use(express.static(path.join(__dirname, 'media')));

app.listen(8688);
console.log("[LOG] Storage server listening on port 8688");




// ********** UPLOAD SERVER FOR CHUNK UPLOADING **********

var app_upload = express();

app_upload.post('/upload',function(req,res){
	
	var form = new formidable.IncomingForm();
	form.multiples = true;
	form.uploadDir = path.join(__dirname, 'chunks');
	
	form.on('file', function(field,file) {
		fs.renameSync(file.path,path.join(form.uploadDir, file.name));
		//console.log("FileSize: " + file.size);
		var namePath = 'chunks/' + file.name.toString();  // 'file' format: _3_0.cache
		//console.log("file: " + file.name.toString() + ", field: " + field.toString() + ", path: " + namePath);

		/*
		 *  The code below listen on file added in chunks folder. Everytime a chunk is added, it is inserted in right place in
		 *  corresponding media file. Media file writing is performed everytime a chunk arrives, both in pre-fetching phase and
		 *  in cache miss phase. This increases response time of application, because in cache miss phase the chunks already
		 *  present in storage have already been inserted inside the file.
		 *
		 */

		var fileRead = fs.readFileSync(namePath);
		//The requested file and the segment number are obtained by file name
		var requestedFile=namePath.split('_')[1]; // 'namePath' format: chunks/_3_0.cache
		var segmentNo=namePath.split('_')[2].split('.')[0];
		var offset = segmentNo * segmentSize;

		console.log('[FILE UPLOADER] Adding segment n. ' + segmentNo + ' at offset ' + offset + ', in file ' + requestedFile);
		var writeStream = fs.createWriteStream('./media/' + requestedFile,{flags:'r+', mode: 0777, start: offset})
		writeStream.on('error', function(e) {console.log('WRITESTREAM ERROR: ' + e);});
		writeStream.write(fileRead);
		writeStream.end();

		items[requestedFile][0]++;
		console.log('[FILE UPLOADER] Now file n. ' + requestedFile + ' have ' + items[requestedFile][0] + ' of ' + items[requestedFile][1] + ' chunks\n');
	});

	form.on('error', function(err){
		console.log('[FILE UPLOADER] An error has occurred: ' + err);
	});

	form.on('end', function(){
		res.end('success');
	});

	form.parse(req);

});

app_upload.listen(9601);
console.log("[LOG] Uploader server listening on port 9601");


//**************** WEB SOCKET CLIENT FOR COMMUNCATION WITH BACKEND CONTAINER ****************

//Wrapper for reconnection
function WebSocketClient(){
	this.number = 0;	// Message number
	this.autoReconnectInterval = 2*1000;	// ms between reconnection attempt
}

WebSocketClient.prototype.open = function(url){
	this.url = url;
	this.instance = new WebSocket(this.url);
	this.instance.on('open',()=>{
		this.onopen();
	});
	this.instance.on('message',(data,flags)=>{
		this.number ++;
		this.onmessage(data,flags,this.number);
	});
	this.instance.on('close',(e)=>{
		switch (e){
		case 1000:	// CLOSE_NORMAL
			console.log("[WEB SOCKET CLIENT] WebSocket: closed");
			break;
		default:	// Abnormal closure
			this.reconnect(e);
			break;
		}
		this.onclose(e);
	});
	this.instance.on('error',(e)=>{
		switch (e.code){
		case 'ECONNREFUSED':
			this.reconnect(e);
			break;
		default:
			this.onerror(e);
			break;
		}
	});
}
WebSocketClient.prototype.send = function(data,option){
	try{
		this.instance.send(data,option);
	}catch (e){
		this.instance.emit('error',e);
	}
}
WebSocketClient.prototype.reconnect = function(e){
//	console.log(`WebSocketClient: retry in ${this.autoReconnectInterval}ms`);
//	console.log(e); //Decomment this line if you want informations about connection error.
        this.instance.removeAllListeners();
	var that = this;
	setTimeout(function(){
		console.log("[WEB SOCKET CLIENT] WebSocketClient: reconnecting...");
		wsc.open('ws://' + ipBackend + ':' + BackendPort + '/');
		// that.open('ws://' + IPCache + ':9602/');
//		that.open(that.url);
	},this.autoReconnectInterval);
}

WebSocketClient.prototype.onopen = function(e){console.log("[WEB SOCKET CLIENT] WebSocketClient: open",arguments);}
WebSocketClient.prototype.onmessage = function(data,flags,number){console.log("[WEB SOCKET CLIENT] WebSocketClient: message",arguments);}
WebSocketClient.prototype.onerror = function(e){console.log("[WEB SOCKET CLIENT] WebSocketClient: error",arguments);}
WebSocketClient.prototype.onclose = function(e){console.log("[WEB SOCKET CLIENT] WebSocketClient: closed",arguments);}
//End of wrapping

var wsc = new WebSocketClient();

// wsc.open('ws://' + IPCache + ':9602/');//cambiare l'indirizzo qui
wsc.open('ws://' + ipBackend + ':' + BackendPort + '/');
wsc.onopen = function(e){
	console.log("[WEB SOCKET CLIENT] WebSocketClient connected:",e);
	this.send("hello");
}

wsc.onmessage = function(data,flags,number){

	if(data == 'welcome'){
		console.log("[WEB SOCKET CLIENT] Connection successful: welcome received.");
	}else {

		console.log('[WEB SOCKET CLIENT] WebSocketClient message: ',data);
		var fileRec = data;
	     	console.log("[WEB SOCKET CLIENT] Now it's available file [" + fileRec + "]");
// 		list[fileRec]['value'] = 2;

// 		/*
// 		 * The file is now available. The code below see if in responses array there are hanging requests for the file.
// 		 * For these requests, will be performed proxying to file claimed.
// 		 */
// //		console.log("RESPONSES: ", responses);
// //		console.log("fileRec = " + fileRec);
// 		for (var i = 0; i < count; i++){
// //			console.log("i = " + i + ", count = " + count);
// //			console.log("responses[" + i + "][0] = " + responses[i][0]);
// 			if (responses[i][0] == fileRec){
// 				var x = responses.splice(i,1);
// 				//x[0][1] = req, x[0][2] = res
// 				proxy.web(x[0][1], x[0][2], { target: 'http://' + x[0][1].headers.host.split(":")[0] + ':8688/'});
// 				i--;
// 				count--;
// 			}
// 		}
	}
}

//********** SERVER FOR RESET CACHE STATUS **********
var http = require('http');


http.createServer(function (req, res) {
fixFile();
res.write('fix'); //write a response to the client
res.end(); //end the response
}).listen(8690); //the server object listens on port 

// CODE FOR IP CACHE FILE FILE LOADING
// var timer2 = setInterval(function loadIPCacheFile(){
// 	console.log('[TIMER IP_CACHE.JSON] Trying to load IPCache.json...');
// 	if(fs.existsSync('./IPCache.json')){
// 		IPCacheFile = require('./IPCache.json');
// 		console.log('[TIMER IP_CACHE.JSON] File IPCache.json correctly loaded!');
// 		IPCache = IPCacheFile.IPCache;
// 		console.log('[TIMER IP_CACHE.JSON] IP of cache:' + IPCache);
// 		clearInterval(timer2);
// 	}
// },1000);

//CHECK FOR ALL CHUNKS OF A FILE CORRECTLY LOADED
var timerAllChunks = setInterval(function chechForMedia(){
	//console.log('[FILE CHECKER] Checking for file completely loaded...');
	for( var j in items){
		//If this condition is true, all the chunks have been inserted in the file.
		if(items[j][0] == items[j][1]){
			console.log("[FILE CHECKER] Item " + j + " RECUPERATO: " + items[j][0] + "/" + items[j][1] );
			items[j][0]++;
			
			//File renaming (from 0.mp4 to file_name.mp4)
			var fileName = list[j].name;
			fs.renameSync('./media/' + j,'./media/' + fileName);
			//fs.chownSync('./media/' + fileName,0,0);
			//Pushing inside 'hit' array the 'requestedFile' value.
			hit.push(j);
			var index = reqFile.indexOf(fileName);
			if (index !== -1) reqFile.splice(index, 1);
		}
	}

},2000);

var timer = setInterval(function loadMediaList(){
	console.log('[TIMER MEDIALIST.JSON] Trying to load MediaList.json...');
	if(fs.existsSync('./mlst/MediaList.json')){
		list = require('./mlst/MediaList.json');
		console.log('[TIMER LIST.JSON] File MediaList.json correctly loaded!');
		//console.log('list', list);
		clearInterval(timer);
		}
},1000);

//CODE FOR MEDIALIST FILE LOADING
var timerMediaList = setInterval(function loadMediaList(){
	console.log('[TIMER MEDIALIST.JSON] Trying to load MediaList.json...');
	if(fs.existsSync('./mlst/MediaList.json')){
		list = require('./mlst/MediaList.json');
		console.log('[TIMER MEDIALIST.JSON] File MediaList.json correctly loaded!');
		clearInterval(timerMediaList);

		//For every media file listed in MediaList.json, save chunks number and create file 'key.mp4');
		var num = 0;
		for (var key in list){
			items.push([0,list[key].numChunks]);
			var fileSize = list[key].size;
			var buffer = Buffer.alloc(fileSize,'0');
			console.log('[TIMER MEDIALIST.JSON] File to be written: ./media/' + num);
			fs.writeFile('./media/' + num,buffer,function(err) {
  				if (err) console.log("[TIMER MEDIALIST.JSON] WRITEFILE ERROR: " + err);
			});
			avai[num]=0;
			reqCount[num]=0;
			num++;
		}
	//console.log('medialist', list);	
		//console.log("[TIMER MEDIALIST.JSON] Items array: " + JSON.stringify(items));
	}
},1000);

// CODE FOR IP PORT BACKEND FILE FILE LOADING
// var timer2 = setInterval(function loadBackendPortfile(){
// 	console.log('[TIMER BACKEND_PORT.JSON] Trying to load BackendPort.json...');
// 	if(fs.existsSync('./BackendPort.json')){
// 		BackendPortFile = require('./BackendPort.json');
// 		console.log('[TIMER BACKEND_PORT.JSON] File BackendPort.json correctly loaded!');
// 		BackendPort = BackendPortFile.BackendPort;
// 		console.log('[TIMER BACKEND_PORT.JSON] Port Number of Backend:' + BackendPort);
// 		clearInterval(timer2);
// 	}
// },6000);

var timerHit = setInterval(function(){
	while(hit.length > 0){
		console.log("HIT LENGTH = " + hit.length);
		var value = hit.pop();	
		console.log("HIT VALUE = " + value);
		avai[value]=1;
		list[value]['value'] = 2;

		/*
		 * The file is now available. The code below see if in responses array there are hanging requests for the file.
		 * For these requests, will be performed proxying to file claimed.
		 */
//		console.log("RESPONSES: ", responses);
//		console.log("fileRec = " + fileRec);
		for (var i = 0; i < count; i++){
//			console.log("i = " + i + ", count = " + count);
//			console.log("responses[" + i + "][0] = " + responses[i][0]);
			if (responses[i][0] == value){
				var x = responses.splice(i,1);
				//x[0][1] = req, x[0][2] = res
				proxy.web(x[0][1], x[0][2], { target: 'http://' + x[0][1].headers.host.split(":")[0] + ':8688/'});
				i--;
				count--;
			}
		}
		
	}
},1000);

// var timerPad = setInterval(function(){
// 	j=0;
//  	for(i=0; i<reqFile.length; i++){
// 		for (var key in list){
// 			if (list.hasOwnProperty(key) && list[key].name === reqFile[i]){
// 				rslt = list[key].value;
// 				j = key;
// 				console.log(j);
// 			}
// 		}
// 		if (items[j][0] != items[j][1]){
// 				//If this condition is true, all the chunks have been inserted in the file.
// 		var fileRead = fs.readFileSync("pad.cache");
// 		//The requested file and the segment number are obtained by file name
// 		var segmentNo=items[j][0]
// 		var offset = segmentNo * segmentSize;
		
// 		console.log('[FIX UPLOAD] Adding padding segment n. ' + segmentNo + ' at offset ' + offset + ', in file ' + i);
// 		var writeStream = fs.createWriteStream('./media/' + i,{flags:'r+', mode: 0777, start: offset})
// 		writeStream.on('error', function(e) {console.log('WRITESTREAM ERROR: ' + e);});
// 		writeStream.write(fileRead);
// 		writeStream.end();
		
// 		items[j][0]++;
// 		console.log('[FIX UPLOAD] Now file n. ' + j + ' have ' + items[j][0] + ' of ' + items[j][1] + ' chunks\n');	
// 		}


// 	}

// },5000);

function fixFile(){
	j=0;
	for(i=0; i<reqFile.length; i++){
		for (var key in list){
			if (list.hasOwnProperty(key) && list[key].name === reqFile[i]){
				rslt = list[key].value;
				j = key;
				console.log(j);
			}
		}
		while(items[j][0] < items[j][1]){
				//If this condition is true, all the chunks have been inserted in the file.
		var fileRead = fs.readFileSync("pad.cache");
		//The requested file and the segment number are obtained by file name
		var segmentNo=items[j][0]
		var offset = segmentNo * segmentSize;
		
		console.log('[FIX UPLOAD] Adding padding segment n. ' + segmentNo + ' at offset ' + offset + ', in file ' + i);
		var writeStream = fs.createWriteStream('./media/' + i,{flags:'r+', mode: 0777, start: offset})
		writeStream.on('error', function(e) {console.log('WRITESTREAM ERROR: ' + e);});
		writeStream.write(fileRead);
		writeStream.end();
		
		items[j][0]++;
		console.log('[FIX UPLOAD] Now file n. ' + j + ' have ' + items[j][0] + ' of ' + items[j][1] + ' chunks\n');	
		}


	}
}
