import { initTabs } from "./elements/_tags.js";
import { initInputRange } from "./elements/_inputRange.js";

/**
 * Initialize the custom JS for the emilBox
 */
function initEmilBox() {
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
