var websocket;

function loadDoc() {
    // get WebSocket URL
    var protocol = location.protocol.indexOf("https") == 0 ? "wss" : "ws"
    var wsurl = protocol + "://" + location.host + /* location.pathname + */"/websocket"

    if ('WebSocket' in window) {
        websocket = new WebSocket(wsurl);
    } else if ('MozWebSocket' in window) {
        websocket = new MozWebSocket(wsurl);
    } else {
        console.log("Does not support WebSocket");
        return;
    }

    // 创建一个websocket连接
    websocket.binaryType = "arraybuffer";
    websocket.onopen = function(){
        console.log("Web Socket connected!");
        websocket.send("hello.");
    };
	websocket.onerror = function(evt){
		console.log("connect ws error.");
	};
	websocket.onclose = function(evt){		
		if(websocket.readyState == websocket.CLOSED){
			console.log('Web Socket closed.' );
		}else{
			console.log('websocket.readyState：' + websocket.readyState );
		}
	};
	
	websocket.onmessage = function (evt) {
        console.log(evt)
        // WebSocket目前发送过来的是TEXT信息
        var obj = JSON.parse(evt.data)
        document.getElementById("result").innerHTML = obj.id;
        document.getElementById("button2").disabled=false;
        document.getElementById("button1").disabled=true;
	};
}

function loadAjaxDoc() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
       var obj = JSON.parse(this.responseText)
       console.log(obj)
       document.getElementById("result").innerHTML = obj.id;
       document.getElementById("button1").disabled=false;
       document.getElementById("button2").disabled=true;
     }
    };
    xhttp.open("GET", "/hello", true);
    xhttp.send();
}