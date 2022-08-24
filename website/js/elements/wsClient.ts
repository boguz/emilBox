const gateway = `ws://${window.location.hostname}/ws`;
let websocket;

function getValues(){
    websocket.send("getValues");
}

export function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    getValues();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(event.data);
    const data = JSON.parse(event.data);
    const keys = Object.keys(data);
}