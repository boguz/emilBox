class InputRange {
    private input: HTMLInputElement;

    constructor(input: HTMLInputElement) {
        this.input = input;
        this.input.addEventListener('input', this._updateUI.bind(this));

        this._updateUI();
    }

    private _updateUI() {
        console.log('updating range ui');
        const percentage = this._getPercentageOfValue(+this.input.min, +this.input.max, +this.input.value);
        this.input.style.background = `linear-gradient(to right, var(--color-primary-dark) 0%, var(--color-primary-dark) ${percentage}%, var(--color-primary) ${percentage}%, var(--color-primary) 100%)`
    }

    private _getPercentageOfValue(min: number, max: number, value: number) {
        return (value * 100) / max;
    }
}

export function initInputRange(): void {
    const inputRange = document.querySelector('input[type="range"]') as HTMLInputElement;
    if (!inputRange) return;

    // @ts-ignore
    window.volumeInput = new InputRange(inputRange);
}