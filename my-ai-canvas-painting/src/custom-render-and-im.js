const css = new CSSStyleSheet();
css.replace(`
:host {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    padding: 8px 12px;
    background-color: #f5f5f5;
    border-radius: 6px;
    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
    font-size: 14px;
    margin: 0.5em 0;
    box-shadow: 0 1px 3px rgba(0,0,0,0.1);
    transition: all 0.2s ease;
}

.image-placeholder {
    color: #666;
    font-weight: 500;
}

a {
    color: #4285f4;
    text-decoration: none;
    padding: 4px 8px;
    border-radius: 4px;
    transition: all 0.2s ease;
    font-weight: 500;
}

a:hover {
    background-color: rgba(66, 133, 244, 0.1);
    text-decoration: none;
}

a:active {
    background-color: rgba(66, 133, 244, 0.2);
}

.separator {
    color: #ddd;
    user-select: none;
}

:focus-visible {
    outline: 2px solid rgb(159.5, 206.5, 255);
    outline-offset: 1px;
    transition: outline-offset 0s, outline 0s;
}
`);

class HTMLXCanvasApiDrawImCommand extends HTMLElement {
    #sr;
    #a;
    #srcmd;
    constructor() {
        super();
        this.#sr = this.attachShadow({ mode: "open" });
        this.#sr.adoptedStyleSheets.push(css);
        this.#a = document.createElement('a');
        this.#a.href = 'javascript:void(0)';
        this.#a.textContent = '绘制';
        this.#srcmd = document.createElement('a');
        this.#srcmd.href = 'javascript:void(0)';
        this.#srcmd.textContent = '查看源码';
        this.#sr.append('图片 ', this.#a, ' ', this.#srcmd);

        this.#a.addEventListener('click', this.draw.bind(this));
        this.#srcmd.addEventListener('click', this.src.bind(this));
    }

    draw() {
        this.dispatchEvent(new CustomEvent('draw', { detail: this.dataset.cmdIndex, bubbles: true }));
    }

    src() {
        this.dispatchEvent(new CustomEvent('viewsource', { detail: this.dataset.cmdIndex, bubbles: true }));
    }
}

customElements.define('x-canvas-api-draw-im-command', HTMLXCanvasApiDrawImCommand);