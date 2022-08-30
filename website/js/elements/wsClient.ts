const gateway = `ws://${window.location.hostname}:1337`;
let websocket;

const IS_PLAYING_ATTRIBUTE = 'is-playing';

let volDownButton: HTMLElement | null = null;
let volUpButton: HTMLElement | null = null;
let volLimitCheckbox: HTMLInputElement | null = null;
let volSlider: HTMLInputElement | null = null;
let volRangeMin: HTMLSpanElement | null = null;
let volRangeCurrent: HTMLSpanElement | null = null;
let volRangeMax: HTMLSpanElement | null = null;


let playerCard: HTMLInputElement | null = null;
let playerArtist: HTMLInputElement | null = null;
let playerTrack: HTMLInputElement | null = null;

interface DataInterface {
    volume: number,
    volume_min: number,
    volume_max: number,
    volume_is_limited: boolean,
    is_playing: boolean,
    track_name: string,
    artist_name: string,
}

function getValues(){
    console.log('Sending GET VALUES');
    websocket.send("getValues");
}

export function initWebSocket() {
    console.log(`Trying to open a WebSocket connection on ${gateway}`);
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;

    setupPageElements()
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
    const data = JSON.parse(event.data) as DataInterface;
    console.log('data', data);
    updateVolumeCard(data);
}

function setupPageElements() {
    volDownButton = document.querySelector('.button--volume-down');
    volUpButton = document.querySelector('.button--volume-up');
    volLimitCheckbox = document.querySelector('#limitVolumeCheckbox');
    volSlider = document.querySelector('.volume-card__input');
    volRangeMin = document.querySelector('.volume-group__input-number--min');
    volRangeCurrent = document.querySelector('.volume-group__input-number--current');
    volRangeMax = document.querySelector('.volume-group__input-number--max');

    playerCard = document.querySelector('.media-player');
    playerArtist = document.querySelector('.media-player__title');
    playerTrack = document.querySelector('.media-player__track');

    volDownButton.addEventListener('click', () => {
        console.log('clicked vol down');
        websocket.send("volume_down_button_click");
    });

    volUpButton.addEventListener('click', () => {
        console.log('clicked up down');
        websocket.send("volume_up_button_click");
    });

    volLimitCheckbox.addEventListener('click', () => {
        const limitState = volLimitCheckbox.checked;
        console.log('sending state', limitState);
        websocket.send(limitState ? "volume_limit_checkbox_on" : "volume_limit_checkbox_off");
    });
}

function updateVolumeCard(data: DataInterface) {
    volRangeMin.textContent = String(data.volume_min);
    volRangeMax.textContent = String(data.volume_max);
    volRangeCurrent.textContent = String(data.volume);
    volSlider.setAttribute('max', String(data.volume_max));
    volSlider.value = String(data.volume);
    volSlider.dispatchEvent(new Event('input'));
    volLimitCheckbox.checked = data.volume_is_limited;

    if (data.is_playing) {
        playerCard.setAttribute(IS_PLAYING_ATTRIBUTE, '');
        playerArtist.textContent = data.artist_name;
        playerTrack.textContent = data.track_name;
    } else{
        playerCard.removeAttribute(IS_PLAYING_ATTRIBUTE);
        playerArtist.textContent = '';
        playerTrack.textContent = 'Add a record to start playing';
    }
}