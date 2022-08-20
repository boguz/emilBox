/**
 * Possible values / states for the ActiveId
 */
enum ActiveId {
    Player = 'Player',
    Catalogue = 'Catalogue',
}

/**
 * Control the state of the Tabs element
 * Click on a tabs button show the visible tabs content
 */
class Tabs {
    private container: HTMLElement;
    private activeID: ActiveId;
    private readonly defaultActiveId = ActiveId.Player;
    private readonly tabsButtons: NodeListOf<Element>;
    private readonly tabsContents: NodeListOf<Element>;
    private readonly buttonActiveAttribute = 'active';
    private readonly contentActiveAttribute = 'active';

    constructor(tabsContainer: HTMLElement) {
        this.container = tabsContainer;
        this.tabsButtons = tabsContainer.querySelectorAll('.tabs__button');
        this.tabsContents = tabsContainer.querySelectorAll('.tabs__content');
        this.activeID = this.defaultActiveId;

        if (!this.tabsButtons || !this.tabsContents || this.tabsButtons.length !== this.tabsContents.length) return;

        this.tabsButtons.forEach((tabButton) => {
            tabButton.addEventListener('click', this._handleButtonClick.bind(this));
        })

        this._updateUI();
    }

    /**
     * Update the UI by showing / hiding the correct button and content according to the active id
     * @private
     */
    private _updateUI() {
        // update buttons
        this.tabsButtons.forEach((tabButton) => {
            const buttonID = tabButton.getAttribute('tab-id');
            buttonID === this.activeID
                ? tabButton.setAttribute(this.buttonActiveAttribute, '')
                : tabButton.removeAttribute(this.buttonActiveAttribute);
        })
        // update contents
        this.tabsContents.forEach((tabContent) => {
            const contentID = tabContent.getAttribute('tab-id');
            contentID === this.activeID
                ? tabContent.setAttribute(this.contentActiveAttribute, '')
                : tabContent.removeAttribute(this.contentActiveAttribute);
        })
    }

    /**
     * Update the active id according to the clicked button
     * @param event
     * @private
     */
    private _handleButtonClick(event) {
        const newId = event.target.getAttribute('tab-id');
        this.activeID = ActiveId[newId];
        this._updateUI();
    }
}

/**
 * Get tabs element and create Tabs instance
 */
export function initTabs(): void {
    const tabsContainer = document.querySelector('.tabs') as HTMLElement;
    if (!tabsContainer) return;

    new Tabs(tabsContainer);
}