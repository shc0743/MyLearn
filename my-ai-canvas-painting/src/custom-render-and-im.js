const css = new CSSStyleSheet();
css.replace(`
:host {
    display: block;
    margin: 0.5em 0;
}
a {
    color: blue;
    text-decoration: none;
}
a:hover {
    text-decoration: underline;
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
        this.#sr.append('[图片] ', this.#a, ' ', this.#srcmd);

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