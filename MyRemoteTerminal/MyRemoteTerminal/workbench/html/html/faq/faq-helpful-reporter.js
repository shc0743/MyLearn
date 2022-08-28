(function () {
    let elem = document.createElement('div');
    elem.innerHTML = `
    <div q style="margin-bottom:5px">Is this helpful for you?</div>
    <div c>
        <button y>Yes</button>
        <button n>No</button>
    </div>
    `;
    elem.setAttribute('style', `
    position: fixed;
    left: 0;
    bottom: 0;
    width: 100%;
    height: auto;
    background: rgb(195 228 237);
    text-align: center;
    z-index: 131072;
    padding: 10px;
    `);
    elem.hidden = true;

    elem.querySelector('[y]').onclick = function () {
        var str = `User Experience Report\r\n` +
            `url=${location.href}\r\n` +
            `report=good\r\n` +
            `data=\r\n\r\n`;

        navigator.sendBeacon('/api/faq/helpful-report/report', str);

        elem.innerHTML = `Thanks for your vote.`;
        setTimeout(function () {
            elem.remove();
        }, 3000);
    };

    elem.querySelector('[n]').onclick = function () {
        elem.innerHTML = `
        <div q>Do you have any other comments?</div>
        <textarea a rows=5 style="display:block;width:calc(100% - 24px);margin:5px 0 5px 0;"></textarea>
        <button s style="display:block;width:calc(100% - 20px);padding:5px">Send</button>
        `;
        elem.querySelector('[s]').onclick = function () {
            var str = `User Experience Report\r\n` +
                `url=${location.pathname}\r\n` +
                `report=bad\r\n` +
                `data=${elem.querySelector('[a]').value}\r\n\r\n`;

            navigator.sendBeacon('/api/faq/helpful-report/report', str);

            elem.innerHTML = `Thanks for your feedback.`;
            setTimeout(function () {
                elem.remove();
            }, 3000);
        }
    };

    (document.body || document.documentElement).append(elem);

    setTimeout(function () {
        function F() {
            elem.hidden = false;
        }
        /*document.body.addEventListener('mousemove', F, {
            useCapture: true,
            once: true
        });*/F();
    }, 10000);

}());
