import { initTabs } from "./elements/_tags.js";
import { initInputRange } from "./elements/_inputRange.js";
import { initWebSocket } from "./elements/wsClient.js";

/**
 * Initialize the custom JS for the emilBox
 */
function initEmilBox() {
    initWebSocket();
    initTabs();
    initInputRange();
}

/**
 * Wait for document to load before initializing the custom js
 */
if (document.readyState === "complete" || document.readyState === "interactive") {
    setTimeout(initEmilBox);
} else {
    document.addEventListener("DOMContentLoaded", initEmilBox);
}
